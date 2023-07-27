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

#include <glm/glm.hpp>

#include "Types.h"


namespace ae {


	class Material;
	class Program;
	class Renderer;


	class GeometryElement : public std::enable_shared_from_this<GeometryElement> {
		
	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		GeometryElement(std::vector<Vertex>& verticies,
						std::vector<Face>& faces);
		~GeometryElement();

/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void draw(Renderer& renderer,
				  Material& material,
				  const glm::mat4& modelMat,
				  const glm::mat4& viewMat,
				  const glm::mat4& projectionMat,
				  const DEBUG_OPTIONS& debugOptions,
				  RenderStats& stats);

		void burnTransform(const glm::mat4& transform, bool normals);

		const std::vector<Vertex>& vertices() const;
		const std::vector<Face>& faces() const;

		GEOMETRY_ELEMENT_DIRTY_BITS dirtyBits() const;
		void dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS bits);
		
	protected:
		
/*********************************************************************************************
	Protected
 *********************************************************************************************/
		
		std::vector<Vertex>						_vertices;
		std::vector<Face>						_faces;

		GEOMETRY_ELEMENT_DIRTY_BITS				_dirtyBits;
	};
}


#endif /* GeometryElement_h */
