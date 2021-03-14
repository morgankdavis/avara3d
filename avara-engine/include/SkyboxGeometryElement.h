//
//  SkyboxGeometryElement.h
//	avara-engine
//
//  Created by Morgan Davis on 12/12/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef SkyboxGeometryElement_h
#define SkyboxGeometryElement_h


#include <memory>
#include <vector>

#include "GeometryElement.h"
#include "Types.h"


namespace ae {
	
	
	class Image;
	class SkyboxMaterial;
	
	
	class SkyboxGeometryElement: public GeometryElement {
		
	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		SkyboxGeometryElement(std::vector<Vertex>& verticies,
							  std::vector<Face>& faces);
		~SkyboxGeometryElement();
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void draw(const glm::mat4& viewMat,
				  const glm::mat4& projectionMat,
				  const SkyboxMaterial& material,
				  RenderStats& stats);
		
		void loadVertexData(const Program& program);
		
	private:
		
/*********************************************************************************************
 	Private
 *********************************************************************************************/
		
	};
}


#endif /* SkyboxGeometryElement_h */
