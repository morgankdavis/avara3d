//
//  Color.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Color.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Color::Color():
	r(0.0),
	g(0.0),
	b(0.0),
	a(1.0) {

}

Color::Color(const float r, const float g, const float b, const float a):
	r(r),
	g(g),
	b(b),
	a(a) {
}


/***************************************************************************************
     MARK:   Static
 **************************************************************************************/

//shared_ptr<Color> Black() {
//	return make_shared<Color>(0.0f, 0.0f, 0.0f, 1.0f);
//}
//
//shared_ptr<Color> White() {
//	return make_shared<Color>(1.0f, 1.0f, 1.0f, 1.0f);
//}
//
//shared_ptr<Color> Gray() {
//	return make_shared<Color>(0.5f, 0.5, 0.5f, 1.0f);
//}
//
//shared_ptr<Color> LightGray() {
//	return make_shared<Color>(0.75f, 0.75f, 0.75f, 1.0f);
//}
//
//shared_ptr<Color> DarkGray() {
//	return make_shared<Color>(0.25f, 0.25f, 0.25f, 1.0f);
//}
//
//shared_ptr<Color> Maroon() {
//	return make_shared<Color>(128.0f/255.0f, 0.0f, 0.0f, 1.0f);
//}
//
//shared_ptr<Color> Red() {
//	return make_shared<Color>(1.0f, 0.0f, 0.0f, 1.0f);
//}
//
//shared_ptr<Color> Orange() {
//	return make_shared<Color>(1.0f, 165.0f/255.0f, 0.0f, 1.0f);
//}
//
//shared_ptr<Color> Yellow() {
//	return make_shared<Color>(1.0f, 1.0f, 0.0f, 1.0f);
//}
//
//shared_ptr<Color> Olive() {
//	return make_shared<Color>(128.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f);
//}
//
//shared_ptr<Color> Line() {
//	return make_shared<Color>(0.0f, 1.0f, 0.0f, 1.0f);
//}
//
//shared_ptr<Color> Green() {
//	return make_shared<Color>(0.0f, 128.0f/255.0f, 0.0f, 1.0f);
//}
//
//shared_ptr<Color> Cyan() {
//	return make_shared<Color>(0.0f, 1.0f, 1.0f, 1.0f);
//}
//
//shared_ptr<Color> Blue() {
//	return make_shared<Color>(0.0f, 0.0f, 1.0f, 1.0f);
//}
//
//shared_ptr<Color> Navy() {
//	return make_shared<Color>(0.0f, 0.0f, 128.0f/255.0f, 1.0f);
//}
//
//shared_ptr<Color> Teal() {
//	return make_shared<Color>(0.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f);
//}
//
//shared_ptr<Color> Magenta() {
//	return make_shared<Color>(1.0f, 0.0f, 1.0f, 1.0f);
//}
//
//shared_ptr<Color> Purple() {
//	return make_shared<Color>(128.0f/255.0f, 0.0f, 128.0f/255.0f, 1.0f);
//}
//
//shared_ptr<Color> Brown() {
//	return make_shared<Color>(153.0f/255.0f, 102.0f/255.0f, 51.0f/255.0f, 1.0f);
//}

shared_ptr<Color> Color::Black() {
	return make_shared<Color>(0.0f, 0.0f, 0.0f, 1.0f);
}

shared_ptr<Color> Color::White() {
	return make_shared<Color>(1.0f, 1.0f, 1.0f, 1.0f);
}

shared_ptr<Color> Color::Gray() {
	return make_shared<Color>(0.5f, 0.5, 0.5f, 1.0f);
}

shared_ptr<Color> Color::LightGray() {
	return make_shared<Color>(0.75f, 0.75f, 0.75f, 1.0f);
}

shared_ptr<Color> Color::DarkGray() {
	return make_shared<Color>(0.25f, 0.25f, 0.25f, 1.0f);
}

shared_ptr<Color> Color::Maroon() {
	return make_shared<Color>(128.0f/255.0f, 0.0f, 0.0f, 1.0f);
}

shared_ptr<Color> Color::Red() {
	return make_shared<Color>(1.0f, 0.0f, 0.0f, 1.0f);
}

shared_ptr<Color> Color::Orange() {
	return make_shared<Color>(1.0f, 165.0f/255.0f, 0.0f, 1.0f);
}

shared_ptr<Color> Color::Yellow() {
	return make_shared<Color>(1.0f, 1.0f, 0.0f, 1.0f);
}

shared_ptr<Color> Color::Olive() {
	return make_shared<Color>(128.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

shared_ptr<Color> Color::Line() {
	return make_shared<Color>(0.0f, 1.0f, 0.0f, 1.0f);
}

shared_ptr<Color> Color::Green() {
	return make_shared<Color>(0.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

shared_ptr<Color> Color::Cyan() {
	return make_shared<Color>(0.0f, 1.0f, 1.0f, 1.0f);
}

shared_ptr<Color> Color::Blue() {
	return make_shared<Color>(0.0f, 0.0f, 1.0f, 1.0f);
}

shared_ptr<Color> Color::Navy() {
	return make_shared<Color>(0.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

shared_ptr<Color> Color::Teal() {
	return make_shared<Color>(0.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f);
}

shared_ptr<Color> Color::Magenta() {
	return make_shared<Color>(1.0f, 0.0f, 1.0f, 1.0f);
}

shared_ptr<Color> Color::Purple() {
	return make_shared<Color>(128.0f/255.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

shared_ptr<Color> Color::Brown() {
	return make_shared<Color>(153.0f/255.0f, 102.0f/255.0f, 51.0f/255.0f, 1.0f);
}

