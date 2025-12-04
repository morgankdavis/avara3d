//
//  Color.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/Color.h"

#include "a3d/Utilities.h"

using namespace a3d;
using namespace glm;
using namespace std;

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

std::shared_ptr<Color> Color::Black() {
	return make_unique<Color>(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
}

std::shared_ptr<Color> Color::DarkGray() {
	return make_unique<Color>(glm::vec4{0.25f, 0.25f, 0.25f, 1.0f});
}

std::shared_ptr<Color> Color::Gray() {
	return make_unique<Color>(glm::vec4{0.5f, 0.5, 0.5f, 1.0f});
}

std::shared_ptr<Color> Color::LightGray() {
	return make_unique<Color>(glm::vec4{0.75f, 0.75f, 0.75f, 1.0f});
}

std::shared_ptr<Color> Color::White() {
	return make_unique<Color>(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
}

std::shared_ptr<Color> Color::Maroon() {
	return make_unique<Color>(glm::vec4{128.0f/255.0f, 0.0f, 0.0f, 1.0f});
}

std::shared_ptr<Color> Color::Red() {
	return make_unique<Color>(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
}

std::shared_ptr<Color> Color::Orange() {
	return make_unique<Color>(glm::vec4{1.0f, 165.0f/255.0f, 0.0f, 1.0f});
}

std::shared_ptr<Color> Color::Yellow() {
	return make_unique<Color>(glm::vec4{1.0f, 1.0f, 0.0f, 1.0f});
}

std::shared_ptr<Color> Color::Olive() {
	return make_unique<Color>(glm::vec4{128.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f});
}

std::shared_ptr<Color> Color::Lime() {
	return make_unique<Color>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
}

std::shared_ptr<Color> Color::Green() {
	return make_unique<Color>(glm::vec4{0.0f, 128.0f/255.0f, 0.0f, 1.0f});
}

std::shared_ptr<Color> Color::Cyan() {
	return make_unique<Color>(glm::vec4{0.0f, 1.0f, 1.0f, 1.0f});
}

std::shared_ptr<Color> Color::Blue() {
	return make_unique<Color>(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

std::shared_ptr<Color> Color::Navy() {
	return make_unique<Color>(glm::vec4{0.0f, 0.0f, 128.0f/255.0f, 1.0f});
}

std::shared_ptr<Color> Color::Teal() {
	return make_unique<Color>(glm::vec4{0.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f});
}

std::shared_ptr<Color> Color::Magenta() {
	return make_unique<Color>(glm::vec4{1.0f, 0.0f, 1.0f, 1.0f});
}

std::shared_ptr<Color> Color::Purple() {
	return make_unique<Color>(glm::vec4{128.0f/255.0f, 0.0f, 128.0f/255.0f, 1.0f});
}

std::shared_ptr<Color> Color::Brown() {
	return make_unique<Color>(glm::vec4{153.0f/255.0f, 102.0f/255.0f, 51.0f/255.0f, 1.0f});
}

shared_ptr<Color> Color::Random() {
	return make_unique<Color>(glm::u8vec3{utils::Uniform(0, 255), utils::Uniform(0, 255), utils::Uniform(0, 255)});
}

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

Color::Color():
		_rgba{1.0, 1.0, 1.0, 1.0} { }

Color::Color(const glm::vec3& rgb):
		_rgba{vec4(rgb, 1.0)} {}

Color::Color(const glm::vec4& rgba):
		_rgba{rgba} {}

Color::Color(const glm::u8vec3& irgb):
		_rgba{(float)irgb.r/255.0f,
			  (float)irgb.g/255.0f,
			  (float)irgb.b/255.0f,
			  1.0} {}

Color::Color(const glm::u8vec4& irgba):
		_rgba{(float)irgba.r/255.0f,
			  (float)irgba.g/255.0f,
			  (float)irgba.b/255.0f,
			  (float)irgba.a/255.0f} {}

Color::Color(float white):
		_rgba{white,
			  white,
			  white,
			  1.0} { }

Color::Color(uint32_t color):
		_rgba{(float)((color & 0xFF000000) >> 24)/255.0f,
			  (float)((color & 0x00FF0000) >> 16)/255.0f,
			  (float)((color & 0x0000FF00) >> 8)/255.0f,
			  (float)((color & 0x000000FF) >> 0)/255.0f} { }

Color::Color(const string& hexString) {
	// TODO
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

float Color::r() const {
	return _rgba.r;
}

float Color::g() const {
	return _rgba.g;
}

float Color::b() const {
	return _rgba.b;
}

float Color::a() const {
	return _rgba.a;
}

uint8_t Color::u8r() const {
	return static_cast<uint8_t>(round(_rgba.r * 255.f));
}

uint8_t Color::u8g() const {
	return static_cast<uint8_t>(round(_rgba.g * 255.f));
}

uint8_t Color::u8b() const {
	return static_cast<uint8_t>(round(_rgba.b * 255.f));
}

uint8_t Color::u8a() const {
	return static_cast<uint8_t>(round(_rgba.a * 255.f));
}

vec3 Color::rgb() const {
	return _rgba;
}

vec4 Color::rgba() const {
	return _rgba;
}

u8vec3 Color::u8rgb() const {
	return u8vec3{u8r(), u8g(), u8b()};
}

u8vec4 Color::u8rgba() const {
	return u8vec4{u8r(), u8g(), u8b(), u8a()};
}
