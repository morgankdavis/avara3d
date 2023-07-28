//
//  Material.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Material_h
#define Material_h


#include <memory>
#include <optional>
#include <string>

//#include <boost/optional.hpp>

#include "Aliases.h"
#include "Types.h"


namespace ae {


	class MaterialProperty;


	class Material {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static MaterialSPtr DefaultMaterial();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Material();
		Material(MaterialPropertySPtr ambient,
				 MaterialPropertySPtr diffuse,
				 MaterialPropertySPtr specular);
		Material(MaterialPropertySPtr ambient,
				 MaterialPropertySPtr diffuse,
				 MaterialPropertySPtr specular,
				 MaterialPropertySPtr emissive);
		~Material();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> 			name() const;
		void 								name(const std::string& name);
		
		MaterialPropertySPtr 				ambient() const;
		void							 	ambient(const MaterialPropertySPtr property);
		
		MaterialPropertySPtr 				diffuse() const;
		void 								diffuse(const MaterialPropertySPtr property);
		
		MaterialPropertySPtr 				specular() const;
		void 								specular(const MaterialPropertySPtr property);
		
		MaterialPropertySPtr 				emissive() const;
		void 								emissive(const MaterialPropertySPtr property);
		
		float 								specularExponent() const;
		void 								specularExponent(float exponent);
		
		bool 								locksAmbientWithDiffuse() const;
		void 								locksAmbientWithDiffuse(bool flag);
		
		bool 								doubleSided() const;
		void 								doubleSided(bool flag);
		
		FILL_MODE 							fillMode() const;
		void 								fillMode(FILL_MODE mode);
		
		float 								uvScale() const;
		void 								uvScale(float scale);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		MATERIAL_DIRTY_BITS 				dirtyBits() const;
		void 								dirtyBits(MATERIAL_DIRTY_BITS bits);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>			_name;
		
		MaterialPropertySPtr				_ambient;
		MaterialPropertySPtr 				_diffuse;
		MaterialPropertySPtr 				_specular;
		MaterialPropertySPtr 				_emissive;

		float 								_specularExponent;
		bool 								_locksAmbientWithDiffuse;
		bool 								_doubleSided;
		FILL_MODE 							_fillMode;
		float 								_uvScale;

		MATERIAL_DIRTY_BITS					_dirtyBits;
	};
}


#endif /* Material_h */
