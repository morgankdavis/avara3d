//
//  MeshElement.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_MESHELEMENT_H
#define AVARA3D_MESH_MESHELEMENT_H

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
#include "a3d/util/bitmask.h"

namespace a3d {

	class Line;
	class Material;
	class Node;
	class GLSLProgram;
	class Renderer;
	class RenderContext;
	class RenderItem;

	class MeshElement {

	public:
		/// Public Lifecycle Functions ///

		MeshElement(VertexLayout layout,
					std::span<const std::byte> vertexBytes,
					uint32_t vertexCount,
					uint16_t vertexStride,
					PrimitiveTopology topology,
					IndexFormat indexFormat,
					std::span<const std::byte> indexBytes,
					uint32_t indexCount);
		virtual ~MeshElement();

		/// Internal Types ///

		enum class DirtyMask : uint32_t {
			None =					0,
			VertexData =			1 << 0,
			IndexData =				1 << 1,
			// AABB?
			All = 					UINT_MAX
		};

		/// Internal Member Functions ///

		PrimitiveTopology				topology() const;

		VertexLayout 					vertexLayout() const;
		uint32_t 						vertexCount() const;
		std::span<const std::byte> 		vertexBytes() const;
		uint16_t 						vertexStride() const;

		IndexFormat						indexFormat() const;
		uint32_t						indexCount() const;
		std::span<const std::byte>		indexBytes() const;

		AABB							localAABB() const;
		AABB							worldAABB(const math::mat4& worldMat,
												  bool vertfit) const;
		math::vec3 						localExtent() const;
		math::vec3 						worldExtent(const math::mat4& worldTransform) const;

		void 							beginBuild(VertexLayout layout,
												   uint16_t vertexStride,
												   PrimitiveTopology topology,
												   IndexFormat indexFormat,
												   uint32_t reserveVerts = 0,
												   uint32_t reserveIndices = 0);
		void 							appendVertexBytes(const void* vertexBytes);
		void 							appendIndex(uint32_t idx);
		void 							appendTriangle(uint32_t a, uint32_t b, uint32_t c);
		void 							endBuild(bool recomputeAABB = true);

		void 							burnTransform(const math::mat4& transform,
													  bool normals);

		DirtyMask 						dirtyMask() const;
		void 							dirtyMask(DirtyMask mask);

	protected:
		/// Protected Member Functions ///

		void							genLocalAABB();

	protected:
		/// Protected Lifecycle ///

		MeshElement();
	protected:

		/// Protected Member Variables ///

		PrimitiveTopology				_topology;
		VertexLayout 					_vertexLayout;
		std::vector<std::byte>			_vertexData;
		uint32_t						_vertexCount;
		uint16_t						_vertexStride;
		IndexFormat						_indexFormat;
		std::vector<std::byte>			_indexData;
		uint32_t						_indexCount;
		AABB							_localAABB;
		DirtyMask						_dirtyMask;
	};

	namespace util::bitmask {
		template <> struct enable_ops<MeshElement::DirtyMask> : std::true_type {};
	}
}

#endif /* AVARA3D_MESH_MESHELEMENT_H */
