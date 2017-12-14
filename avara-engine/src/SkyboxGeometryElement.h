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
	
	
	class SkyboxGeometryElement: public GeometryElement {
		
	public:
		
		/***************************************************************************************
			MARK:   Lifecycle
		 **************************************************************************************/
		
		SkyboxGeometryElement(std::vector<Vertex>& verticies,
							  std::vector<Face>& faces);
		
		/***************************************************************************************
		 	MARK:   Internal
		 **************************************************************************************/
		
		unsigned draw(const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const Material& material);
		
	private:
		
		/***************************************************************************************
		    	 MARK:   Private
		 **************************************************************************************/
		
	};
}


#endif /* SkyboxGeometryElement_h */
