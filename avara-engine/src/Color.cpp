//
//  Color.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Color.h"


using namespace ae;


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

//static Color red() {
//	return Color(1.0, 0.0, 0.0, 1.0);
//}
//
//static Color black() {
//	return Color(0.0, 0.0, 0.0, 1.0);
//}
//
//static Color blue() {
//	return Color(0.0, 0.0, 1.0, 1.0);
//}
//
//static Color brown() {
//
//}
//
//static Color clear() {
//	return Color(0.0, 0.0, 0.0, 0.0);
//}
//
//static Color cyan() {
//
//}
//
//static Color gray() {
//
//}
//
//static Color lightGray() {
//
//}
//
//static Color darkGray() {
//
//}
//
//static Color green() {
//	return Color(0.0, 1.0, 0.0, 1.0);
//}
//
//static Color magenta() {
//
//}
//
//static Color orange() {
//
//}
//
//static Color purple() {
//
//}
//
//static Color white() {
//	return Color(1.0, 1.0, 1.0, 1.0);
//}
//
//static Color yellow() {
//
//}
