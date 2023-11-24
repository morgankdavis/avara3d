//
//  WindowInputManager.h
//	avara-engine
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef WindowInputManager_h
#define WindowInputManager_h

#ifdef DESKTOP


#include "input/InputManager.h"

#include <memory>
#include <set>

#include "glm/vec2.hpp"

#include "Types.h"


struct GLFWwindow;


namespace ae {


	class Window;

	
	class WindowInputManager : public InputManager {

	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		WindowInputManager(std::shared_ptr<Window> window);
		WindowInputManager(const InputManager& other) = delete; // copy constructor
		WindowInputManager& operator=(const InputManager& other) = delete; // copy assignment
		~WindowInputManager();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 										update();

/*********************************************************************************************
	GLFW Callbacks
 *********************************************************************************************/

		static void 								GLFWMouseButtonCallback(GLFWwindow* glfwWindow,
																		   int button,
																		   int action,
																		   int mods);
		static void 								GLFWCursorPositionCallback(GLFWwindow* glfwWindow,
																			  double xPos,
																			  double yPos);
		static void 								GLFWScrollWheelCallback(GLFWwindow* glfwWindow,
																		   double xOffset,
																		   double yOffset);
		static void 								GLFWKeyCallback(GLFWwindow* glfwWindow,
																   int key,
																   int scancode,
																   int action,
																   int mods);
		static std::shared_ptr<WindowInputManager>	InputManagerFromGLFWWindow(GLFWwindow* glfwWindow);
		
	private:

/*********************************************************************************************
	Private
 *********************************************************************************************/

		void 										initMouseMotionInput();
		void 										initManyMouse();
		void 										quitManyMouse();
		void 										registerGLFWCallbacks(GLFWwindow* glfwWindow);
		void 										unregisterGLFWCallbacks(GLFWwindow* glfwWindow);

		std::weak_ptr<Window>						_window;
		bool										_usingManyMouse;
	};
}

#endif // DESKTOP

#endif /* WindowInputManager_h */

