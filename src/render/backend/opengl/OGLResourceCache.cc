//
//  OGLResourceCache.cc
//  avara3d
//
//  Created by Morgan Davis on 12/31/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/OGLResourceCache.h"

#include <type_traits>
#include <variant>

#include "a3d/Assert.h"
#include "a3d/Buffer.h"
#include "a3d/Image.h"
#include "a3d/CubeImage.h"
#include "a3d/log/Log.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/VertexLayout.h"
#include "a3d/render/VertexLayoutDesc.h"
#include "a3d/render/backend/opengl/gl.h"
#include "a3d/render/backend/opengl/GLSLProgram.h"
#include "a3d/visual/material/Material.h"
#include "a3d/visual/material/Sampler.h"
#include "a3d/visual/material/Texture.h"

using namespace a3d;
using namespace std;


static OGLPipeline BuildPipeline(const PipelineKey& key);


// ---- Texture helpers (private to this TU) -----------------------------------

static GLenum GLFilterModeForFilterMode(FilterMode mode) {
	switch (mode) {
		case FilterMode::Nearest: 				return GL_NEAREST;
		case FilterMode::Linear: 				return GL_LINEAR;
		case FilterMode::NearestMipmapNearest:	return GL_NEAREST_MIPMAP_NEAREST;
		case FilterMode::LinearMipmapNearest: 	return GL_LINEAR_MIPMAP_NEAREST;
		case FilterMode::NearestMipmapLinear: 	return GL_NEAREST_MIPMAP_LINEAR;
		case FilterMode::LinearMipmapLinear: 	return GL_LINEAR_MIPMAP_LINEAR;
	}
	return GL_LINEAR;
}

static GLenum GLWrapModeForWrapMode(WrapMode mode) {
	switch (mode) {
		case WrapMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
		case WrapMode::Repeat:      return GL_REPEAT;
		default:                    return GL_MIRRORED_REPEAT;
	}
}

static inline bool UsesMipmaps(FilterMode mode) {
	switch (mode) {
		case FilterMode::NearestMipmapNearest:
		case FilterMode::NearestMipmapLinear:
		case FilterMode::LinearMipmapNearest:
		case FilterMode::LinearMipmapLinear:
			return true;
		default:
			return false;
	}
}

static void GLAttribFor(VertexFormat f, GLint& comps, GLenum& type) {
	switch (f) {
		case VertexFormat::F32x2: comps = 2; type = GL_FLOAT; break;
		case VertexFormat::F32x3: comps = 3; type = GL_FLOAT; break;
		case VertexFormat::F32x4: comps = 4; type = GL_FLOAT; break;
	}
}

static unsigned BufferTextureContents(const Texture& texture) {
	unsigned glTextureHandle = 0;

	auto contents = texture.contents();
	std::visit([&](auto&& v) {
		using T = std::decay_t<decltype(v)>;

		if constexpr (std::is_same_v<T, shared_ptr<CubeImage>>) {
			auto cubeImage = dynamic_pointer_cast<CubeImage>(v);
			if (!cubeImage) return;

			Image* images[] = {
					cubeImage->posX(), cubeImage->negX(),
					cubeImage->posY(), cubeImage->negY(),
					cubeImage->posZ(), cubeImage->negZ()
			};

			GLenum sides[] = {
					GL_TEXTURE_CUBE_MAP_POSITIVE_X,
					GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
					GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
					GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
			};

			glGenTextures(1, (GLuint*)&glTextureHandle);
			glBindTexture(GL_TEXTURE_CUBE_MAP, (GLuint)glTextureHandle);

			for (int s = 0; s < 6; ++s) {
				auto* image = images[s];
				if (!image) continue;
				// Your code currently assumes 4 bytes/px
				glTexImage2D(sides[s],
							 0,
							 GL_RGBA8,
							 image->width(),
							 image->height(),
							 0,
							 GL_RGBA,
							 GL_UNSIGNED_BYTE,
							 *(image->buffer()));
			}
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<Image>>) {
			auto image = std::dynamic_pointer_cast<Image>(v);
			if (!image) return;

			glGenTextures(1, (GLuint*)&glTextureHandle);
			glBindTexture(GL_TEXTURE_2D, (GLuint)glTextureHandle);

			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 GL_RGBA8,
						 image->width(),
						 image->height(),
						 0,
						 GL_RGBA,
						 GL_UNSIGNED_BYTE,
						 *(image->buffer()));
		}
		else if constexpr (std::is_same_v<T, std::monostate>) {
			log::w()("Texture has empty contents.");
		}
	}, contents);

	return glTextureHandle;
}

