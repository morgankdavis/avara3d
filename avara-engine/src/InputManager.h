//
//  InputManager.h
//	avara-engine
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef InputManager_h
#define InputManager_h


#include "InputManager.h"

#include <memory>
#include <set>

//#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Types.h"


namespace ae {


	class Window;

	
	class InputManager {

	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/

		InputManager(Window* window);
		~InputManager();

		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		/* IMPLEMENT */ bool keyDown(Key key);
		/* IMPLEMENT */ bool mouseButtonDown(MouseButton button);
		/* IMPLEMENT */ bool stickyKeyDown(Key key);
		/* IMPLEMENT */ bool stickyMouseButtonDown(MouseButton button);

		std::set<Key> keysDown(); // keys currently down
		std::set<MouseButton> mouseButtonsDown(); // mouse buttons down
		
		/* IMPLEMENT */ std::set<Key> stickyKeysDown(); // only reports keys down for one query until they are released
		/* IMPLEMENT */ std::set<MouseButton> stickyMouseButtonsDown(); // only reports mouse buttons down for one query until they are released
		
		glm::vec2 mousePositionDelta(); // mouse position delta since last query
		glm::vec2 mouseScrollWheelDelta(); // mouse wheen scroll delta since last query
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/

		void update(float deltaSeconds);
		Window* window() const;
		
		std::set<Key> 				m_keysDown;
		std::set<MouseButton> 		m_mouseButtonsDown;
		/* IMPLEMENT */ std::set<Key> 				m_stickyKeysDown;
		/* IMPLEMENT */ std::set<MouseButton> 		m_stickyMmouseButtonsDown;
		glm::vec2  					m_mousePositionDelta;
		glm::vec2  					m_mouseScrollWheelDelta;
		
		/***************************************************************************************
		     MARK:   GLFW Callbacks
		 **************************************************************************************/
		
		static void glfwMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods);
		static void glfwCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos);
		static void glfwScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset);
		static void glfwKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);

	private:

		/***************************************************************************************
     		MARK:   Private
		 **************************************************************************************/

		void initManyMouse();
		void quitManyMouse();
		void registerGLFWCallbacks(GLFWwindow* glfwWindow);
		void unregisterGLFWCallbacks(GLFWwindow* glfwWindow);
		
		void clearMousePositionDelta(); // called after mouseMoveDelta()
		void clearMouseScrollWheelDelta(); // called after mouseScrollWheelDelta()

		Window*					m_window;
	};
}


#endif /* InputManager_h */

