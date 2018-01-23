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
//#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Types.h"


struct GLFWwindow;


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
		
		bool keyDown(Key key);
		bool mouseButtonDown(MouseButton button);
		
		bool keyPressed(Key key);
		bool mouseButtonPressed(MouseButton button);

		std::set<Key> keysDown(); // keys currently down
		std::set<MouseButton> mouseButtonsDown(); // mouse buttons currently down
		
		// only reports keys down for one query until they are released
		std::set<Key> keysPressed();
		// only reports mouse buttons down for one query until they are released
		std::set<MouseButton> mouseButtonsPressed();
		
		glm::vec2 mousePositionDelta(); // mouse position delta since last query
		glm::vec2 mouseScrollWheelDelta(); // mouse wheen scroll delta since last query
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/

		void update();
		Window* window() const;
		
		std::set<Key> 				m_keysDown;
		std::set<MouseButton> 		m_mouseButtonsDown;
		std::set<Key> 				m_keysPressed;
		std::set<Key> 				m_keysPressedCleared;
		std::set<MouseButton> 		m_mouseButtonsPressed;
		std::set<MouseButton> 		m_mouseButtonsPressedCleared;
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

