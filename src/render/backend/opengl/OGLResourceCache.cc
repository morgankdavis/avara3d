
#include "a3d/render/backend/opengl/OGLResourceCache.h"

#include <type_traits>
#include <variant>

#include "a3d/Buffer.h"
#include "a3d/Configuration.h"
#include "a3d/Image.h"
#include "a3d/CubeImage.h"
#include "a3d/log/Log.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/render/backend/opengl/gl.h"
#include "a3d/render/backend/opengl/GLSLProgram.h"
#include "a3d/visual/material/Material.h"
#include "a3d/visual/material/Sampler.h"
#include "a3d/visual/material/Texture.h"

using namespace a3d;
using namespace std;


static PipelineOGL BuildPipeline(const PipelineKey& key);


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

	auto doMin = forceAll || A3D_MASK_CONTAINS(sm, SamplerDirtyMask::MinificationFilter);
	auto doMag = forceAll || A3D_MASK_CONTAINS(sm, SamplerDirtyMask::MagnificationFilter);
	auto doWS  = forceAll || A3D_MASK_CONTAINS(sm, SamplerDirtyMask::WrapS);
	auto doWT  = forceAll || A3D_MASK_CONTAINS(sm, SamplerDirtyMask::WrapT);
	auto doWR  = forceAll || (isCubemap && A3D_MASK_CONTAINS(sm, SamplerDirtyMask::WrapR));
	auto doAn  = forceAll || A3D_MASK_CONTAINS(sm, SamplerDirtyMask::MaxAnisotropy);

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
		if (doMin) cleared = A3D_MASK_REMOVE(cleared, SamplerDirtyMask::MinificationFilter);
		if (doMag) cleared = A3D_MASK_REMOVE(cleared, SamplerDirtyMask::MagnificationFilter);
		if (doWS)  cleared = A3D_MASK_REMOVE(cleared, SamplerDirtyMask::WrapS);
		if (doWT)  cleared = A3D_MASK_REMOVE(cleared, SamplerDirtyMask::WrapT);
		if (doWR)  cleared = A3D_MASK_REMOVE(cleared, SamplerDirtyMask::WrapR);
		if (doAn)  cleared = A3D_MASK_REMOVE(cleared, SamplerDirtyMask::MaxAnisotropy);
		sampler->dirtyMask(cleared);
	}
}

static inline int SlotFor(MaterialPropertyType t) {
	switch (t) {
		case MaterialPropertyType::Ambient:  return 0;
		case MaterialPropertyType::Diffuse:  return 1;
		case MaterialPropertyType::Specular: return 2;
		case MaterialPropertyType::Emission: return 3;
		default: return -1;
	}
}

// ----------------------------------------------------------------------------


PipelineHandle OGLResourceCache::ensurePipeline(const PipelineKey& key) {

	if (auto it = _pipelineMap.find(key); it != _pipelineMap.end()) return it->second;

	PipelineOGL p = BuildPipeline(key);

	const PipelineHandle h = (PipelineHandle)_pipelineList.size();
	_pipelineList.push_back(std::move(p));
	_pipelineMap.emplace(key, h);

	return h;
}

const PipelineOGL& OGLResourceCache::pipeline(PipelineHandle h) const {
	return _pipelineList.at(h);
}

const MeshElementOGL& OGLResourceCache::ensureMeshElement(MeshElement& element,
														  uint32_t vertexLayoutKey) {
	auto it = _meshElementMap.find(&element);
	if (it == _meshElementMap.end()) {
		it = _meshElementMap.emplace(&element, MeshElementOGL{}).first;
	}

	auto& res = it->second;

	const bool dirty = A3D_MASK_CONTAINS(element.dirtyMask(), MeshElementDirtyMask::VertexData);
	const bool missing = (res.vao == 0);
	const bool layoutChanged = (!missing && res.vertexLayoutKey != vertexLayoutKey);

	if (dirty || missing || layoutChanged) {
		if (!missing) {
			glDeleteBuffers(1, (GLuint*)&res.vbo);
			glDeleteBuffers(1, (GLuint*)&res.ebo);
			glDeleteVertexArrays(1, (GLuint*)&res.vao);
			res = {};
		}

		res.vertexLayoutKey = vertexLayoutKey;

		glGenVertexArrays(1, (GLuint*)&res.vao);
		glGenBuffers(1, (GLuint*)&res.vbo);
		glGenBuffers(1, (GLuint*)&res.ebo);

		glBindVertexArray((GLuint)res.vao);

		glBindBuffer(GL_ARRAY_BUFFER, (GLuint)res.vbo);
		glBufferData(GL_ARRAY_BUFFER,
					 element.vertices().size() * sizeof(Vertex),
					 element.vertices().data(),
					 GL_STATIC_DRAW);

		// TODO: use vertexLayoutKey to pick an attribute layout table.
		// Current hard-coded Vertex layout:
		// 0: position (vec3), 1: normal (vec3), 2: uv (vec2)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(math::vec3));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(math::vec3) * 2));
		glEnableVertexAttribArray(2);

		std::vector<uint32_t> indices;
		indices.reserve(element.faces().size() * 3);
		for (auto& f : element.faces()) {
			indices.push_back((uint32_t)f.a);
			indices.push_back((uint32_t)f.b);
			indices.push_back((uint32_t)f.c);
		}
		res.indexCount = (uint32_t)indices.size();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)res.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					 indices.size() * sizeof(uint32_t),
					 indices.data(),
					 GL_STATIC_DRAW);

		element.dirtyMask(A3D_MASK_REMOVE(element.dirtyMask(), MeshElementDirtyMask::VertexData));
	}

	return res;
}

const MaterialOGL& OGLResourceCache::ensureMaterial(Material& material) {
	auto it = _materialMap.find(&material);
	if (it == _materialMap.end()) it = _materialMap.emplace(&material, MaterialOGL{}).first;
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
		it = _textureMap.emplace(&texture, TextureOGL{}).first;
	}

	unsigned handle = it->second.id;

	const bool contentsDirty =
			A3D_MASK_CONTAINS(texture.dirtyMask(), TextureDirtyMask::Contents);
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
		texture.dirtyMask(A3D_MASK_REMOVE(texture.dirtyMask(), TextureDirtyMask::Contents));
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




PipelineOGL BuildPipeline(const PipelineKey& key) {
	PipelineOGL p;
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
