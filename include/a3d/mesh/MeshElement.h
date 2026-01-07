//
//  MeshElement.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESHELEMENT_H
#define AVARA3D_MESHELEMENT_H

#include <cstddef>
#include <memory>
#include <span>
#include <vector>

#include "a3d/Assert.h"
#include "a3d/Types.h"
#include "a3d/Math.h"
#include "a3d/mesh/AABB.h"
#include "a3d/mesh/IndexTypes.h"

#include "a3d/mesh/VertexLayout.h"

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

//		MeshElement(const std::vector<Vertex>& verticies,
//					const std::vector<Face>& faces);
//// TODO: remove
//		MeshElement(VertexLayout layout,
//					std::span<const std::byte> bytes,
//					uint32_t vertexCount,
//					uint16_t stride,
//					std::span<const Face> faces);
		MeshElement(VertexLayout layout,
					std::span<const std::byte> vertexBytes,
					uint32_t vertexCount,
					uint16_t vertexStride,
					PrimitiveTopology topology,
					IndexFormat indexFormat,
					std::span<const std::byte> indexBytes,
					uint32_t indexCount);
		virtual ~MeshElement();

		/// Internal Member Functions ///











//		const std::vector<Vertex>& 		vertices() const;
//		const std::vector<Face>&		faces() const; // PNT-only legacy path

		void 							burnTransform(const math::mat4& transform,
													  bool normals);

		AABB							localAABB() const;
		AABB							worldAABB(const math::mat4& worldMat,
												  bool vertfit) const;
		math::vec3 						localExtent() const;
		math::vec3 						worldExtent(const math::mat4& worldTransform) const;

		MeshElementDirtyMask 			dirtyMask() const;
		void 							dirtyMask(MeshElementDirtyMask mask);

	protected:
		/// Protected Member Functions ///

		void							genLocalAABB();

		/// Protected Lifecycle ///

	public: // ******************** TEMPORARY *******************
		MeshElement();
	protected:
//		MeshElement(VertexLayout layout,
//					std::vector<std::byte>& vertexData,
//					uint32_t vertexCount,
//					uint16_t vertexStride,
//					std::vector<Face>& faces);

		/// Protected Member Variables ///


//		std::vector<Vertex>				_vertices;
//		std::vector<Face>				_faces;
		AABB							_localAABB;
		MeshElementDirtyMask			_dirtyMask;





//		// TODO: gross.
//		template <class TVertex>
//		void appendVertex(const TVertex& v) {
//			static_assert(std::is_trivially_copyable_v<TVertex>);
//			A3D_ASSERT(_vertexStride == sizeof(TVertex));
//			const std::byte* p = reinterpret_cast<const std::byte*>(&v);
//			_vertexData.insert(_vertexData.end(), p, p + sizeof(TVertex));
//			++_vertexCount;
//		}
//
//		void reserveVertices(uint32_t count) {
//			_vertexData.reserve(size_t(count) * size_t(_vertexStride));
//		}








	public:

		VertexLayout 					vertexLayout() const;
//		void 							vertexLayout(VertexLayout layout);

//		void setVertexData(VertexLayout layout,
//						   std::span<const std::byte> bytes,
//						   uint32_t vertexCount,
//						   uint16_t stride);
//
//		template <class TVertex>
//		void setVertices(VertexLayout layout, const std::vector<TVertex>& verts) {
//			static_assert(std::is_trivially_copyable_v<TVertex>);
//			auto s = std::span<const TVertex>(verts.data(), verts.size());
//			setVertexData(layout,
//						  std::as_bytes(s),
//						  (uint32_t)verts.size(),
//						  (uint16_t)sizeof(TVertex));
//		}
//
//		void setFaces(std::span<const Face> faces);


		uint32_t 						vertexCount() const;
		std::span<const std::byte> 		vertexBytes() const;
		uint16_t 						vertexStride() const;



		VertexLayout 					_layout;

		std::vector<std::byte>			_vertexData;
		uint32_t						_vertexCount = 0;
		uint16_t						_vertexStride = 0;

		PrimitiveTopology				topology() const;
		IndexFormat						indexFormat() const;
		uint32_t						indexCount() const;
		std::span<const std::byte>		indexBytes() const;






		PrimitiveTopology				_topology = PrimitiveTopology::Triangles;
		IndexFormat						_indexFormat = IndexFormat::None;
		std::vector<std::byte>			_indexData;
		uint32_t						_indexCount = 0; // number of indices (NOT triangles)






	protected:
		// Call once at start of derived constructor
		void beginBuild(VertexLayout layout,
						uint16_t vertexStride,
						PrimitiveTopology topology = PrimitiveTopology::Triangles,
						IndexFormat indexFormat = IndexFormat::U32,
						uint32_t reserveVerts = 0,
						uint32_t reserveIndices = 0);

		// Append exactly one vertex (stride bytes)
		void appendVertexBytes(const void* vertexBytes);

		// Append one face
//		void appendFace(const Face& f);

		void appendIndex(uint32_t idx);
		void appendTriangle(uint32_t a, uint32_t b, uint32_t c);

		// Call once at end (computes AABB, marks dirty, etc.)
		void endBuild(bool recomputeAABB = true);


	};
}

#endif /* AVARA3D_MESHELEMENT_H */
