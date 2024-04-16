//
//  InputManager.h
//	avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef InputManager_h
#define InputManager_h


#include <memory>
#include <set>

#include "glm/vec2.hpp"

#include "a3d/Types.h"


namespace a3d {


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
		virtual ~InputManager();

/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		bool 						keyDown(Key key);
		bool 						mouseButtonDown(MouseButton button);
		
		bool 						keyPressed(Key key);
		bool 						mouseButtonPressed(MouseButton button);

		std::set<Key> 				keysDown(); // keys currently down
		std::set<MouseButton> 		mouseButtonsDown(); // mouse buttons currently down
		
		// only reports keys down for one query until they are released
		std::set<Key> 				keysPressed();
		// only reports mouse buttons down for one query until they are released
		std::set<MouseButton> 		mouseButtonsPressed();
		
		glm::vec2 					mousePositionDelta(); // mouse position delta since last query
		glm::vec2 					mouseScrollWheelDelta(); // mouse wheen scroll delta since last query

		Scene*						scene() const;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void						attachedToScene(Scene& scene);
		void						detachedFromScene(Scene& scene);

		virtual void				update() = 0;

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		std::set<Key> 				_keysDown;
		std::set<MouseButton> 		_mouseButtonsDown;
		std::set<Key> 				_keysPressed;
		std::set<Key> 				_keysPressedCleared;
		std::set<MouseButton> 		_mouseButtonsPressed;
		std::set<MouseButton> 		_mouseButtonsPressedCleared;
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

