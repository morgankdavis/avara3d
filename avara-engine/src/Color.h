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


namespace ae {
	
	class Color {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Color();
		Color(const float r, const float g, const float b, const float a);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		float r;
		float g;
		float b;
		float a;
	};
	
	/***************************************************************************************
	     MARK:   Static
	 **************************************************************************************/
	
	static std::shared_ptr<Color> MakeColorBlack();
	static std::shared_ptr<Color> MakeColorWhite();
	static std::shared_ptr<Color> MakeColorGray();
	static std::shared_ptr<Color> MakeColorLightGray();
	static std::shared_ptr<Color> MakeColorDarkGray();
	static std::shared_ptr<Color> MakeColorMaroon();
	static std::shared_ptr<Color> MakeColorRed();
	static std::shared_ptr<Color> MakeColorOrange();
	static std::shared_ptr<Color> MakeColorYellow();
	static std::shared_ptr<Color> MakeColorOlive();
	static std::shared_ptr<Color> MakeColorLine();
	static std::shared_ptr<Color> MakeColorGreen();
	static std::shared_ptr<Color> MakeColorCyan();
	static std::shared_ptr<Color> MakeColorBlue();
	static std::shared_ptr<Color> MakeColorNavy();
	static std::shared_ptr<Color> MakeColorTeal();
	static std::shared_ptr<Color> MakeColorMagenta();
	static std::shared_ptr<Color> MakeColorPurple();
	static std::shared_ptr<Color> MakeColorBrown();
}


#endif /* Color_h */
