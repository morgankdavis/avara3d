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
     Public Static
 ***************************************************************************************/

shared_ptr<Color> Color::Black() {
	return make_shared<Color>(0.0f, 0.0f, 0.0f, 1.0f);
}

shared_ptr<Color> Color::DarkGray() {
	return make_shared<Color>(0.25f, 0.25f, 0.25f, 1.0f);
}

shared_ptr<Color> Color::Gray() {
	return make_shared<Color>(0.5f, 0.5, 0.5f, 1.0f);
}

shared_ptr<Color> Color::LightGray() {
	return make_shared<Color>(0.75f, 0.75f, 0.75f, 1.0f);
}

shared_ptr<Color> Color::White() {
	return make_shared<Color>(1.0f, 1.0f, 1.0f, 1.0f);
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

shared_ptr<Color> Color::Lime() {
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

vector<shared_ptr<Color>> Color::Rainbow() {
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
}


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

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

Color::Color(float black):
	r(black),
	g(black),
	b(black),
	a(1.0) {
	
}

Color::Color(const Color& color) {
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;
}
