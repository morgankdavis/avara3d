//
//  Color.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Color_h
#define Color_h


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
	
//	static Color red();
//	static Color black();
//	static Color blue();
//	static Color brown();
//	static Color clear();
//	static Color cyan();
//	static Color gray();
//	static Color lightGray();
//	static Color darkGray();
//	static Color green();
//	static Color magenta();
//	static Color orange();
//	static Color purple();
//	static Color white();
//	static Color yellow();
}


#endif /* Color_h */
