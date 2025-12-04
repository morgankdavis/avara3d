//
//  Material.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MATERIAL_H
#define AVARA3D_MATERIAL_H

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
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static std::shared_ptr<Material> 	DefaultMaterial();
		static std::shared_ptr<Material> 	EmissionMaterial(MaterialProperty property);

/*********************************************************************************************
	Public Lifecycle Functions
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
	Public Member Functions
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
	Internal Static Member Functions
 *********************************************************************************************/

		static std::shared_ptr<Material> 	MissingTextureMaterial();
		static MaterialProperty 			MissingTextureProperty();
		
/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		MaterialDirtyMask 					dirtyMask() const;
		void 								dirtyMask(MaterialDirtyMask mask);

/*********************************************************************************************
	Private Member Variables
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

#endif /* AVARA3D_MATERIAL_H */
