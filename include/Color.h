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


#include "Aliases.h"
#include "MaterialPropertyContents.h"


namespace ae {
	
	class Color : public MaterialPropertyContents {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static ColorSPtr 				Black();
		static ColorSPtr 				DarkGray();
		static ColorSPtr 				Gray();
		static ColorSPtr 				LightGray();
		static ColorSPtr 				White();
		static ColorSPtr 				Maroon();
		static ColorSPtr 				Red();
		static ColorSPtr 				Orange();
		static ColorSPtr 				Yellow();
		static ColorSPtr 				Olive();
		static ColorSPtr 				Lime();
		static ColorSPtr 				Green();
		static ColorSPtr				Cyan();
		static ColorSPtr 				Blue();
		static ColorSPtr 				Navy();
		static ColorSPtr 				Teal();
		static ColorSPtr 				Magenta();
		static ColorSPtr				Purple();
		static ColorSPtr 				Brown();
		
		static std::vector<ColorSPtr> 	Rainbow();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Color();
		Color(float r, float g, float b);
		Color(float r, float g, float b, float a);
		Color(float black);
		//Color(const Color& color); // copy
		
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
