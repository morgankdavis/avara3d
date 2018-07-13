//
//  SkyboxMaterial.h
//	avara-engine
//
//  Created by Morgan Davis on 1/8/18.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef SkyboxMaterial_h
#define SkyboxMaterial_h


#include "Material.h"

#include <memory>


namespace ae {
	
	
	class Material;
	
	
	class SkyboxMaterial: public Material {
		
	public:
		
		/***************************************************************************************
		   	Lifecycle
		 ***************************************************************************************/
		
		SkyboxMaterial(std::shared_ptr<MaterialProperty> cubeProperty);
		
		/***************************************************************************************
		   	Internal
		 ***************************************************************************************/
		
		std::shared_ptr<MaterialProperty> cubeProperty() const;
		void cubeProperty(std::shared_ptr<MaterialProperty> property);
		
		/***************************************************************************************
		   	Material
		 ***************************************************************************************/
		
		void prepareToRender(DEBUG_OPTIONS debugOptions) const;

	private:
		
		/***************************************************************************************
			Private
		 ***************************************************************************************/

		std::shared_ptr<MaterialProperty>	m_cubeProperty;
	};
}


#endif /* SkyboxMaterial_h */
