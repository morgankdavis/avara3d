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
	
	
	class SkyboxMaterial;
	
	
	class SkyboxGeometry: public Geometry {
		
	public:
		
		/***************************************************************************************
		   	Lifecycle
		 ***************************************************************************************/
		
		SkyboxGeometry(const std::shared_ptr<SkyboxMaterial> material);
		
		/***************************************************************************************
		   	Internal
		 ***************************************************************************************/
		
		void draw(const glm::mat4& viewMat, const glm::mat4& projectionMat, DrawStats& stats);
		
		void material(std::shared_ptr<SkyboxMaterial> material);
		//void skyboxMaterial(std::shared_ptr<SkyboxMaterial> material);

	private:
		
		/***************************************************************************************
			Private
		 ***************************************************************************************/

		std::vector<std::shared_ptr<SkyboxMaterial>>		m_materials;
	};
}


#endif /* SkyboxGeometry_h */
