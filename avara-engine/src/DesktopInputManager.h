//
//  DesktopInputManager.h
//	avara-engine
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef DesktopInputManager_h
#define DesktopInputManager_h

#ifdef DESKTOP


#include "InputManager.h"

#include <memory>
#include <set>

#include <glm/vec2.hpp>

#include "Types.h"


struct GLFWwindow;


namespace ae {


	class Window;

	
	class DesktopInputManager: public InputManager {

	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		DesktopInputManager(std::shared_ptr<Window> window);
		
		DesktopInputManager(const InputManager& other) = delete; // copy constructor
		DesktopInputManager& operator=(const InputManager& other) = delete; // copy assignment
		
		~DesktopInputManager();

		/***************************************************************************************
		     Internal
		 ***************************************************************************************/

		void update();
		
//		void window(std::shared_ptr<Window> window) const;
//		std::weak_ptr<Window> window() const;

		/***************************************************************************************
		     GLFW Callbacks
		 ***************************************************************************************/
		
		static void glfwMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods);
		static void glfwCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos);
		static void glfwScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset);
		static void glfwKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);

	private:

		/***************************************************************************************
     		Private
		 ***************************************************************************************/

		void initManyMouse();
		void quitManyMouse();
		void registerGLFWCallbacks(GLFWwindow* glfwWindow);
		void unregisterGLFWCallbacks(GLFWwindow* glfwWindow);

		std::weak_ptr<Window>			m_window;
	};
}

#endif // DESKTOP

#endif /* DesktopInputManager_h */

