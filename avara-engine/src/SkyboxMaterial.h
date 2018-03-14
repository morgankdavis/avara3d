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
		   	MARK:   Lifecycle
		 **************************************************************************************/
		
		SkyboxMaterial(std::shared_ptr<MaterialProperty> cubeProperty);
		
		/***************************************************************************************
		   	MARK:   Internal
		 **************************************************************************************/
		
		std::shared_ptr<MaterialProperty> cubeProperty() const;
		void cubeProperty(std::shared_ptr<MaterialProperty> property);
		
		/***************************************************************************************
		   	MARK:   Material
		 **************************************************************************************/
		
		void prepareToRender(DebugOption debugOptions) const;

	private:
		
		/***************************************************************************************
			MARK:   Private
		 **************************************************************************************/

		std::shared_ptr<MaterialProperty>	m_cubeProperty;
	};
}


#endif /* SkyboxMaterial_h */
