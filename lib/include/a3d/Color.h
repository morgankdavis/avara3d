//
//  Color.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_COLOR_H
#define AVARA3D_COLOR_H

#include <memory>
#include <string>
#include <vector>

#include "a3d/Math.h"

namespace a3d {
	
	class Color {

	public:
		/// Public Static Member Functions ///

		static std::shared_ptr<Color> 		Black();
		static std::shared_ptr<Color> 		DarkGray();
		static std::shared_ptr<Color> 		Gray();
		static std::shared_ptr<Color> 		LightGray();
		static std::shared_ptr<Color> 		White();
		static std::shared_ptr<Color> 		Maroon();
		static std::shared_ptr<Color> 		Red();
		static std::shared_ptr<Color> 		Orange();
		static std::shared_ptr<Color> 		Yellow();
		static std::shared_ptr<Color> 		Olive();
		static std::shared_ptr<Color> 		Lime();
		static std::shared_ptr<Color> 		Green();
		static std::shared_ptr<Color>		Cyan();
		static std::shared_ptr<Color> 		Blue();
		static std::shared_ptr<Color> 		Navy();
		static std::shared_ptr<Color> 		Teal();
		static std::shared_ptr<Color> 		Magenta();
		static std::shared_ptr<Color>		Purple();
		static std::shared_ptr<Color> 		Brown();

		static std::shared_ptr<Color> 		Random();

		/// Public Lifecycle Functions ///

		Color();
		explicit Color(const math::vec3& rgb);
		explicit Color(const math::vec4& rgba);
		explicit Color(const math::u8vec3& irgb);
		explicit Color(const math::u8vec4& irgba);
		explicit Color(float white);
		explicit Color(uint32_t color);
		explicit Color(const std::string& hexString);

		// TODO: operator*, operator[]

		/// Public Member Functions ///

		float r() const;
		float g() const;
		float b() const;
		float a() const;

		uint8_t u8r() const;
		uint8_t u8g() const;
		uint8_t u8b() const;
		uint8_t u8a() const;

		math::vec3 rgb() const;
		math::vec4 rgba() const;

		math::u8vec3 u8rgb() const;
		math::u8vec4 u8rgba() const;

	private:
		/// Private Member Variables ///

		math::vec4 _rgba;
	};
}

#endif /* AVARA3D_COLOR_H */
