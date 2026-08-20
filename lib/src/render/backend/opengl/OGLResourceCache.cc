//
//  OGLResourceCache.cc
//  avara3d
//
//  Created by Morgan Davis on 12/31/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/OGLResourceCache.h"

#include <algorithm>
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

using namespace a3d;
using namespace std;

// [Private Static Non-Member Prototypes]

static OGLPipeline BuildPipeline(const PipelineDesc& desc, gl::uint_t program);
static GLenum      GLFilterModeForFilterMode(Sampler::FilterMode mode);
static GLenum      GLWrapModeForWrapMode(Sampler::WrapMode mode);
static bool        UsesMipmaps(Sampler::FilterMode mode);
static void        GLAttribFor(VertexAttribFormat f, GLint& comps, GLenum& type);
static unsigned    BufferTextureContents(const Texture& texture);
static void        ApplySamplerState(Texture&          texture,
                                     unsigned          glTextureHandle,
                                     bool              forceAll,
                                     OGLMemoryTracker& memoryTracker);
static int         SlotFor(Material::PropertyType type);
static GLenum      GLIndexTypeForIndexFormat(IndexFormat format);
static uint64_t    ImageTextureStorageBytes(const Image& image, bool includeMipmaps);
static uint64_t    TextureStorageBytes(const Texture& texture, bool includeMipmaps);

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
    auto& res = it->second;

    res.specularExponent = material.specularExponent();
    res.uvScale = material.uvScale();
    res.tex.fill(0);

    for (auto& [property, type] : material.properties()) {
        const int slot = SlotFor(type);
        if (slot < 0) {
            continue;
        }
        auto textureSP = std::get_if<std::shared_ptr<Texture>>(property);
        if (!textureSP || !(*textureSP)) {
            continue;
        }
        res.tex[(size_t) slot] = ensureTexture(*(*textureSP)).id;
    }

    return res;
}

const OGLTexture& OGLResourceCache::ensureTexture(Texture& texture) {

    auto it = _textureMap.find(&texture);
    if (it == _textureMap.end()) {
        it = _textureMap.emplace(&texture, OGLTexture {}).first;
    }

    unsigned handle = it->second.id;

    const bool contentsDirty = util::bitmask::contains(texture.dirtyMask(), Texture::DirtyMask::Contents);
    const bool missingOrZero = (handle == 0);
    const bool forceAll = contentsDirty || missingOrZero;

    if (forceAll) {
        if (handle != 0) {
            _memoryTracker.removeAllocation({
                OGLMemoryTracker::ObjectNamespace::Texture,
                handle,
            });

            glDeleteTextures(1, (GLuint*) &handle);
            handle = 0;
        }

        handle = BufferTextureContents(texture);

        if (handle == 0) {
            log::e()("BufferTextureContents failed for Texture {:p}", (void*) &texture);
            it->second.id = 0;
            return it->second;
        }

        it->second.id = handle;

        _memoryTracker.setAllocation(
            {
                OGLMemoryTracker::ObjectNamespace::Texture,
                handle,
            },
            OGLMemoryTracker::Source::A3D, OGLMemoryTracker::Category::Texture,
            TextureStorageBytes(texture, false));

        // clear only Contents bit
        texture.dirtyMask(util::bitmask::remove(texture.dirtyMask(), Texture::DirtyMask::Contents));
    }

    // apply sampler state whenever sampler says it’s dirty, and always after (re)upload
    if (handle != 0) {
        const auto sampler = texture.sampler();
        const bool samplerDirty = sampler && sampler->dirtyMask() != (Sampler::DirtyMask) 0;
        if (forceAll || samplerDirty) {
            ApplySamplerState(texture, handle, forceAll, _memoryTracker);
        }
    }

    return it->second;
}

// [Private Static Non-Member Functions]

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

void ApplySamplerState(Texture&          texture,
                       unsigned          glTextureHandle,
                       bool              forceAll,
                       OGLMemoryTracker& memoryTracker) {
    auto sampler = texture.sampler();
    if (!sampler) {
        return;
    }

    const bool   isCubemap = std::holds_alternative<std::shared_ptr<CubeImage>>(texture.contents());
    const GLenum texType = isCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;

    glBindTexture(texType, (GLuint) glTextureHandle);

    const auto sm = sampler->dirtyMask();

    auto doMin = forceAll || util::bitmask::contains(sm, Sampler::DirtyMask::MinificationFilter);
    auto doMag = forceAll || util::bitmask::contains(sm, Sampler::DirtyMask::MagnificationFilter);
    auto doWS = forceAll || util::bitmask::contains(sm, Sampler::DirtyMask::WrapS);
    auto doWT = forceAll || util::bitmask::contains(sm, Sampler::DirtyMask::WrapT);
    auto doWR = forceAll || (isCubemap && util::bitmask::contains(sm, Sampler::DirtyMask::WrapR));
    auto doAn = forceAll || util::bitmask::contains(sm, Sampler::DirtyMask::MaxAnisotropy);

    if (doMin) {
        auto mode = sampler->minificationFilter();
        if (UsesMipmaps(mode)) {
            glGenerateMipmap(texType);

            memoryTracker.setAllocation(
                {
                    OGLMemoryTracker::ObjectNamespace::Texture,
                    glTextureHandle,
                },
                OGLMemoryTracker::Source::A3D, OGLMemoryTracker::Category::Texture,
                TextureStorageBytes(texture, true));
        }
        glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, (GLint) GLFilterModeForFilterMode(mode));
    }

    if (doMag) {
        auto mode = sampler->magnificationFilter();
        // only Nearest/Linear are valid
        glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, (GLint) GLFilterModeForFilterMode(mode));
    }

    if (doWS) {
        glTexParameteri(texType, GL_TEXTURE_WRAP_S, (GLint) GLWrapModeForWrapMode(sampler->wrapS()));
    }
    if (doWT) {
        glTexParameteri(texType, GL_TEXTURE_WRAP_T, (GLint) GLWrapModeForWrapMode(sampler->wrapT()));
    }
    if (doWR) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                        (GLint) GLWrapModeForWrapMode(sampler->wrapR()));
    }

#ifdef A3D_GL_DESKTOP
    if (doAn) {
    #if defined(GL_EXT_texture_filter_anisotropic)
        float anisotropy = sampler->maxAnisotropy();
        float largest = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
        if (largest > 0.0f && anisotropy > largest) {
            anisotropy = largest;
        }
        glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
    #else
        (void) texType; // anisotropy extension not available
    #endif
    }
#endif

    // clear sampler dirty bits we just applied
    if (forceAll) {
        sampler->dirtyMask((Sampler::DirtyMask) 0);
    }
    else {
        auto cleared = sm;
        if (doMin) {
            cleared = util::bitmask::remove(cleared, Sampler::DirtyMask::MinificationFilter);
        }
        if (doMag) {
            cleared = util::bitmask::remove(cleared, Sampler::DirtyMask::MagnificationFilter);
        }
        if (doWS) {
            cleared = util::bitmask::remove(cleared, Sampler::DirtyMask::WrapS);
        }
        if (doWT) {
            cleared = util::bitmask::remove(cleared, Sampler::DirtyMask::WrapT);
        }
        if (doWR) {
            cleared = util::bitmask::remove(cleared, Sampler::DirtyMask::WrapR);
        }
        if (doAn) {
            cleared = util::bitmask::remove(cleared, Sampler::DirtyMask::MaxAnisotropy);
        }
        sampler->dirtyMask(cleared);
    }
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
