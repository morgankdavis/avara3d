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

		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		std::set<KeyCode> keysDown(); // keys down since last query
		std::set<MouseButtonCode> mouseButtonsDown(); // mouse buttons down since last query
		glm::vec2 mouseMoveDelta(); // mouse move delta since last query
		glm::vec2 mouseScrollWheelDelta(); // mouse wheen scroll delta since last query
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/

		void unregisterCallbacks();
		
		Window* window() const;
		
		std::set<KeyCode> 			m_keysDown;
		std::set<MouseButtonCode> 	m_mouseButtonsDown;
		glm::vec2  					m_mouseMoveDelta;
		glm::vec2  					m_mouseScrollWheelDelta;

	private:

		/***************************************************************************************
     		MARK:   Private
		 **************************************************************************************/

		void clearKeysDown(); // called after keysDown()
		void clearMouseDown(); // called after mouseButtonsDown()
		void clearMouseMoveDelta(); // called after mouseMoveDelta()
		void clearMouseScrollWheelDelta(); // called after mouseScrollWheelDelta()

		Window*						m_window;
		//GLFWwindow*					m_glfwWindow;
	};
}


#endif /* InputManager_h */

