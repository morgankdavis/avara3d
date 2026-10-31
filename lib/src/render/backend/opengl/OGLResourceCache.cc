//
//  OGLResourceCache.cc
//  avara3d
//
//  Created by Morgan Davis on 12/31/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/OGLResourceCache.h"

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <variant>

#include "a3d/Assert.h"
#include "a3d/Buffer.h"
#include "a3d/Image.h"
#include "a3d/CubeImage.h"
#include "a3d/log/Log.h"
#include "a3d/mesh/IndexAccess.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/VertexLayout.h"
#include "a3d/mesh/VertexLayoutDesc.h"
#include "a3d/render/backend/opengl/gl.h"
#include "a3d/render/backend/opengl/OGLMemoryTracker.h"
#include "a3d/visual/material/Material.h"
#include "a3d/visual/material/Sampler.h"
#include "a3d/visual/material/Texture.h"
#include "a3d/visual/material/TextureBinding.h"

using namespace std;

namespace a3d {

// [Internal Member Functions]

std::size_t SamplerHash::operator()(const Sampler& sampler) const noexcept {

    std::size_t seed = 0;

    auto combine = [&seed](std::size_t hash) {
        seed ^= hash + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
    };

    combine(std::hash<uint16_t> {}(static_cast<uint16_t>(sampler.minificationFilter())));
    combine(std::hash<uint16_t> {}(static_cast<uint16_t>(sampler.magnificationFilter())));
    combine(std::hash<float> {}(sampler.maxAnisotropy()));
    combine(std::hash<uint16_t> {}(static_cast<uint16_t>(sampler.wrapS())));
    combine(std::hash<uint16_t> {}(static_cast<uint16_t>(sampler.wrapT())));
    combine(std::hash<uint16_t> {}(static_cast<uint16_t>(sampler.wrapR())));

    return seed;
}

namespace {

    // [Private Non-Member Prototypes]

