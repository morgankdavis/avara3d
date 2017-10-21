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

		
		void pumpManyMouse();
		Window* window() const;
		
		std::set<Key> 				m_keysDown;
		std::set<MouseButton> 		m_mouseButtonsDown;
		glm::vec2  					m_mousePositionDelta;
		glm::vec2  					m_mouseScrollWheelDelta;

	private:

		/***************************************************************************************
     		MARK:   Private
		 **************************************************************************************/

		void initManyMouse();
		void quitManyMouse();
		void registerGLFWCallbacks();
		void unregisterGLFWCallbacks();
		
		void clearKeysDown(); // called after keysDown()
		void clearMouseDown(); // called after mouseButtonsDown()
		void clearMousePositionDelta(); // called after mouseMoveDelta()
		void clearMouseScrollWheelDelta(); // called after mouseScrollWheelDelta()

		Window*						m_window;
		//GLFWwindow*					m_glfwWindow;
	};
}


#endif /* InputManager_h */

