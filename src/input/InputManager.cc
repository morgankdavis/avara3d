//
//  InputManager.cc
//  avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/input/InputManager.h"

#include "a3d/diagnostic/log/Log.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

InputManager::InputManager()/*:
		_scene{}*/ { }

//Scene* InputManager::scene() const {
//	return _scene;
//}

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

//void InputManager::attachedToScene(Scene& scene) {
//	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));
//
//	_scene = &scene;
//}
//
//void InputManager::detachedFromScene(Scene& scene) {
//	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));
//
//	_scene = nullptr;
//}