    OGLPipeline BuildPipeline(const PipelineDesc& desc, gl::uint_t program);
    GLenum      GLFilterModeForFilterMode(Sampler::FilterMode mode);
    GLenum      GLWrapModeForWrapMode(Sampler::WrapMode mode);
    bool        UsesMipmaps(Sampler::FilterMode mode);
    void        GLAttribFor(VertexAttribFormat f, GLint& comps, GLenum& type);
    GLenum      GLTargetForTexture(const Texture& texture);
    unsigned    BufferTextureContents(const Texture& texture);
    int         SlotFor(Material::PropertyType type);
    GLenum      GLIndexTypeForIndexFormat(IndexFormat format);
    uint64_t    ImageTextureStorageBytes(const Image& image, bool includeMipmaps);
    uint64_t    TextureStorageBytes(const Texture& texture, bool includeMipmaps);

} // namespace

// [Internal Lifecycle Functions]

OGLResourceCache::OGLResourceCache(OGLMemoryTracker& memoryTracker):
    _memoryTracker {memoryTracker} {}

OGLResourceCache::~OGLResourceCache() {
    log::d()("Destroying OGLResourceCache {:p}", static_cast<void*>(this));

    for (const auto& [element, resource] : _meshElementMap) {
        if (resource.ebo != 0) {
            _memoryTracker.removeAllocation({
                OGLMemoryTracker::ObjectNamespace::Buffer,
                resource.ebo,
            });

            const GLuint ebo = resource.ebo;
            glDeleteBuffers(1, &ebo);
        }

        if (resource.vbo != 0) {
            _memoryTracker.removeAllocation({
                OGLMemoryTracker::ObjectNamespace::Buffer,
                resource.vbo,
            });

            const GLuint vbo = resource.vbo;
            glDeleteBuffers(1, &vbo);
        }

        if (resource.vao != 0) {
            const GLuint vao = resource.vao;
            glDeleteVertexArrays(1, &vao);
        }
    }

    for (const auto& [texture, resource] : _textureMap) {
        if (resource.id != 0) {
            _memoryTracker.removeAllocation({
                OGLMemoryTracker::ObjectNamespace::Texture,
                resource.id,
            });

            const GLuint id = resource.id;
            glDeleteTextures(1, &id);
        }
    }

    for (const auto& entry : _samplerMap) {

        const auto& resource = entry.second;

        if (resource.id != 0) {

            const GLuint id = resource.id;

            glDeleteSamplers(1, &id);
        }
    }
}

// [Internal Member Functions]

PipelineId OGLResourceCache::ensurePipeline(const PipelineDesc& desc, gl::uint_t program) {

    if (auto it = _pipelineMap.find(desc); it != _pipelineMap.end()) {

        return it->second;
    }

    OGLPipeline pipeline = BuildPipeline(desc, program);

    const PipelineId pipelineId = static_cast<PipelineId>(_pipelineList.size());

    _pipelineList.push_back(std::move(pipeline));
    _pipelineMap.emplace(desc, pipelineId);

    return pipelineId;
}

const OGLPipeline& OGLResourceCache::pipeline(PipelineId pipelineId) const {
    return _pipelineList.at(pipelineId);
}

const OGLMeshElement& OGLResourceCache::ensureMeshElement(MeshElement& element) {

    // find/create cache entry
    auto [it, inserted] = _meshElementMap.try_emplace(&element, OGLMeshElement {});
    auto& res = it->second;

    const VertexLayout layout = element.vertexLayout();

    const bool vDirty = util::bitmask::contains(element.dirtyMask(), MeshElement::DirtyMask::VertexData);
    const bool iDirty = util::bitmask::contains(element.dirtyMask(), MeshElement::DirtyMask::IndexData);

    const bool missing = (res.vao == 0);
    const bool layoutChanged = (!missing && res.vertexLayoutKey != layout);

    if (!vDirty && !iDirty && !missing && !layoutChanged) {
        return res;
    }

    // if rebuilding, delete old GL objects
    if (!missing) {
        _memoryTracker.removeAllocation({
            OGLMemoryTracker::ObjectNamespace::Buffer,
            res.vbo,
        });

        _memoryTracker.removeAllocation({
            OGLMemoryTracker::ObjectNamespace::Buffer,
            res.ebo,
        });

        glDeleteBuffers(1, (GLuint*) &res.vbo);
        glDeleteBuffers(1, (GLuint*) &res.ebo);
        glDeleteVertexArrays(1, (GLuint*) &res.vao);
        res = {};
    }

    res.vertexLayoutKey = layout;

    // create GL objects
    glGenVertexArrays(1, (GLuint*) &res.vao);
    glGenBuffers(1, (GLuint*) &res.vbo);
    glGenBuffers(1, (GLuint*) &res.ebo);

    glBindVertexArray((GLuint) res.vao);

    // vertex buffer

    const VertexLayoutDesc& desc = GetVertexLayoutDesc(layout);

    const auto     vb = element.vertexBytes();
    const uint32_t vcount = element.vertexCount();
    const uint16_t stride = element.vertexStride();

    A3D_ASSERT(desc.stride == stride);
    A3D_ASSERT(vb.size() == size_t(vcount) * size_t(stride));

    glBindBuffer(GL_ARRAY_BUFFER, (GLuint) res.vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) vb.size(), (const void*) vb.data(), GL_STATIC_DRAW);

    _memoryTracker.setAllocation(
        {
            OGLMemoryTracker::ObjectNamespace::Buffer,
            res.vbo,
        },
        OGLMemoryTracker::Source::A3D, OGLMemoryTracker::Category::VertexBuffer, vb.size());

    // setup vertex attributes from the descriptor
    for (const auto& a : desc.attribs) {

        GLint      comps = 0;
        GLenum     type = 0;
        const bool isIntegerAttrib = false;

        GLAttribFor(a.format, comps, type);

        glEnableVertexAttribArray(a.location);

        glVertexAttribPointer(a.location, comps, type, a.normalized ? GL_TRUE : GL_FALSE, stride,
                              (const void*) (uintptr_t) a.offset);
    }

    // index buffer

    res.vertexCount = vcount;

    auto ivOpt = IndexAccess::GetIndexStreamView(element);
    if (!ivOpt) {
        // non-indexed mesh
        res.indexCount = 0;
        res.indexType = GL_UNSIGNED_INT;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint) res.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

