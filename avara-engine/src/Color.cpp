//
//  Color.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Color.h"

#include <vector>


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Static
 **************************************************************************************/

Color Color::Black() {
	return Color(0.0f, 0.0f, 0.0f, 1.0f);
}

Color Color::White() {
	return Color(1.0f, 1.0f, 1.0f, 1.0f);
}

Color Color::Gray() {
	return Color(0.5f, 0.5, 0.5f, 1.0f);
}

Color Color::LightGray() {
	return Color(0.75f, 0.75f, 0.75f, 1.0f);
}

Color Color::DarkGray() {
	return Color(0.25f, 0.25f, 0.25f, 1.0f);
}

Color Color::Maroon() {
	return Color(128.0f/255.0f, 0.0f, 0.0f, 1.0f);
}

Color Color::Red() {
	return Color(1.0f, 0.0f, 0.0f, 1.0f);
}

Color Color::Orange() {
	return Color(1.0f, 165.0f/255.0f, 0.0f, 1.0f);
}

Color Color::Yellow() {
	return Color(1.0f, 1.0f, 0.0f, 1.0f);
}

Color Color::Olive() {
	return Color(128.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

Color Color::Lime() {
	return Color(0.0f, 1.0f, 0.0f, 1.0f);
}

Color Color::Green() {
	return Color(0.0f, 128.0f/255.0f, 0.0f, 1.0f);
}

Color Color::Cyan() {
	return Color(0.0f, 1.0f, 1.0f, 1.0f);
}

Color Color::Blue() {
	return Color(0.0f, 0.0f, 1.0f, 1.0f);
}

Color Color::Navy() {
	return Color(0.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

Color Color::Teal() {
	return Color(0.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f);
}

Color Color::Magenta() {
	return Color(1.0f, 0.0f, 1.0f, 1.0f);
}

Color Color::Purple() {
	return Color(128.0f/255.0f, 0.0f, 128.0f/255.0f, 1.0f);
}

Color Color::Brown() {
	return Color(153.0f/255.0f, 102.0f/255.0f, 51.0f/255.0f, 1.0f);
}

vector<Color> Color::Rainbow() {
	auto colors = vector<Color>();
	colors.emplace_back(Black());
	colors.emplace_back(White());
	colors.emplace_back(Gray());
	colors.emplace_back(LightGray());
	colors.emplace_back(DarkGray());
	colors.emplace_back(Maroon());
	colors.emplace_back(Red());
	colors.emplace_back(Orange());
	colors.emplace_back(Yellow());
	colors.emplace_back(Olive());
	colors.emplace_back(Lime());
	colors.emplace_back(Green());
	colors.emplace_back(Cyan());
	colors.emplace_back(Blue());
	colors.emplace_back(Navy());
	colors.emplace_back(Teal());
	colors.emplace_back(Magenta());
	colors.emplace_back(Purple());
	colors.emplace_back(Brown());
	return colors;
}


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

Color::Color(const float black):
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
