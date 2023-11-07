//
//  Color.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Color.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

std::shared_ptr<ae::Color> Color::Black() {
	return make_shared<Color>(0.0f, 0.0f, 0.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::DarkGray() {
	return make_shared<Color>(0.25f, 0.25f, 0.25f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Gray() {
	return make_shared<Color>(0.5f, 0.5, 0.5f, 1.0f);
}

std::shared_ptr<ae::Color> Color::LightGray() {
	return make_shared<Color>(0.75f, 0.75f, 0.75f, 1.0f);
}

std::shared_ptr<ae::Color> Color::White() {
	return make_shared<Color>(1.0f, 1.0f, 1.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Maroon() {
	return make_shared<Color>(128.0f/255.0f, 0.0f, 0.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Red() {
	return make_shared<Color>(1.0f, 0.0f, 0.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Orange() {
	return make_shared<Color>(1.0f, 165.0f/255.0f, 0.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Yellow() {
	return make_shared<Color>(1.0f, 1.0f, 0.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Olive() {
	return make_shared<Color>(128.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Lime() {
	return make_shared<Color>(0.0f, 1.0f, 0.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Green() {
	return make_shared<Color>(0.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Cyan() {
	return make_shared<Color>(0.0f, 1.0f, 1.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Blue() {
	return make_shared<Color>(0.0f, 0.0f, 1.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Navy() {
	return make_shared<Color>(0.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Teal() {
	return make_shared<Color>(0.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Magenta() {
	return make_shared<Color>(1.0f, 0.0f, 1.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Purple() {
	return make_shared<Color>(128.0f/255.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

std::shared_ptr<ae::Color> Color::Brown() {
	return make_shared<Color>(153.0f/255.0f, 102.0f/255.0f, 51.0f/255.0f, 1.0f);
}

vector<std::shared_ptr<ae::Color>> Color::Rainbow() {
	return vector<std::shared_ptr<ae::Color>>() = {
		Black(),
		DarkGray(),
		Gray(),
		LightGray(),
		White(),
		Maroon(),
		Red(),
		Orange(),
		Yellow(),
		Olive(),
		Lime(),
		Green(),
		Cyan(),
		Blue(),
		Navy(),
		Teal(),
		Magenta(),
		Purple(),
		Brown() };
}


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Color::Color():
	r(0.0),
	g(0.0),
	b(0.0),
	a(1.0) {
		
}

Color::Color(float r, float g, float b):
	Color(r, g, b, 1.0) {

}

Color::Color(float r, float g, float b, float a):
	r(r),
	g(g),
	b(b),
	a(a) {
}

Color::Color(int r, int g, int b):
		Color((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, 1.0) {

}

Color::Color(int r, int g, int b, int a):
		Color((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f) {

}

Color::Color(float black):
	r(black),
	g(black),
	b(black),
	a(1.0) {
	
}

//Color::Color(const Color& color) {
//	r = color.r;
//	g = color.g;
//	b = color.b;
//	a = color.a;
//}
