//
//  MeshElement.h
//	avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef MeshElement_h
#define MeshElement_h


#include <memory>
#include <vector>

#include "glm/glm.hpp"

#include "a3d/Types.h"


namespace a3d {


	class Line;
	class Material;
	class Node;
	class Program;
	class Renderer;


	class MeshElement {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		MeshElement(const std::vector<Vertex>& verticies,
					const std::vector<Face>& faces);
		virtual ~MeshElement();

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

		AABB							aabb(const Node* convertTo = nullptr) const;
		glm::vec3 						extent(const Node* convertTo = nullptr) const;

		const std::vector<Line>&				aabbLines();

		MeshElementDirtyMask 			dirtyMask() const;
		void 							dirtyMask(MeshElementDirtyMask mask);

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		MeshElement();

		std::vector<Vertex>				_vertices;
		std::vector<Face>				_faces;
		std::vector<Line>				_aabbLines;
		MeshElementDirtyMask			_dirtyMask;
	};
}


#endif /* MeshElement_h */
