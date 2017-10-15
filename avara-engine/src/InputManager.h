////
////  InputManager.h
////	avara-engine
////
////  Created by Morgan Davis on 10/9/17.
////  Copyright © 2017 Morgan K Davis. All rights reserved.
////
//
//#ifndef InputManager_h
//#define InputManager_h
//
//
//#include "InputManager.h"
//
////#include <map>
//#include <memory>
//#include <vector>
//
//#include <glm/vec2.hpp>
//
//
//namespace ae {
//
//
//	typedef int KeyCode;
//	typedef int MouseButtonCode;
//
//
//	class Window;
//
//	
//	class InputManager {
//
//	public:
//
//		/***************************************************************************************
//		     MARK:   Lifecycle
//		 **************************************************************************************/
//
//		InputManager(const std::shared_ptr<Window> window);
//
//		/***************************************************************************************
//		     MARK:   Public
//		 **************************************************************************************/
//
//		std::vector<KeyCode> keysDown(); // keys down since last query
//		std::vector<MouseButtonCode> mouseButtonsDown(); // mouse buttons down since last query
//		glm::vec2 mouseMoveDelta(); // mouse move delta since last query
//		glm::vec2 mouseScrollWheelDelta(); // mouse wheen scroll delta since last query
//
//		std::shared_ptr<Window> window() const;
//
//	private:
//
//		/***************************************************************************************
//     		MARK:   Private
//		 **************************************************************************************/
//
//		//void clearKeysDown();
//		void clearMouseDown(); // called after mouseButtonsDown()
//		void clearMouseMoveDelta(); // called after mouseMoveDelta()
//		void clearMouseScrollWheelDelta(); // called after mouseScrollWheelDelta()
//
//		std::shared_ptr<Window>		m_window;
//		//GLFWwindow					*m_glfwWindow;
//
//		std::vector<KeyCode> m_keysDown;
//		std::vector<MouseButtonCode> m_mouseButtonsDown;
//		glm::vec2  m_mouseMoveDelta;
//		glm::vec2  m_mouseScrollWheelDelta;
//	};
//}
//
//
//#endif /* InputManager_h */

