//
//  InputContext.cc
//  avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/input/InputContext.h"

using namespace a3d;
using namespace std;

/// Public Lifecycle Functions ///

InputContext::InputContext():
	_didUpdateCallback{} {}
InputContext::~InputContext() {}

/// Public Member Functions ///

InputContext::DidUpdateCallback InputContext::didUpdateCallback() const {
	return _didUpdateCallback;
}

void InputContext::didUpdateCallback(DidUpdateCallback callback) {
	_didUpdateCallback = callback;
}
