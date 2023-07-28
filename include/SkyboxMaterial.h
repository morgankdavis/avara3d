//
//  SkyboxMaterial.h
//	avara-engine
//
//  Created by Morgan Davis on 1/8/18.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef SkyboxMaterial_h
#define SkyboxMaterial_h

#include <memory>

#include "Aliases.h"
#include "Material.h"


namespace ae {
	
	
	class Material;
	
	
	class SkyboxMaterial: public Material {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		SkyboxMaterial(MaterialPropertySPtr cubeProperty);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		MaterialPropertySPtr 		cubeProperty() const;
		void 						cubeProperty(MaterialPropertySPtr property);
		
/*********************************************************************************************
	Material
 *********************************************************************************************/
		
		void 						prepareToRender(DEBUG_OPTIONS debugOptions) const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		MaterialPropertySPtr		_cubeProperty;
	};
}


#endif /* SkyboxMaterial_h */
