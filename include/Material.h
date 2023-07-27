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

#include "Types.h"


namespace ae {


	class MaterialProperty;


	class Material {
		
	public:
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/
		
		static std::shared_ptr<Material> DefaultMaterial();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Material();
		Material(std::shared_ptr<MaterialProperty> ambient,
				 std::shared_ptr<MaterialProperty> diffuse,
				 std::shared_ptr<MaterialProperty> specular);
		Material(std::shared_ptr<MaterialProperty> ambient,
				 std::shared_ptr<MaterialProperty> diffuse,
				 std::shared_ptr<MaterialProperty> specular,
				 std::shared_ptr<MaterialProperty> emissive);
		~Material();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> name() const;
		void name(const std::string& name);
		
		std::shared_ptr<MaterialProperty> ambient() const;
		void ambient(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> diffuse() const;
		void diffuse(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> specular() const;
		void specular(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> emissive() const;
		void emissive(const std::shared_ptr<MaterialProperty> property);
		
		float specularExponent() const;
		void specularExponent(float exponent);
		
		bool locksAmbientWithDiffuse() const;
		void locksAmbientWithDiffuse(bool flag);
		
		bool doubleSided() const;
		void doubleSided(bool flag);
		
		FILL_MODE fillMode() const;
		void fillMode(FILL_MODE mode);
		
		float uvScale() const;
		void uvScale(float scale);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		MATERIAL_DIRTY_BITS dirtyBits() const;
		void dirtyBits(MATERIAL_DIRTY_BITS bits);

	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/
		
		std::optional<std::string>		_name;
		
		std::shared_ptr<MaterialProperty>	_ambient;
		std::shared_ptr<MaterialProperty> 	_diffuse;
		std::shared_ptr<MaterialProperty> 	_specular;
		std::shared_ptr<MaterialProperty> 	_emissive;

		float 								_specularExponent;
		bool 								_locksAmbientWithDiffuse;
		bool 								_doubleSided;
		FILL_MODE 							_fillMode;
		float 								_uvScale;

		MATERIAL_DIRTY_BITS					_dirtyBits;
	};
}


#endif /* Material_h */
