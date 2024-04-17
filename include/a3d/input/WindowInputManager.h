//
//  WindowInputManager.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_WINDOWINPUTMANAGER_H
#define AVARA3D_WINDOWINPUTMANAGER_H

#ifdef DESKTOP


#include <memory>
#include <set>

#include "glm/vec2.hpp"

#include "a3d/Types.h"
#include "a3d/input/InputManager.h"


struct GLFWwindow;


namespace a3d {


	class Window;

	
	class WindowInputManager : public InputManager {

	public:

/*********************************************************************************************
	Public
 *********************************************************************************************/

		explicit WindowInputManager(Window* window);
		WindowInputManager(const InputManager& other) = delete; // copy constructor
		WindowInputManager& operator=(const InputManager& other) = delete; // copy assignment
		~WindowInputManager() override;

/*********************************************************************************************
	Public InputManager
 *********************************************************************************************/

		void 							update() override;

	private:

/*********************************************************************************************
	Private
 *********************************************************************************************/

		static void 					GLFWMouseButtonCallback(GLFWwindow* glfwWindow,
															   int button,
															   int action,
															   int mods);
		static void 					GLFWCursorPositionCallback(GLFWwindow* glfwWindow,
																  double xPos,
																  double yPos);
		static void 					GLFWScrollWheelCallback(GLFWwindow* glfwWindow,
															   double xOffset,
															   double yOffset);
		static void 					GLFWKeyCallback(GLFWwindow* glfwWindow,
													   int key,
													   int scancode,
													   int action,
													   int mods);
		void 							initMouseInput();
		void 							initManyMouse();
		void 							quitManyMouse();
		void 							registerGLFWCallbacks(GLFWwindow* glfwWindow);
		void 							unregisterGLFWCallbacks(GLFWwindow* glfwWindow);

		bool							_usingManyMouse;
		Window*							_window;
	};
}

#endif // DESKTOP


#endif /* AVARA3D_WINDOWINPUTMANAGER_H */
