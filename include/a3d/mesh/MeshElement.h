//
//  MeshElement.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESHELEMENT_H
#define AVARA3D_MESHELEMENT_H

#include <memory>
#include <vector>

#include "a3d/Types.h"
#include "a3d/Math.h"

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

		MeshElement(const std::vector<Vertex>& verticies,
					const std::vector<Face>& faces);
		virtual ~MeshElement();

		/// Internal Member Functions ///

		void 							burnTransform(const math::mat4& transform,
													  bool normals);

		const std::vector<Vertex>& 		vertices() const;
		const std::vector<Face>&		faces() const;

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

		MeshElement();

		/// Protected Member Variables ///

		std::vector<Vertex>				_vertices;
		std::vector<Face>				_faces;
		AABB							_localAABB;
		MeshElementDirtyMask			_dirtyMask;
	};
}

#endif /* AVARA3D_MESHELEMENT_H */
