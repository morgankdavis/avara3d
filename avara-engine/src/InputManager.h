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

#define GLFW_DLL
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

		std::set<Key> keysDown(); // keys down since last query
		std::set<MouseButton> mouseButtonsDown(); // mouse buttons down since last query
		glm::vec2 mousePositionDelta(); // mouse position delta since last query
		glm::vec2 mouseScrollWheelDelta(); // mouse wheen scroll delta since last query
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/

		void update(float deltaSeconds);
		Window* window() const;
		
		std::set<Key> 				m_keysDown;
		std::set<MouseButton> 		m_mouseButtonsDown;
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
//		void initGainput();
		void registerGLFWCallbacks();
		void unregisterGLFWCallbacks();
		
		void clearKeysDown(); // called after keysDown()
		void clearMouseDown(); // called after mouseButtonsDown()
		void clearMousePositionDelta(); // called after mouseMoveDelta()
		void clearMouseScrollWheelDelta(); // called after mouseScrollWheelDelta()

		Window*									m_window;
		//GLFWwindow*							m_glfwWindow;
		//std::shared_ptr<gainput::InputManager>	m_gainputInputManager;
	};
}


#endif /* InputManager_h */

