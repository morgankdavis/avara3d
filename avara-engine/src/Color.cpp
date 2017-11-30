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

static std::shared_ptr<Color> MakeColorBlack() {
	return make_shared<Color>(0.0f, 0.0f, 0.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorWhite() {
	return make_shared<Color>(1.0f, 1.0f, 1.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorGray() {
	return make_shared<Color>(0.5f, 0.5, 0.5f, 1.0f);
}

static std::shared_ptr<Color> MakeColorLightGray() {
	return make_shared<Color>(0.75f, 0.75f, 0.75f, 1.0f);
}

static std::shared_ptr<Color> MakeColorDarkGray() {
	return make_shared<Color>(0.25f, 0.25f, 0.25f, 1.0f);
}

static std::shared_ptr<Color> MakeColorMaroon() {
	return make_shared<Color>(128.0f/255.0f, 0.0f, 0.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorRed() {
	return make_shared<Color>(1.0f, 0.0f, 0.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorOrange() {
	return make_shared<Color>(1.0f, 165.0f/255.0f, 0.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorYellow() {
	return make_shared<Color>(1.0f, 1.0f, 0.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorOlive() {
	return make_shared<Color>(128.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorLine() {
	return make_shared<Color>(0.0f, 1.0f, 0.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorGreen() {
	return make_shared<Color>(0.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorCyan() {
	return make_shared<Color>(0.0f, 1.0f, 1.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorBlue() {
	return make_shared<Color>(0.0f, 0.0f, 1.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorNavy() {
	return make_shared<Color>(0.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorTeal() {
	return make_shared<Color>(0.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorMagenta() {
	return make_shared<Color>(1.0f, 0.0f, 1.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorPurple() {
	return make_shared<Color>(128.0f/255.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

static std::shared_ptr<Color> MakeColorBrown() {
	return make_shared<Color>(153.0f/255.0f, 102.0f/255.0f, 51.0f/255.0f, 1.0f);
}

