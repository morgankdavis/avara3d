//
//  MeshElement.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_MESHELEMENT_H
#define AVARA3D_MESH_MESHELEMENT_H

#include <climits>
#include <cstddef>
#include <memory>
#include <span>
#include <vector>

#include "a3d/Assert.h"
#include "a3d/Math.h"
#include "a3d/mesh/AABB.h"
#include "a3d/mesh/IndexFormats.h"
#include "a3d/mesh/PrimitiveTopology.h"
#include "a3d/mesh/VertexLayout.h"
#include "a3d/util/Bitmask.h"

namespace a3d {

    class Line;
    class Material;
    class Node;
    class GLSLProgram;
    class Renderer;
    class RenderContext;
    class RenderItem;

    /**
     * @brief Owns the vertex and optional index data for one mesh primitive.
     *
     * MeshElement corresponds roughly to what many 3D APIs call a "submesh" or "mesh
     * primitive." SceneKit calls the analogous concept a "geometry element," while
     * glTF uses the term "mesh primitive."
     */
    class MeshElement {

    public:
        // [Public Lifecycle Functions]

        /**
         * @brief Creates a MeshElement by copying the supplied vertex and index data.
         *
         * The input byte spans do not need to outlive the MeshElement. For a
         * non-indexed element, use IndexFormat::None with an index count of zero and
         * an empty index span.
         *
         * @param layout layout of each vertex record.
         * @param vertexBytes source bytes containing at least @p vertexCount records.
         * @param vertexCount number of vertices.
         * @param vertexStride size in bytes of one vertex record.
         * @param topology primitive topology represented by the data.
         * @param indexFormat format of each index, or IndexFormat::None for non-indexed geometry.
         * @param indexBytes source index bytes for indexed geometry.
         * @param indexCount number of indices for indexed geometry.
         * @throws std::runtime_error if a supplied byte span is too small for its declared count and stride.
         */
        MeshElement(VertexLayout               layout,
                    std::span<const std::byte> vertexBytes,
                    uint32_t                   vertexCount,
                    uint16_t                   vertexStride,
                    PrimitiveTopology          topology,
                    IndexFormat                indexFormat,
                    std::span<const std::byte> indexBytes,
                    uint32_t                   indexCount);
        virtual ~MeshElement();

        // [Internal Types]

        enum class DirtyMask : uint32_t {
            None       = 0,
            VertexData = 1 << 0,
            IndexData  = 1 << 1,
            // AABB?
            All = UINT_MAX
        };

        // [Internal Member Functions]

        PrimitiveTopology          topology() const;

        VertexLayout               vertexLayout() const;
        uint32_t                   vertexCount() const;
        std::span<const std::byte> vertexBytes() const;
        uint16_t                   vertexStride() const;

        IndexFormat                indexFormat() const;
        uint32_t                   indexCount() const;
        std::span<const std::byte> indexBytes() const;

        const AABB&                localAABB() const;
        AABB                       worldAABB(const math::mat4& worldMat, bool vertfit) const;
        math::vec3                 localExtent() const;
        math::vec3                 worldExtent(const math::mat4& worldTransform) const;

        void                       beginBuild(VertexLayout      layout,
                                              uint16_t          vertexStride,
                                              PrimitiveTopology topology,
                                              IndexFormat       indexFormat,
                                              uint32_t          reserveVerts   = 0,
                                              uint32_t          reserveIndices = 0);
        void                       appendVertexBytes(const void* vertexBytes);
        void                       appendIndex(uint32_t idx);
        void                       appendTriangle(uint32_t a, uint32_t b, uint32_t c);
        void                       endBuild(bool recomputeAABB = true);

        void                       burnTransform(const math::mat4& transform, bool normals);

        DirtyMask                  dirtyMask() const;
        void                       dirtyMask(DirtyMask mask);

    protected:
        // [Protected Member Functions]

        void genLocalAABB();

        // [Protected Lifecycle]

        MeshElement();

        // [Protected Member Variables]

        PrimitiveTopology      _topology;
        VertexLayout           _vertexLayout;
        std::vector<std::byte> _vertexData;
        uint32_t               _vertexCount;
        uint16_t               _vertexStride;
        IndexFormat            _indexFormat;
        std::vector<std::byte> _indexData;
        uint32_t               _indexCount;
        AABB                   _localAABB;
        DirtyMask              _dirtyMask;
    };

    namespace util::bitmask {

        template<>
        struct enable_ops<MeshElement::DirtyMask> : std::true_type {};

    }
}

#endif // AVARA3D_MESH_MESHELEMENT_H
