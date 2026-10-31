//
//  OGLResourceCache.h
//  avara3d
//
//  Created by Morgan Davis on 12/31/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_OGLRESOURCECACHE_H
#define AVARA3D_RENDER_BACKEND_OPENGL_OGLRESOURCECACHE_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "a3d/render/PipelineDesc.h"
#include "a3d/render/backend/opengl/GLTypes.h"
#include "a3d/visual/material/Sampler.h"

namespace a3d {

class Material;
class MeshElement;
class OGLMemoryTracker;
class Texture;

// [Internal Types]

struct OGLPipeline {
    PipelineDesc desc    = {};
    gl::uint_t   program = 0;
};

struct OGLMeshElement {
    gl::uint_t   vao             = 0;
    gl::uint_t   vbo             = 0;
    gl::uint_t   ebo             = 0;
    gl::enum_t   indexType       = gl::value::unsigned_int;
    uint32_t     indexCount      = 0;1
    uint32_t     vertexCount     = 0;
    VertexLayout vertexLayoutKey = VertexLayout::None;
};

struct OGLTexture {
    gl::uint_t id               = 0;
    bool       mipmapsGenerated = false;
};

struct OGLSampler {
    gl::uint_t id = 0;
};

struct OGLTextureBinding {
    gl::uint_t texture = 0;
    gl::uint_t sampler = 0;
};

static constexpr size_t NUM_MATERIAL_PROPERTY_SLOTS = 4;

struct OGLMaterial {
    std::array<OGLTextureBinding, NUM_MATERIAL_PROPERTY_SLOTS> bindings {};
    float                                                      specularExponent = 64.0f;
    float                                                      uvScale          = 1.0f;
};

struct SamplerHash {
    std::size_t operator()(const Sampler& sampler) const noexcept;
};

class OGLResourceCache {

public:
    // [Internal Lifecycle Functions]

    explicit OGLResourceCache(OGLMemoryTracker& memoryTracker);

    OGLResourceCache(const OGLResourceCache&)            = delete;
    OGLResourceCache& operator=(const OGLResourceCache&) = delete;

    OGLResourceCache(OGLResourceCache&&)            = delete;
    OGLResourceCache& operator=(OGLResourceCache&&) = delete;

    ~OGLResourceCache();

    // [Internal Member Functions]

    PipelineId            ensurePipeline(const PipelineDesc& desc, gl::uint_t program);
    const OGLPipeline&    pipeline(PipelineId pipelineId) const;
    const OGLMeshElement& ensureMeshElement(MeshElement& element);
    const OGLMaterial&    ensureMaterial(Material& material);
    const OGLTexture&     ensureTexture(Texture& texture, bool mipmapsRequired = false);
    const OGLSampler&     ensureSampler(const Sampler& sampler);

private:
    // [Private Member Variables]

    OGLMemoryTracker&                                              _memoryTracker;
    std::unordered_map<PipelineDesc, PipelineId, PipelineDescHash> _pipelineMap;
    std::vector<OGLPipeline>                                       _pipelineList;
    std::unordered_map<MeshElement*, OGLMeshElement>               _meshElementMap;
    std::unordered_map<Material*, OGLMaterial>                     _materialMap;
    std::unordered_map<Texture*, OGLTexture>                       _textureMap;
    std::unordered_map<Sampler, OGLSampler, SamplerHash>           _samplerMap;
};

} // namespace a3d

#endif // AVARA3D_RENDER_BACKEND_OPENGL_OGLRESOURCECACHE_H
