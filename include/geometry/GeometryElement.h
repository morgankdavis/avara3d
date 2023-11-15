//
//  GeometryElement.h
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef GeometryElement_h
#define GeometryElement_h


#include <memory>
#include <vector>

#include "glm/glm.hpp"

#include "Types.h"


namespace ae {


	class Material;
	class Node;
	class Program;
	class Renderer;


	class GeometryElement : public std::enable_shared_from_this<GeometryElement> {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		GeometryElement(std::vector<Vertex>& verticies,
						std::vector<Face>& faces);
		~GeometryElement();

/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void 								draw(Renderer& renderer,
												 Material& material,
												 const glm::mat4& modelMat,
												 const glm::mat4& viewMat,
												 const glm::mat4& projectionMat,
												 const DEBUG_OPTIONS& debugOptions,
												 FrameStats& stats);

		void 								burnTransform(const glm::mat4& transform,
														  bool normals);

		const std::vector<Vertex>& 			vertices() const;
		const std::vector<Face>&			faces() const;

		AABB								aabb(const std::shared_ptr<Node> convertToNode = nullptr) const;
		glm::vec3 							extent(const std::shared_ptr<Node> convertToNode = nullptr) const;

		GEOMETRY_ELEMENT_DIRTY_MASK 		dirtyMask() const;
		void 								dirtyMask(GEOMETRY_ELEMENT_DIRTY_MASK mask);
		
/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		std::vector<Vertex>					_vertices;
		std::vector<Face>					_faces;

		GEOMETRY_ELEMENT_DIRTY_MASK			_dirtyMask;
	};
}


#endif /* GeometryElement_h */
