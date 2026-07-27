//
//  PointLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_POINTLIGHT_H
#define AVARA3D_VISUAL_LIGHT_POINTLIGHT_H

#include <memory>
#include <string>

#include "a3d/visual/light/Attenuation.h"
#include "a3d/visual/light/Light.h"

namespace a3d {

	class Color;

	class PointLight : public Light {

	public:
		/// Public Lifecycle Functions ///

		PointLight();
		explicit PointLight(const std::string& name);
		explicit PointLight(const std::shared_ptr<Color>& color);
		PointLight(const std::string& name, const std::shared_ptr<Color>& color);

		PointLight(const PointLight&) = default;
		PointLight& operator=(const PointLight&) = default;

		PointLight(PointLight&&) noexcept = default;
		PointLight& operator=(PointLight&&) noexcept = default;

		/// Public Member Functions ///

		const Attenuation&		attenuation() const;
		void					attenuation(const Attenuation& attenuation);

	private:
		/// Private Member Variables ///

		Attenuation 			_attenuation;
	};
}

#endif //AVARA3D_VISUAL_LIGHT_POINTLIGHT_H */
