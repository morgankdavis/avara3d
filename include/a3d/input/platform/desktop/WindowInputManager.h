//
//  WindowInputManager.h
//	avara3d
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef WindowInputManager_h
#define WindowInputManager_h

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

		static std::shared_ptr<WindowInputManager>	InputManagerFromGLFWWindow(GLFWwindow* glfwWindow);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		WindowInputManager(Window* window);
		WindowInputManager(const InputManager& other) = delete; // copy constructor
		WindowInputManager& operator=(const InputManager& other) = delete; // copy assignment
		~WindowInputManager();

/*********************************************************************************************
	InputManager
 *********************************************************************************************/

		void 							update() override;

/*********************************************************************************************
	GLFW Callbacks
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

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

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


#endif /* WindowInputManager_h */
