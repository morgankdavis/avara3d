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


namespace a3d {
	
	class Color {

/*********************************************************************************************
	Public Static Members
 *********************************************************************************************/

	public:

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

/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

		Color();
		Color(float r, float g, float b);
		Color(float r, float g, float b, float a);
		Color(int r, int g, int b);
		Color(int r, int g, int b, int a);
		explicit Color(float white);
		explicit Color(uint32_t color);
		explicit Color(const std::string& hexString);

		// TODO: operator*, operator[], rgb(), rgba()

/*********************************************************************************************
	Public IVars
 *********************************************************************************************/

		float r;
		float g;
		float b;
		float a;
	};
}


#endif /* AVARA3D_COLOR_H */
