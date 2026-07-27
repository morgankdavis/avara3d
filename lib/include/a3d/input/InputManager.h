//
//  InputManager.h
//  avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_INPUT_INPUTMANAGER_H
#define AVARA3D_INPUT_INPUTMANAGER_H

#include <memory>
#include <set>

namespace a3d {

	class RenderContext;
	class Scene;

	class InputManager {

	public:
		/// Public Lifecycle Functions ///

		InputManager();

		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;

		InputManager(InputManager&&) = delete;
		InputManager& operator=(InputManager&&) = delete;

		virtual ~InputManager();

		/// Internal Member Functions ///

		virtual void	update() = 0;
		virtual void	attachedToScene(Scene& scene) = 0;
		virtual void	visualWorldAttachedToScene(Scene& scene) = 0;
	};
}

#endif /* AVARA3D_INPUT_INPUTMANAGER_H */
