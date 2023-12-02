//
//  Color.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "utilities/Color.h"

#include "utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

std::shared_ptr<Color> Color::Black() {
	return make_shared<Color>(0.0f, 0.0f, 0.0f, 1.0f);
}

std::shared_ptr<Color> Color::DarkGray() {
	return make_shared<Color>(0.25f, 0.25f, 0.25f, 1.0f);
}

std::shared_ptr<Color> Color::Gray() {
	return make_shared<Color>(0.5f, 0.5, 0.5f, 1.0f);
}

std::shared_ptr<Color> Color::LightGray() {
	return make_shared<Color>(0.75f, 0.75f, 0.75f, 1.0f);
}

std::shared_ptr<Color> Color::White() {
	return make_shared<Color>(1.0f, 1.0f, 1.0f, 1.0f);
}

std::shared_ptr<Color> Color::Maroon() {
	return make_shared<Color>(128.0f/255.0f, 0.0f, 0.0f, 1.0f);
}

std::shared_ptr<Color> Color::Red() {
	return make_shared<Color>(1.0f, 0.0f, 0.0f, 1.0f);
}

std::shared_ptr<Color> Color::Orange() {
	return make_shared<Color>(1.0f, 165.0f/255.0f, 0.0f, 1.0f);
}

std::shared_ptr<Color> Color::Yellow() {
	return make_shared<Color>(1.0f, 1.0f, 0.0f, 1.0f);
}

std::shared_ptr<Color> Color::Olive() {
	return make_shared<Color>(128.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

std::shared_ptr<Color> Color::Lime() {
	return make_shared<Color>(0.0f, 1.0f, 0.0f, 1.0f);
}

std::shared_ptr<Color> Color::Green() {
	return make_shared<Color>(0.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

std::shared_ptr<Color> Color::Cyan() {
	return make_shared<Color>(0.0f, 1.0f, 1.0f, 1.0f);
}

std::shared_ptr<Color> Color::Blue() {
	return make_shared<Color>(0.0f, 0.0f, 1.0f, 1.0f);
}

std::shared_ptr<Color> Color::Navy() {
	return make_shared<Color>(0.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

std::shared_ptr<Color> Color::Teal() {
	return make_shared<Color>(0.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f);
}

std::shared_ptr<Color> Color::Magenta() {
	return make_shared<Color>(1.0f, 0.0f, 1.0f, 1.0f);
}

std::shared_ptr<Color> Color::Purple() {
	return make_shared<Color>(128.0f/255.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

std::shared_ptr<Color> Color::Brown() {
	return make_shared<Color>(153.0f/255.0f, 102.0f/255.0f, 51.0f/255.0f, 1.0f);
}

vector<std::shared_ptr<Color>> Color::Rainbow() {
	return vector<shared_ptr<Color>>() = {
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

//	auto v = vector<unique_ptr<Color>>();
//	v.push_back(Black());
//	v.push_back(DarkGray());
//	v.push_back(Gray());
//	v.push_back(LightGray());
//	v.push_back(White());
//	v.push_back(Maroon());
//	v.push_back(Red());
//	v.push_back(Orange());
//	v.push_back(Yellow());
//	v.push_back(Olive());
//	v.push_back(Lime());
//	v.push_back(Green());
//	v.push_back(Cyan());
//	v.push_back(Blue());
//	v.push_back(Navy());
//	v.push_back(Teal());
//	v.push_back(Magenta());
//	v.push_back(Purple());
//	v.push_back(Brown());
//	return v;
}

shared_ptr<Color> Color::Random() {
	return make_shared<Color>(Uniform(0, 255), Uniform(0, 255), Uniform(0, 255));
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Color::Color():
		r(0.0),
		g(0.0),
		b(0.0),
		a(1.0) { }

Color::Color(float r, float g, float b):
		Color(r, g, b, 1.0) { }

Color::Color(float r, float g, float b, float a):
		r(r),
		g(g),
		b(b),
		a(a) { }

Color::Color(int r, int g, int b):
		Color((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, 1.0) { }

Color::Color(int r, int g, int b, int a):
		Color((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f) { }

Color::Color(float black):
		r(black),
		g(black),
		b(black),
		a(1.0) { }

Color::Color(uint32_t color):
		r((float)((color & 0xFF000000) >> 24)/255.0f),
		g((float)((color & 0x00FF0000) >> 16)/255.0f),
		b((float)((color & 0x0000FF00) >> 8)/255.0f),
		a((float)((color & 0x000000FF) >> 0)/255.0f) { }

//Color::Color(const Color& color) {
//	r = color.r;
//	g = color.g;
//	b = color.b;
//	a = color.a;
//}
