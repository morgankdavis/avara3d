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

#include "ae/Types.h"


namespace ae {


	class MaterialProperty;
	class MaterialPropertyContents;


	class Material {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<Material> DefaultMaterial();
		static std::shared_ptr<Material> MissingTextureMaterial(); // TODO: make private
		static std::shared_ptr<Material> EmissiveMaterial(std::shared_ptr<MaterialPropertyContents> contents);
		
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
				 std::shared_ptr<MaterialProperty> emission);
		~Material();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> 				name() const;
		void 									name(const std::string& name);
		
		std::shared_ptr<MaterialProperty> 		ambient() const;
		void							 		ambient(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> 		diffuse() const;
		void 									diffuse(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty>		specular() const;
		void 									specular(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> 		emission() const;
		void 									emission(const std::shared_ptr<MaterialProperty> property);
		
		float 									specularExponent() const;
		void 									specularExponent(float exponent);
		
		bool 									locksAmbientWithDiffuse() const;
		void 									locksAmbientWithDiffuse(bool flag);
		
		bool 									doubleSided() const;
		void 									doubleSided(bool flag);
		
		FillMode 								fillMode() const;
		void 									fillMode(FillMode mode);
		
		float 									uvScale() const;
		void 									uvScale(float scale);

		BlendFunction							blendFunction() const;
		void 									blendFunction(BlendFunction function);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		MaterialDirtyMask 						dirtyMask() const;
		void 									dirtyMask(MaterialDirtyMask mask);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>				_name;

		std::shared_ptr<MaterialProperty>		_ambient;
		std::shared_ptr<MaterialProperty> 		_diffuse;
		std::shared_ptr<MaterialProperty> 		_specular;
		std::shared_ptr<MaterialProperty> 		_emission;

		float 									_specularExponent;
		bool 									_locksAmbientWithDiffuse;
		bool 									_doubleSided;
		FillMode 								_fillMode;
		float 									_uvScale;
		BlendFunction							_blendFunction;

		MaterialDirtyMask						_dirtyMask;
	};
}


#endif /* Material_h */
