//
//  Material.h
//	avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Material_h
#define Material_h


#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "a3d/Types.h"


namespace a3d {


	class Color;
	class Texture;


	class Material {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<Material> DefaultMaterial();
		static std::shared_ptr<Material> MissingTextureMaterial(); // TODO: make internal?
		static std::shared_ptr<Material> EmissionMaterial(MaterialProperty property);
		static MaterialProperty MissingTextureProperty(); // TODO: make internal?

//		static std::unique_ptr<Material> DiffuseMaterial(Material::Property property);
//		static std::unique_ptr<Material> EmissionMaterial(Material::Property property);
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Material();
		Material(const MaterialProperty& ambient,
				 const MaterialProperty& diffuse,
				 const MaterialProperty& specular);
		Material(const MaterialProperty& ambient,
				 const MaterialProperty& diffuse,
				 const MaterialProperty& specular,
				 const MaterialProperty& emission);
		~Material();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);

		const MaterialProperty& 			ambient() const;
		void								ambient(const MaterialProperty& ambient);

		const MaterialProperty& 			diffuse() const;
		void 								diffuse(const MaterialProperty& diffuse);

		const MaterialProperty&				specular() const;
		void 								specular(const MaterialProperty& specular);

		const MaterialProperty&				emission() const;
		void 								emission(const MaterialProperty& emission);

		MaterialPropertyList				properties() const;
		
		float 								specularExponent() const;
		void 								specularExponent(float exponent);
		
		bool 								locksAmbientWithDiffuse() const;
		void 								locksAmbientWithDiffuse(bool flag);
		
		bool 								doubleSided() const;
		void 								doubleSided(bool flag);

		FillMode 							fillMode() const;
		void 								fillMode(FillMode mode);
		
		float 								uvScale() const;
		void 								uvScale(float scale);

		BlendFunction						blendFunction() const;
		void 								blendFunction(BlendFunction function);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		MaterialDirtyMask 					dirtyMask() const;
		void 								dirtyMask(MaterialDirtyMask mask);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>			_name;

		MaterialProperty					_ambient;
		MaterialProperty					_diffuse;
		MaterialProperty					_specular;
		MaterialProperty					_emission;

		float 								_specularExponent;
		bool 								_locksAmbientWithDiffuse;
		bool 								_doubleSided;
		FillMode 							_fillMode;
		float 								_uvScale;
		BlendFunction						_blendFunction;

		MaterialDirtyMask					_dirtyMask;
	};
}


#endif /* Material_h */