        _memoryTracker.setAllocation(
            {
                OGLMemoryTracker::ObjectNamespace::Buffer,
                res.ebo,
            },
            OGLMemoryTracker::Source::A3D, OGLMemoryTracker::Category::IndexBuffer, 0);
    }
    else {
        const IndexStreamView iv = *ivOpt;

        A3D_ASSERT(iv.base != nullptr);
        A3D_ASSERT(iv.count == element.indexCount());
        A3D_ASSERT(iv.format == element.indexFormat());
        A3D_ASSERT(IndexStride(iv.format) == 2u || IndexStride(iv.format) == 4u);

        A3D_ASSERT(element.indexBytes().data() == iv.base);
        A3D_ASSERT(element.indexBytes().size() == size_t(iv.count) * size_t(IndexStride(iv.format)));

        res.indexCount = iv.count;
        res.indexType = GLIndexTypeForIndexFormat(iv.format);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint) res.ebo);

        const size_t indexBufferSize = size_t(iv.count) * size_t(IndexStride(iv.format));

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) indexBufferSize, (const void*) iv.base,
                     GL_STATIC_DRAW);

        _memoryTracker.setAllocation(
            {
                OGLMemoryTracker::ObjectNamespace::Buffer,
                res.ebo,
            },
            OGLMemoryTracker::Source::A3D, OGLMemoryTracker::Category::IndexBuffer, indexBufferSize);
    }

    element.dirtyMask(util::bitmask::remove(element.dirtyMask(), MeshElement::DirtyMask::VertexData));
    element.dirtyMask(util::bitmask::remove(element.dirtyMask(), MeshElement::DirtyMask::IndexData));

    return res;
}

const OGLMaterial& OGLResourceCache::ensureMaterial(Material& material) {

    auto it = _materialMap.find(&material);

    if (it == _materialMap.end()) {
        it = _materialMap.emplace(&material, OGLMaterial {}).first;
    }

    auto& resource = it->second;

    resource.specularExponent = material.specularExponent();
    resource.uvScale = material.uvScale();

    resource.bindings.fill(OGLTextureBinding {});

    for (auto& [property, type] : material.properties()) {

        const int slot = SlotFor(type);

        if (slot < 0) {
            continue;
        }

        const auto* binding = get_if<TextureBinding>(property);

        if (!binding) {
            continue;
        }

        const auto& texture = binding->texture();

        if (!texture) {
            continue;
        }

        const bool mipmapsRequired = UsesMipmaps(binding->sampler().minificationFilter());

        const auto& glTexture = ensureTexture(*texture, mipmapsRequired);

        const auto& glSampler = ensureSampler(binding->sampler());

        resource.bindings[static_cast<size_t>(slot)] = {
            .texture = glTexture.id,
            .sampler = glSampler.id,
        };
    }

    return resource;
}

const OGLTexture& OGLResourceCache::ensureTexture(Texture& texture, bool mipmapsRequired) {

    auto  result = _textureMap.try_emplace(&texture, OGLTexture {});
    auto& resource = result.first->second;

    const bool contentsDirty = util::bitmask::contains(texture.dirtyMask(), Texture::DirtyMask::Contents);

    const bool missing = resource.id == 0;

    if (contentsDirty || missing) {

        if (resource.id != 0) {

            _memoryTracker.removeAllocation({
                OGLMemoryTracker::ObjectNamespace::Texture,
                resource.id,
            });

            const GLuint id = resource.id;
            glDeleteTextures(1, &id);

            resource.id = 0;
        }

        const unsigned handle = BufferTextureContents(texture);

        if (handle == 0) {
            log::e()("BufferTextureContents failed for Texture {:p}", static_cast<void*>(&texture));

            resource.mipmapsGenerated = false;
            return resource;
        }

        resource.id = handle;
        resource.mipmapsGenerated = false;

        _memoryTracker.setAllocation(
            {
                OGLMemoryTracker::ObjectNamespace::Texture,
                resource.id,
            },
            OGLMemoryTracker::Source::A3D, OGLMemoryTracker::Category::Texture,
            TextureStorageBytes(texture, false));

        texture.dirtyMask(util::bitmask::remove(texture.dirtyMask(), Texture::DirtyMask::Contents));
    }

    if (mipmapsRequired && resource.id != 0 && !resource.mipmapsGenerated) {

        const GLenum target = GLTargetForTexture(texture);

        if (target == 0) {
            log::e()("Cannot generate mipmaps for empty Texture {:p}", static_cast<void*>(&texture));

            return resource;
        }

        glBindTexture(target, static_cast<GLuint>(resource.id));

        glGenerateMipmap(target);

        resource.mipmapsGenerated = true;

        _memoryTracker.setAllocation(
            {
                OGLMemoryTracker::ObjectNamespace::Texture,
                resource.id,
            },
            OGLMemoryTracker::Source::A3D, OGLMemoryTracker::Category::Texture,
            TextureStorageBytes(texture, true));
    }

    return resource;
}

