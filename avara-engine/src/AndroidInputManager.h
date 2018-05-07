//
//  AndroidInputManager.h
//	avara-engine
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef AndroidInputManager_h
#define AndroidInputManager_h

#ifdef ANDROID


#include "InputManager.h"

#include <memory>
#include <set>

#include <glm/vec2.hpp>

#include "Types.h"



namespace ae {


	class Activity;

	
	class AndroidInputManager: public InputManager {

	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		AndroidInputManager(std::shared_ptr<Activity> activity);
		
		AndroidInputManager(const InputManager& other) = delete; // copy constructor
		AndroidInputManager& operator=(const InputManager& other) = delete; // copy assignment
		
		~AndroidInputManager();

		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
//		bool keyDown(KEY key);
//		bool mouseButtonDown(MOUSE_BUTTON button);
//		
//		bool keyPressed(KEY key);
//		bool mouseButtonPressed(MOUSE_BUTTON button);
//
//		std::set<KEY> keysDown(); // keys currently down
//		std::set<MOUSE_BUTTON> mouseButtonsDown(); // mouse buttons currently down
//		
//		// only reports keys down for one query until they are released
//		std::set<KEY> keysPressed();
//		// only reports mouse buttons down for one query until they are released
//		std::set<MOUSE_BUTTON> mouseButtonsPressed();
//		
//		glm::vec2 mousePositionDelta(); // mouse position delta since last query
//		glm::vec2 mouseScrollWheelDelta(); // mouse wheen scroll delta since last query
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/

		void update(AInputEvent* event);
		
		std::weak_ptr<Activity> activity() const;
		
//		std::set<KEY> 				m_keysDown;
//		std::set<MOUSE_BUTTON> 		m_mouseButtonsDown;
//		std::set<KEY> 				m_keysPressed;
//		std::set<KEY> 				m_keysPressedCleared;
//		std::set<MOUSE_BUTTON> 		m_mouseButtonsPressed;
//		std::set<MOUSE_BUTTON> 		m_mouseButtonsPressedCleared;
//		glm::vec2  					m_mousePositionDelta;
//		glm::vec2  					m_mouseScrollWheelDelta;
		
	private:

		/***************************************************************************************
     		Private
		 ***************************************************************************************/

//		void initManyMouse();
//		void quitManyMouse();
//		void registerGLFWCallbacks(GLFWwindow* glfwWindow);
//		void unregisterGLFWCallbacks(GLFWwindow* glfwWindow);
//		
//		void clearMousePositionDelta(); // called after mouseMoveDelta()
//		void clearMouseScrollWheelDelta(); // called after mouseScrollWheelDelta()

		std::weak_ptr<Activity> 		m_activity;
	};
}

#endif // ANDROID

#endif /* AndroidInputManager_h */