static void ApplySamplerState(Texture& texture, unsigned glTextureHandle, bool forceAll) {
	auto sampler = texture.sampler();
	if (!sampler) return;

	const bool isCubemap = std::holds_alternative<std::shared_ptr<CubeImage>>(texture.contents());
	const GLenum texType = isCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;

	glBindTexture(texType, (GLuint)glTextureHandle);

	const auto sm = sampler->dirtyMask();

	auto doMin = forceAll || util::bitmask::contains(sm, SamplerDirtyMask::MinificationFilter);
	auto doMag = forceAll || util::bitmask::contains(sm, SamplerDirtyMask::MagnificationFilter);
	auto doWS  = forceAll || util::bitmask::contains(sm, SamplerDirtyMask::WrapS);
	auto doWT  = forceAll || util::bitmask::contains(sm, SamplerDirtyMask::WrapT);
	auto doWR  = forceAll || (isCubemap && util::bitmask::contains(sm, SamplerDirtyMask::WrapR));
	auto doAn  = forceAll || util::bitmask::contains(sm, SamplerDirtyMask::MaxAnisotropy);

	if (doMin) {
		auto mode = sampler->minificationFilter();
		if (UsesMipmaps(mode)) {
			glGenerateMipmap(texType);
		}
		glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, (GLint)GLFilterModeForFilterMode(mode));
	}

	if (doMag) {
		auto mode = sampler->magnificationFilter();
		// Only Nearest/Linear are valid
		glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, (GLint)GLFilterModeForFilterMode(mode));
	}

	if (doWS) glTexParameteri(texType, GL_TEXTURE_WRAP_S, (GLint)GLWrapModeForWrapMode(sampler->wrapS()));
	if (doWT) glTexParameteri(texType, GL_TEXTURE_WRAP_T, (GLint)GLWrapModeForWrapMode(sampler->wrapT()));
	if (doWR) glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (GLint)GLWrapModeForWrapMode(sampler->wrapR()));

#ifdef A3D_GL_DESKTOP
	if (doAn) {
#if defined(GL_EXT_texture_filter_anisotropic)
		float anisotropy = sampler->maxAnisotropy();
		float largest = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
		if (largest > 0.0f && anisotropy > largest) anisotropy = largest;
		glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
#else
		(void)texType; // anisotropy extension not available
#endif
	}
#endif

	// Clear sampler dirty bits we just applied
	if (forceAll) {
		sampler->dirtyMask((SamplerDirtyMask)0);
	} else {
		auto cleared = sm;
		if (doMin) cleared = util::bitmask::remove(cleared, SamplerDirtyMask::MinificationFilter);
		if (doMag) cleared = util::bitmask::remove(cleared, SamplerDirtyMask::MagnificationFilter);
		if (doWS)  cleared = util::bitmask::remove(cleared, SamplerDirtyMask::WrapS);
		if (doWT)  cleared = util::bitmask::remove(cleared, SamplerDirtyMask::WrapT);
		if (doWR)  cleared = util::bitmask::remove(cleared, SamplerDirtyMask::WrapR);
		if (doAn)  cleared = util::bitmask::remove(cleared, SamplerDirtyMask::MaxAnisotropy);
		sampler->dirtyMask(cleared);
	}
}

static inline int SlotFor(Material::PropertyType t) {
	switch (t) {
		case Material::PropertyType::Ambient:  return 0;
		case Material::PropertyType::Diffuse:  return 1;
		case Material::PropertyType::Specular: return 2;
		case Material::PropertyType::Emission: return 3;
		default: return -1;
	}
}

// ----------------------------------------------------------------------------


PipelineHandle OGLResourceCache::ensurePipeline(const PipelineKey& key) {

	if (auto it = _pipelineMap.find(key); it != _pipelineMap.end()) return it->second;

	OGLPipeline p = BuildPipeline(key);

	const PipelineHandle h = (PipelineHandle)_pipelineList.size();
	_pipelineList.push_back(std::move(p));
	_pipelineMap.emplace(key, h);

	return h;
}

const OGLPipeline& OGLResourceCache::pipeline(PipelineHandle h) const {
	return _pipelineList.at(h);
}

