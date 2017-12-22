//
//  Color.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Color_h
#define Color_h


#include <memory>
#include <vector>


namespace ae {
	
	class Color {
		
	public:
		
		/***************************************************************************************
		     MARK:   Static
		 **************************************************************************************/
		
		static Color Black();
		static Color DarkGray();
		static Color Gray();
		static Color LightGray();
		static Color White();
		static Color Maroon();
		static Color Red();
		static Color Orange();
		static Color Yellow();
		static Color Olive();
		static Color Lime();
		static Color Green();
		static Color Cyan();
		static Color Blue();
		static Color Navy();
		static Color Teal();
		static Color Magenta();
		static Color Purple();
		static Color Brown();
		
		static std::vector<Color> Rainbow();
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Color();
		Color(const float r, const float g, const float b, const float a);
		Color(const float black);
		Color(const Color& color); // copy
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		float r;
		float g;
		float b;
		float a;
	};
}


#endif /* Color_h */