const OGLSampler& OGLResourceCache::ensureSampler(const Sampler& sampler) {

    if (const auto it = _samplerMap.find(sampler); it != _samplerMap.end()) {
        return it->second;
    }

    GLuint id = 0;
    glGenSamplers(1, &id);

    if (id == 0) {
        throw runtime_error("Unable to create OpenGL sampler.");
    }

    glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER,
                        static_cast<GLint>(GLFilterModeForFilterMode(sampler.minificationFilter())));
    glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER,
                        static_cast<GLint>(GLFilterModeForFilterMode(sampler.magnificationFilter())));
    glSamplerParameteri(id, GL_TEXTURE_WRAP_S, static_cast<GLint>(GLWrapModeForWrapMode(sampler.wrapS())));
    glSamplerParameteri(id, GL_TEXTURE_WRAP_T, static_cast<GLint>(GLWrapModeForWrapMode(sampler.wrapT())));
    glSamplerParameteri(id, GL_TEXTURE_WRAP_R, static_cast<GLint>(GLWrapModeForWrapMode(sampler.wrapR())));

#ifdef A3D_GL_DESKTOP

    #if defined(GL_EXT_texture_filter_anisotropic)

    float anisotropy = sampler.maxAnisotropy();

    float largest = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);

    if (largest > 0.0f && anisotropy > largest) {
        anisotropy = largest;
    }

    glSamplerParameterf(id, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

    #endif

#endif

    const auto [it, inserted] = _samplerMap.emplace(sampler, OGLSampler {.id = id});

    if (!inserted) {
        glDeleteSamplers(1, &id);
    }

    return it->second;
}

namespace {

    // [Private Non-Member Functions]

    OGLPipeline BuildPipeline(const PipelineDesc& desc, gl::uint_t program) {

        OGLPipeline pipeline;

        pipeline.desc = desc;
        pipeline.program = program;

        return pipeline;
    }

    GLenum GLFilterModeForFilterMode(Sampler::FilterMode mode) {
        switch (mode) {
            case Sampler::FilterMode::Nearest:
                return GL_NEAREST;
            case Sampler::FilterMode::Linear:
                return GL_LINEAR;
            case Sampler::FilterMode::NearestMipmapNearest:
                return GL_NEAREST_MIPMAP_NEAREST;
            case Sampler::FilterMode::LinearMipmapNearest:
                return GL_LINEAR_MIPMAP_NEAREST;
            case Sampler::FilterMode::NearestMipmapLinear:
                return GL_NEAREST_MIPMAP_LINEAR;
            case Sampler::FilterMode::LinearMipmapLinear:
                return GL_LINEAR_MIPMAP_LINEAR;
        }
        return GL_LINEAR;
    }

    GLenum GLWrapModeForWrapMode(Sampler::WrapMode mode) {
        switch (mode) {
            case Sampler::WrapMode::ClampToEdge:
                return GL_CLAMP_TO_EDGE;
            case Sampler::WrapMode::Repeat:
                return GL_REPEAT;
            default:
                return GL_MIRRORED_REPEAT;
        }
    }

    bool UsesMipmaps(Sampler::FilterMode mode) {
        switch (mode) {
            case Sampler::FilterMode::NearestMipmapNearest:
            case Sampler::FilterMode::NearestMipmapLinear:
            case Sampler::FilterMode::LinearMipmapNearest:
            case Sampler::FilterMode::LinearMipmapLinear:
                return true;
            default:
                return false;
        }
    }

    void GLAttribFor(VertexAttribFormat f, GLint& comps, GLenum& type) {
        switch (f) {
            case VertexAttribFormat::F32x2:
                comps = 2;
                type = GL_FLOAT;
                break;
            case VertexAttribFormat::F32x3:
                comps = 3;
                type = GL_FLOAT;
                break;
            case VertexAttribFormat::F32x4:
                comps = 4;
                type = GL_FLOAT;
                break;
        }
    }

    GLenum GLTargetForTexture(const Texture& texture) {

        const auto& contents = texture.contents();

        if (holds_alternative<shared_ptr<Image>>(contents)) {
            return GL_TEXTURE_2D;
        }

        if (holds_alternative<shared_ptr<CubeImage>>(contents)) {
            return GL_TEXTURE_CUBE_MAP;
        }

        return 0;
    }