const OGLMeshElement& OGLResourceCache::ensureMeshElement(MeshElement& element) {

	// Find/create cache entry
	auto [it, inserted] = _meshElementMap.try_emplace(&element, OGLMeshElement{});
	auto& res = it->second;

	const VertexLayout layout = element.vertexLayout();

	const bool dirty  = util::bitmask::contains(element.dirtyMask(), MeshElementDirtyMask::VertexData);
	const bool missing = (res.vao == 0);
	const bool layoutChanged = (!missing && res.vertexLayoutKey != layout);

	if (!dirty && !missing && !layoutChanged) {
		return res;
	}

	// If rebuilding, delete old GL objects
	if (!missing) {
		glDeleteBuffers(1, (GLuint*)&res.vbo);
		glDeleteBuffers(1, (GLuint*)&res.ebo);
		glDeleteVertexArrays(1, (GLuint*)&res.vao);
		res = {};
	}

	res.vertexLayoutKey = layout;

	// Create GL objects
	glGenVertexArrays(1, (GLuint*)&res.vao);
	glGenBuffers(1, (GLuint*)&res.vbo);
	glGenBuffers(1, (GLuint*)&res.ebo);

	glBindVertexArray((GLuint)res.vao);

	// --- Vertex buffer ---
	const VertexLayoutDesc& desc = GetVertexLayoutDesc(layout);

	const auto vb = element.vertexBytes();           // span<const std::byte>
	const uint32_t vcount = element.vertexCount();
	const uint16_t stride = element.vertexStride();

	// Hard sanity checks (these will save your life)
	// If you don’t have A3D_ASSERT, use assert().
	A3D_ASSERT(desc.stride == stride);
	A3D_ASSERT(vb.size() == size_t(vcount) * size_t(stride));

	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)res.vbo);
	glBufferData(GL_ARRAY_BUFFER,
				 (GLsizeiptr)vb.size(),
				 (const void*)vb.data(),
				 GL_STATIC_DRAW);

	// Set up vertex attributes from the descriptor
	for (const auto& a : desc.attribs) {

		GLint comps = 0;
		GLenum type = 0;
		const bool isIntegerAttrib = false; // change later if you add integer formats

		GLAttribFor(a.format, comps, type);

		glEnableVertexAttribArray(a.location);

		// If you ever add true integer formats, use glVertexAttribIPointer for those.
		glVertexAttribPointer(a.location,
							  comps,
							  type,
							  a.normalized ? GL_TRUE : GL_FALSE,
							  stride,
							  (const void*)(uintptr_t)a.offset);
	}

	// --- Index buffer ---
	std::vector<uint32_t> indices;
	indices.reserve(element.faces().size() * 3);
	for (const auto& f : element.faces()) {
		indices.push_back((uint32_t)f.a);
		indices.push_back((uint32_t)f.b);
		indices.push_back((uint32_t)f.c);
	}
	res.indexCount = (uint32_t)indices.size();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)res.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 (GLsizeiptr)(indices.size() * sizeof(uint32_t)),
				 indices.data(),
				 GL_STATIC_DRAW);

	// Clear dirty bit
	element.dirtyMask(util::bitmask::remove(element.dirtyMask(), MeshElementDirtyMask::VertexData));

	return res;
}

const OGLMaterial& OGLResourceCache::ensureMaterial(Material& material) {
	auto it = _materialMap.find(&material);
	if (it == _materialMap.end()) it = _materialMap.emplace(&material, OGLMaterial{}).first;
	auto& res = it->second;

	res.specularExponent = material.specularExponent();
	res.uvScale = material.uvScale();
	res.tex.fill(0);

	for (auto& [property, type] : material.properties()) {
		const int slot = SlotFor(type);
		if (slot < 0) continue;
		auto textureSP = std::get_if<std::shared_ptr<Texture>>(property);
		if (!textureSP || !(*textureSP)) continue;
		res.tex[(size_t)slot] = ensureTexture(*(*textureSP));
	}

	return res;
}

unsigned OGLResourceCache::ensureTexture(Texture& texture) {
	auto it = _textureMap.find(&texture);
	if (it == _textureMap.end()) {
		it = _textureMap.emplace(&texture, OGLTexture{}).first;
	}

	unsigned handle = it->second.id;

	const bool contentsDirty =
			util::bitmask::contains(texture.dirtyMask(), TextureDirtyMask::Contents);
	const bool missingOrZero = (handle == 0);
	const bool forceAll = contentsDirty || missingOrZero;

	if (forceAll) {
		if (handle != 0) {
			glDeleteTextures(1, (GLuint*)&handle);
			handle = 0;
		}

		handle = BufferTextureContents(texture);

		if (handle == 0) {
			log::e()("ensureTexture: BufferTextureContents failed for Texture {:p}",
					 (void*)&texture);
			_textureMap.erase(it);
			return 0;
		}

		it->second.id = handle;

		// Clear only Contents bit
		texture.dirtyMask(util::bitmask::remove(texture.dirtyMask(), TextureDirtyMask::Contents));
	}

	// Apply sampler state whenever sampler says it’s dirty, and always after (re)upload
	if (handle != 0) {
		const auto sampler = texture.sampler();
		const bool samplerDirty = sampler && sampler->dirtyMask() != (SamplerDirtyMask)0;
		if (forceAll || samplerDirty) {
			ApplySamplerState(texture, handle, forceAll);
		}
	}

	return handle;
}




OGLPipeline BuildPipeline(const PipelineKey& key) {
	OGLPipeline p;
	p.key = key;
	p.key.doubleSided = key.doubleSided;
	p.key.fillMode = key.fillMode;
	p.key.blendFunction = key.blendFunction;
//	p.key.depthWrite = true;

	// For now assume depth always on for your main pass:
	p.key.depthTest  = true;

	// TODO: temporary?
	switch (p.key.shaderKind) {
		case ShaderKind::Skybox:
			p.program = GLSLProgram::Skybox().glID();
			break;
		case ShaderKind::Wireframe:
			p.program = GLSLProgram::Wireframe().glID();
			break;
		case ShaderKind::Lines:
			p.program = GLSLProgram::Lines().glID();
			break;
		default:
			p.program = GLSLProgram::Default().glID();
			break;
	}

	// p.program = compileProgramForKey(key); // you already had this placeholder

	return p;
}
