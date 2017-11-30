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
		     MARK:   Static
		 **************************************************************************************/
		
		static std::shared_ptr<Color> Black();
		static std::shared_ptr<Color> White();
		static std::shared_ptr<Color> Gray();
		static std::shared_ptr<Color> LightGray();
		static std::shared_ptr<Color> DarkGray();
		static std::shared_ptr<Color> Maroon();
		static std::shared_ptr<Color> Red();
		static std::shared_ptr<Color> Orange();
		static std::shared_ptr<Color> Yellow();
		static std::shared_ptr<Color> Olive();
		static std::shared_ptr<Color> Line();
		static std::shared_ptr<Color> Green();
		static std::shared_ptr<Color> Cyan();
		static std::shared_ptr<Color> Blue();
		static std::shared_ptr<Color> Navy();
		static std::shared_ptr<Color> Teal();
		static std::shared_ptr<Color> Magenta();
		static std::shared_ptr<Color> Purple();
		static std::shared_ptr<Color> Brown();
		
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
	
	
//	static std::shared_ptr<Color> Black() {
//		return std::make_shared<Color>(0.0f, 0.0f, 0.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> White() {
//		return std::make_shared<Color>(1.0f, 1.0f, 1.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Gray() {
//		return std::make_shared<Color>(0.5f, 0.5, 0.5f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> LightGray() {
//		return std::make_shared<Color>(0.75f, 0.75f, 0.75f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> DarkGray() {
//		return std::make_shared<Color>(0.25f, 0.25f, 0.25f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Maroon() {
//		return std::make_shared<Color>(128.0f/255.0f, 0.0f, 0.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Red() {
//		return std::make_shared<Color>(1.0f, 0.0f, 0.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Orange() {
//		return std::make_shared<Color>(1.0f, 165.0f/255.0f, 0.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Yellow() {
//		return std::make_shared<Color>(1.0f, 1.0f, 0.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Olive() {
//		return std::make_shared<Color>(128.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Line() {
//		return std::make_shared<Color>(0.0f, 1.0f, 0.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Green() {
//		return std::make_shared<Color>(0.0f, 128.0f/255.0f, 0.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Cyan() {
//		return std::make_shared<Color>(0.0f, 1.0f, 1.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Blue() {
//		return std::make_shared<Color>(0.0f, 0.0f, 1.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Navy() {
//		return std::make_shared<Color>(0.0f, 0.0f, 128.0f/255.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Teal() {
//		return std::make_shared<Color>(0.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Magenta() {
//		return std::make_shared<Color>(1.0f, 0.0f, 1.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Purple() {
//		return std::make_shared<Color>(128.0f/255.0f, 0.0f, 128.0f/255.0f, 1.0f);
//	}
//
//	static std::shared_ptr<Color> Brown() {
//		return std::make_shared<Color>(153.0f/255.0f, 102.0f/255.0f, 51.0f/255.0f, 1.0f);
//	}

}


#endif /* Color_h */
