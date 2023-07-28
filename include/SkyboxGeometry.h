//
//  SkyboxGeometry.h
//	avara-engine
//
//  Created by Morgan Davis on 12/12/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef SkyboxGeometry_h
#define SkyboxGeometry_h

#include <memory>

#include "Aliases.h"
#include "Geometry.h"


namespace ae {
	
	
	class SkyboxMaterial;
	
	
	class SkyboxGeometry: public Geometry {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		SkyboxGeometry(const SkyboxMaterialSPtr material);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 								draw(const glm::mat4& viewMat,
												 const glm::mat4& projectionMat,
												 RenderStats& stats);

		void 								material(SkyboxMaterialSPtr material);
		//void 								skyboxMaterial(SkyboxMaterialSPtr material);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::vector<SkyboxMaterialSPtr>		_materials;
	};
}


#endif /* SkyboxGeometry_h */
