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

		static std::shared_ptr<ae::Color> 				Black();
		static std::shared_ptr<ae::Color> 				DarkGray();
		static std::shared_ptr<ae::Color> 				Gray();
		static std::shared_ptr<ae::Color> 				LightGray();
		static std::shared_ptr<ae::Color> 				White();
		static std::shared_ptr<ae::Color> 				Maroon();
		static std::shared_ptr<ae::Color> 				Red();
		static std::shared_ptr<ae::Color> 				Orange();
		static std::shared_ptr<ae::Color> 				Yellow();
		static std::shared_ptr<ae::Color> 				Olive();
		static std::shared_ptr<ae::Color> 				Lime();
		static std::shared_ptr<ae::Color> 				Green();
		static std::shared_ptr<ae::Color>				Cyan();
		static std::shared_ptr<ae::Color> 				Blue();
		static std::shared_ptr<ae::Color> 				Navy();
		static std::shared_ptr<ae::Color> 				Teal();
		static std::shared_ptr<ae::Color> 				Magenta();
		static std::shared_ptr<ae::Color>				Purple();
		static std::shared_ptr<ae::Color> 				Brown();
		
		static std::vector<std::shared_ptr<ae::Color>> 	Rainbow();
		
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
