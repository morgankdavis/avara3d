//
//  MeshElement.h
//	avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef GeometryElement_h
#define GeometryElement_h


#include <memory>
#include <vector>

#include "glm/glm.hpp"

#include "a3d/Types.h"


namespace a3d {


	class Material;
	class Node;
	class Program;
	class Renderer;


	class MeshElement : public std::enable_shared_from_this<MeshElement> {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		MeshElement(const std::vector<Vertex>& verticies,
					const std::vector<Face>& faces);
		~MeshElement();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 							draw(Renderer& renderer,
											 Material& material,
											 const glm::mat4& modelMat,
											 const glm::mat4& viewMat,
											 const glm::mat4& projectionMat,
											 const DebugOptions& debugOptions,
											 Stats& stats);

		void 							burnTransform(const glm::mat4& transform,
													  bool normals);

		const std::vector<Vertex>& 		vertices() const;
		const std::vector<Face>&		faces() const;

		AABB							aabb(const std::shared_ptr<Node> convertToNode = nullptr) const;
		glm::vec3 						extent(const std::shared_ptr<Node> convertToNode = nullptr) const;

		MeshElementDirtyMask 			dirtyMask() const;
		void 							dirtyMask(MeshElementDirtyMask mask);

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		std::vector<Vertex>				_vertices;
		std::vector<Face>				_faces;

		MeshElementDirtyMask			_dirtyMask;
	};
}


#endif /* GeometryElement_h */
