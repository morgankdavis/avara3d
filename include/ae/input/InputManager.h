//
//  InputManager.h
//	avara-engine
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef InputManager_h
#define InputManager_h


#include <memory>
#include <set>

#include "glm/vec2.hpp"

#include "ae/Types.h"


namespace ae {


	class RenderContext;
	class Scene;

	
	class InputManager {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		InputManager();
		InputManager(const InputManager& other) = delete; // copy constructor
		InputManager& operator=(const InputManager& other) = delete; // copy assignment
		~InputManager();

/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		bool 						keyDown(KEY key);
		bool 						mouseButtonDown(MOUSE_BUTTON button);
		
		bool 						keyPressed(KEY key);
		bool 						mouseButtonPressed(MOUSE_BUTTON button);

		std::set<KEY> 				keysDown(); // keys currently down
		std::set<MOUSE_BUTTON> 		mouseButtonsDown(); // mouse buttons currently down
		
		// only reports keys down for one query until they are released
		std::set<KEY> 				keysPressed();
		// only reports mouse buttons down for one query until they are released
		std::set<MOUSE_BUTTON> 		mouseButtonsPressed();
		
		glm::vec2 					mousePositionDelta(); // mouse position delta since last query
		glm::vec2 					mouseScrollWheelDelta(); // mouse wheen scroll delta since last query

		Scene*						scene() const;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void						attachedToScene(Scene* scene);
		void						detachedFromScene(Scene* scene);

		virtual void				update() = 0;

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		std::set<KEY> 				_keysDown;
		std::set<MOUSE_BUTTON> 		_mouseButtonsDown;
		std::set<KEY> 				_keysPressed;
		std::set<KEY> 				_keysPressedCleared;
		std::set<MOUSE_BUTTON> 		_mouseButtonsPressed;
		std::set<MOUSE_BUTTON> 		_mouseButtonsPressedCleared;
		glm::vec2  					_mousePositionDelta;
		glm::vec2  					_mouseScrollWheelDelta;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		void			 			clearMousePositionDelta(); // called after mousePositionDelta()
		void 						clearMouseScrollWheelDelta(); // called after mouseScrollWheelDelta()

		Scene*						_scene;
	};
}

#endif /* InputManager_h */

