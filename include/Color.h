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

#include "MaterialPropertyContents.h"


namespace ae {
	
	class Color : public MaterialPropertyContents {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<Color> 				Black();
		static std::shared_ptr<Color> 				DarkGray();
		static std::shared_ptr<Color> 				Gray();
		static std::shared_ptr<Color> 				LightGray();
		static std::shared_ptr<Color> 				White();
		static std::shared_ptr<Color> 				Maroon();
		static std::shared_ptr<Color> 				Red();
		static std::shared_ptr<Color> 				Orange();
		static std::shared_ptr<Color> 				Yellow();
		static std::shared_ptr<Color> 				Olive();
		static std::shared_ptr<Color> 				Lime();
		static std::shared_ptr<Color> 				Green();
		static std::shared_ptr<Color>				Cyan();
		static std::shared_ptr<Color> 				Blue();
		static std::shared_ptr<Color> 				Navy();
		static std::shared_ptr<Color> 				Teal();
		static std::shared_ptr<Color> 				Magenta();
		static std::shared_ptr<Color>				Purple();
		static std::shared_ptr<Color> 				Brown();
		
		static std::vector<std::shared_ptr<Color>> 	Rainbow();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Color();
		Color(float r, float g, float b);
		Color(float r, float g, float b, float a);
		Color(int r, int g, int b);
		Color(int r, int g, int b, int a);
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
