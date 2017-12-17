//
//  SkyboxGeometry.h
//	avara-engine
//
//  Created by Morgan Davis on 12/12/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef SkyboxGeometry_h
#define SkyboxGeometry_h


#include "Geometry.h"

#include <memory>


namespace ae {
	
	
	class Material;
	
	
	class SkyboxGeometry: public Geometry {
		
	public:
		
		/***************************************************************************************
		   	MARK:   Lifecycle
		 **************************************************************************************/
		
		SkyboxGeometry(const std::shared_ptr<Material> material);
		
		/***************************************************************************************
		   	MARK:   Internal
		 **************************************************************************************/
		
		unsigned draw(const glm::mat4& viewMat, const glm::mat4& projectionMat);
		
		void material(std::shared_ptr<Material> material);

	private:
		
		/***************************************************************************************
			MARK:   Private
		 **************************************************************************************/

	};
}


#endif /* SkyboxGeometry_h */
