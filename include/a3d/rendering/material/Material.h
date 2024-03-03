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
#include <vector>

#include "a3d/Types.h"


namespace a3d {


	class Color;
	class Texture;


	class Material {

	public:

/*********************************************************************************************
	Public Types
 *********************************************************************************************/

		using Property = std::variant<
				std::monostate,
				std::shared_ptr<Texture>,
				std::shared_ptr<Color>>;

		enum class PropertyType {
			Ambient,
			Diffuse,
			Specular,
			Emission
		};

		using Properties = std::vector<std::pair<Property*, PropertyType>>;
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

		static std::shared_ptr<Material> DefaultMaterial();
		static std::shared_ptr<Material> MissingTextureMaterial(); // TODO: make internal?
		static std::shared_ptr<Material> EmissionMaterial(Property property);
		static Property MissingTextureProperty(); // TODO: make internal?

//		static std::unique_ptr<Material> DiffuseMaterial(Material::Property property);
//		static std::unique_ptr<Material> EmissionMaterial(Material::Property property);
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Material();
		Material(Property ambient,
				 Property diffuse,
				 Property specular);
		Material(Property ambient,
				 Property diffuse,
				 Property specular,
				 Property emission);
		~Material();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> 				name() const;
		void 									name(const std::string& name);

		Property 								ambient() const;
		void									ambient(Property ambient);

		Property 								diffuse() const;
		void 									diffuse(Property diffuse);

		Property								specular() const;
		void 									specular(Property specular);

		Property								emission() const;
		void 									emission(Property emission);

		const Properties 						properties() const;
		
		float 									specularExponent() const;
		void 									specularExponent(float exponent);
		
		bool 									locksAmbientWithDiffuse() const;
		void 									locksAmbientWithDiffuse(bool flag);
		
		bool 									doubleSided() const;
		void 									doubleSided(bool flag);

//		float 									maxAnisotropy() const;
//		void 									maxAnisotropy(float max);
		
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

		Property								_ambient;
		Property								_diffuse;
		Property								_specular;
		Property								_emission;

		float 									_specularExponent;
		bool 									_locksAmbientWithDiffuse;
		bool 									_doubleSided;
//		float									_maxAnisotropy;
		FillMode 								_fillMode;
		float 									_uvScale;
		BlendFunction							_blendFunction;

		MaterialDirtyMask						_dirtyMask;
	};
}


#endif /* Material_h */
