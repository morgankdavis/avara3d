//
//  InputManager.h
//  avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_INPUTMANAGER_H
#define AVARA3D_INPUTMANAGER_H

#include <memory>
#include <set>

#include "glm/vec2.hpp"

#include "a3d/Types.h"

namespace a3d {

	class RenderContext;
	class Scene;

	
	class InputManager {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		InputManager();
		InputManager(const InputManager& other) = delete; // copy constructor
		InputManager& operator=(const InputManager& other) = delete; // copy assignment
//		virtual ~InputManager() = 0;

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		virtual void						update() = 0;
	};
}

#endif /* AVARA3D_INPUTMANAGER_H */
