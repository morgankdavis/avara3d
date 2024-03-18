//
//  Color.h
//	avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Color_h
#define Color_h


#include <memory>
#include <string>
#include <vector>


namespace a3d {
	
	class Color {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::unique_ptr<Color> 		Black();
		static std::unique_ptr<Color> 		DarkGray();
		static std::unique_ptr<Color> 		Gray();
		static std::unique_ptr<Color> 		LightGray();
		static std::unique_ptr<Color> 		White();
		static std::unique_ptr<Color> 		Maroon();
		static std::unique_ptr<Color> 		Red();
		static std::unique_ptr<Color> 		Orange();
		static std::unique_ptr<Color> 		Yellow();
		static std::unique_ptr<Color> 		Olive();
		static std::unique_ptr<Color> 		Lime();
		static std::unique_ptr<Color> 		Green();
		static std::unique_ptr<Color>		Cyan();
		static std::unique_ptr<Color> 		Blue();
		static std::unique_ptr<Color> 		Navy();
		static std::unique_ptr<Color> 		Teal();
		static std::unique_ptr<Color> 		Magenta();
		static std::unique_ptr<Color>		Purple();
		static std::unique_ptr<Color> 		Brown();

		static std::unique_ptr<Color> 		Random();

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Color();
		Color(float r, float g, float b);
		Color(float r, float g, float b, float a);
		Color(int r, int g, int b);
		Color(int r, int g, int b, int a);
		Color(float white);
		Color(uint32_t color);
		Color(const std::string& hexString);
		Color(Color* other);
//		Color(const Color& color); // copy
//		Color& operator=(const Color& other); // copy assignment
//		Color(Color&& other) noexcept; // move
//		Color& operator=(Color&& other) noexcept; // move assignment
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float r;
		float g;
		float b;
		float a;
	};
}


#endif /* Color_h */
