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
#include <vector>

#include "a3d/Id.h"
#include "a3d/util/bitmask.h"

namespace a3d {

	class Color;
	class Texture;

	class Material {

	public:
		/// Public Types ///

		using Property = std::variant<
				std::monostate,
				std::shared_ptr<Texture>,
				std::shared_ptr<Color>>;

		enum class PropertyType : uint8_t {
			Ambient = 	0,
			Diffuse = 	1,
			Specular =	2,
			Emission =	3
		};

		enum class FillMode {
			Fill,
			Lines,
			Points
		};

		enum class AlphaMode : uint8_t {
			Opaque, // no discard, no blending
			Mask, // uses discard/alpha threshold
			Blend }; // real transparency

		enum class BlendFunction : uint8_t {
			Disabled,
			Alpha,
			Additive,
			PremultipliedAlpha
		};

		using PropertyList = std::vector<std::pair<const Property*, PropertyType>>;

		/// Public Static Member Functions ///

		static std::shared_ptr<Material> 	DefaultMaterial();
		static std::shared_ptr<Material> 	EmissionMaterial(Property property);

		/// Public Lifecycle Functions ///

		Material();
		Material(const Property& ambient,
				 const Property& diffuse,
				 const Property& specular);
		Material(const Property& ambient,
				 const Property& diffuse,
				 const Property& specular,
				 const Property& emission);
		~Material();

		/// Public Member Functions ///

		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);

		const Property& 					ambient() const;
		void								ambient(const Property& ambient);

		const Property& 					diffuse() const;
		void 								diffuse(const Property& diffuse);

		const Property&						specular() const;
		void 								specular(const Property& specular);

		const Property&						emission() const;
		void 								emission(const Property& emission);

		PropertyList						properties() const;
		
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

		AlphaMode							alphaMode() const;
		void 								alphaMode(AlphaMode mode);

		// TODO: UNIMPLEMENTED
		float 								alphaCutoff() const;
		void 								alphaCutoff(float v);

		BlendFunction						blendFunction() const;
		void 								blendFunction(BlendFunction function);

		/// Internal Types ///

		enum class DirtyMask : uint32_t {
			None =					0,
			All = 					UINT_MAX
		};

		/// Internal Static Member Functions ///

		static std::shared_ptr<Material> 	MissingTextureMaterial();
		static Property 					MissingTextureProperty();
		
		/// Internal Member Functions ///

		MaterialId 							id() const noexcept;

		DirtyMask 							dirtyMask() const;
		void 								dirtyMask(DirtyMask mask);

	private:
		/// Private Member Variables ///

		MaterialId 							_id;
		std::optional<std::string>			_name;
		Property							_ambient;
		Property							_diffuse;
		Property							_specular;
		Property							_emission;
		float 								_specularExponent;
		bool 								_locksAmbientWithDiffuse;
		bool 								_doubleSided;
		FillMode 							_fillMode;
		float 								_uvScale;
		AlphaMode							_alphaMode;
		float 								_alphaCutoff;
		BlendFunction						_blendFunction;
		DirtyMask							_dirtyMask;
	};

	namespace util::bitmask {
		template <> struct enable_ops<Material::DirtyMask> : std::true_type {};
	}
}

#endif /* AVARA3D_MATERIAL_H */