    unsigned BufferTextureContents(const Texture& texture) {
        unsigned glTextureHandle = 0;

        auto contents = texture.contents();
        std::visit(
            [&](auto&& v) {
                using T = std::decay_t<decltype(v)>;

                if constexpr (std::is_same_v<T, shared_ptr<CubeImage>>) {
                    auto cubeImage = dynamic_pointer_cast<CubeImage>(v);
                    if (!cubeImage) {
                        return;
                    }

                    Image* images[] = {cubeImage->face(CubeImage::Face::X_Pos),
                                       cubeImage->face(CubeImage::Face::X_Neg),
                                       cubeImage->face(CubeImage::Face::Y_Pos),
                                       cubeImage->face(CubeImage::Face::Y_Neg),
                                       cubeImage->face(CubeImage::Face::Z_Pos),
                                       cubeImage->face(CubeImage::Face::Z_Neg)};

                    GLenum sides[] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                      GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                      GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

                    glGenTextures(1, (GLuint*) &glTextureHandle);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, (GLuint) glTextureHandle);

                    for (int s = 0; s < 6; ++s) {
                        auto* image = images[s];
                        if (!image) {
                            continue;
                        }
                        // Your code currently assumes 4 bytes/px
                        glTexImage2D(sides[s], 0, GL_RGBA8, image->width(), image->height(), 0, GL_RGBA,
                                     GL_UNSIGNED_BYTE, *(image->buffer()));
                    }
                }
                else if constexpr (std::is_same_v<T, std::shared_ptr<Image>>) {
                    auto image = std::dynamic_pointer_cast<Image>(v);
                    if (!image) {
                        return;
                    }

                    glGenTextures(1, (GLuint*) &glTextureHandle);
                    glBindTexture(GL_TEXTURE_2D, (GLuint) glTextureHandle);

                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->width(), image->height(), 0, GL_RGBA,
                                 GL_UNSIGNED_BYTE, *(image->buffer()));
                }
                else if constexpr (std::is_same_v<T, std::monostate>) {
                    log::w()("Texture has empty contents.");
                }
            },
            contents);

        return glTextureHandle;
    }

    int SlotFor(Material::PropertyType type) {
        switch (type) {
            case Material::PropertyType::Ambient:
                return 0;
            case Material::PropertyType::Diffuse:
                return 1;
            case Material::PropertyType::Specular:
                return 2;
            case Material::PropertyType::Emission:
                return 3;
            default:
                return -1;
        }
    }

    GLenum GLIndexTypeForIndexFormat(IndexFormat format) {
        switch (format) {
            case IndexFormat::U16:
                return GL_UNSIGNED_SHORT;
            case IndexFormat::U32:
                return GL_UNSIGNED_INT;
            default:
                return GL_UNSIGNED_INT;
        }
    }

    uint64_t ImageTextureStorageBytes(const Image& image, bool includeMipmaps) {

        uint64_t width = image.width();
        uint64_t height = image.height();

        if (width == 0 || height == 0) {
            return 0;
        }

        constexpr uint64_t BYTES_PER_PIXEL = 4;

        uint64_t bytes = 0;

        while (true) {
            bytes += width * height * BYTES_PER_PIXEL;

            if (!includeMipmaps || (width == 1 && height == 1)) {
                break;
            }

            width = math::max<uint64_t>(1, width / 2);
            height = math::max<uint64_t>(1, height / 2);
        }

        return bytes;
    }

    uint64_t TextureStorageBytes(const Texture& texture, bool includeMipmaps) {

        return std::visit(
            [includeMipmaps](const auto& contents) -> uint64_t {
                using T = std::decay_t<decltype(contents)>;

                if constexpr (std::is_same_v<T, std::shared_ptr<Image>>) {
                    if (!contents) {
                        return 0;
                    }

                    return ImageTextureStorageBytes(*contents, includeMipmaps);
                }
                else if constexpr (std::is_same_v<T, shared_ptr<CubeImage>>) {
                    if (!contents) {
                        return 0;
                    }

                    const Image* images[] = {
                        contents->face(CubeImage::Face::X_Pos), contents->face(CubeImage::Face::X_Neg),
                        contents->face(CubeImage::Face::Y_Pos), contents->face(CubeImage::Face::Y_Neg),
                        contents->face(CubeImage::Face::Z_Pos), contents->face(CubeImage::Face::Z_Neg),
                    };

                    uint64_t bytes = 0;

                    for (const Image* image : images) {
                        if (image) {
                            bytes += ImageTextureStorageBytes(*image, includeMipmaps);
                        }
                    }

                    return bytes;
                }
                else {
                    return 0;
                }
            },
            texture.contents());
    }

} // namespace
} // namespace a3d
