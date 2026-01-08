//
//  SpotLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_SPOTLIGHT_H
#define AVARA3D_VISUAL_LIGHT_SPOTLIGHT_H

#include <memory>
#include <optional>
#include <string>

#include "a3d/visual/light/AttenuatedLight.h"

namespace a3d {

	class Color;

	class SpotLight : public AttenuatedLight {

	public:
		/// Public Types ///

		enum class FeatheringMode : uint8_t {
			Linear =	0,
			Sharp = 	1, // y = x(2-x)
			Soft = 		2 // y = x^2
		};

		/// Public Lifecycle Functions ///

		SpotLight();
		explicit SpotLight(const std::string& name);
		explicit SpotLight(const std::shared_ptr<Color>& color);
		SpotLight(const std::string& name, const std::shared_ptr<Color>& color);

	public:
		/// Public Member Functions ///

		float					innerAngle() const;
		void					innerAngle(float angle);

		float					outerAngle() const;
		void					outerAngle(float angle);

		FeatheringMode			featheringMode() const;
		void					featheringMode(FeatheringMode mode);

		/// Internal Member Functions ///

		float					innerAngleCos() const;
		float					outerAngleCos() const;

		/// Private Lifecycle Functions ///

//	private:
//
//		SpotLight();

	private:
		/// Private Member Variables ///

		float					_innerAngleCos;
		float					_outerAngleCos;
		FeatheringMode			_featherMode;
	};
}

#endif /* AVARA3D_VISUAL_LIGHT_SPOTLIGHT_H */
