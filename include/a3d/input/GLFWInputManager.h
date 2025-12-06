//
//  GLFWInputManager.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_GLFWINPUTMANAGER_H
#define AVARA3D_GLFWINPUTMANAGER_H

#include <memory>
#include <set>

#include "glm/vec2.hpp"

#include "a3d/Types.h"
#include "a3d/input/DesktopInputManager.h"

struct GLFWwindow;

namespace a3d {

	class GLFWWindow;
	
	class GLFWInputManager : public DesktopInputManager {

	public:
		/// Public Lifecycle Functions ///

		explicit GLFWInputManager(GLFWWindow* window);
		GLFWInputManager(const InputManager& other) = delete; // copy constructor
		GLFWInputManager& operator=(const InputManager& other) = delete; // copy assignment
		~GLFWInputManager() override;

		/// InputManager Internal Member Functions ///

		void 				update() override;

		/// Internal Member Functions ///

		void glfwMouseDeltaEvent(double xDelta, double yDelta);
		void glfwMouseButtonEvent(int button, int action, int mods);
		void glfwScrollEvent(double xOffset, double yOffset);
		void glfwKeyEvent(int key, int scanCode, int action, int mods);

	private:
		/// Private Member Functions ///

		void 				initMouseInput();
		void 				initManyMouse();
		void 				quitManyMouse();

		/// Private Member Variables ///

		GLFWWindow*			_window;
		bool				_usingManyMouse;
	};
}

#endif /* AVARA3D_GLFWINPUTMANAGER_H */
