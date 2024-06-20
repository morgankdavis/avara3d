//
//  InputManager.h
//  avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_INPUTMANAGER_H
#define AVARA3D_INPUTMANAGER_H


#include <memory>
#include <set>

#include "glm/vec2.hpp"

#include "a3d/Types.h"


namespace a3d {


	class RenderContext;
	class Scene;

	
	class InputManager {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		InputManager();
		InputManager(const InputManager& other) = delete; // copy constructor
		InputManager& operator=(const InputManager& other) = delete; // copy assignment
		virtual ~InputManager();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		bool 								keyDown(Key key);
		bool 								mouseButtonDown(MouseButton button);
		
		bool 								keyPressed(Key key);
		bool 								mouseButtonPressed(MouseButton button);

		std::unordered_set<Key> 			keysDown(); // keys currently down
		std::unordered_set<MouseButton> 	mouseButtonsDown(); // mouse buttons currently down
		
		// only reports keys down for one query until they are released
		std::unordered_set<Key> 			keysPressed();
		// only reports mouse buttons down for one query until they are released
		std::unordered_set<MouseButton> 	mouseButtonsPressed();
		
		glm::vec2 							mousePositionDelta(); // mouse position delta since last query
		glm::vec2 							mouseScrollWheelDelta(); // mouse wheen scroll delta since last query

		Scene*								scene() const; // eh

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		void								attachedToScene(Scene& scene);
		void								detachedFromScene(Scene& scene);

		virtual void						update() = 0;

/*********************************************************************************************
	Protected Member Variables
 *********************************************************************************************/

	protected:

		std::unordered_set<Key> 			_keysDown;
		std::unordered_set<MouseButton> 	_mouseButtonsDown;
		std::unordered_set<Key> 			_keysPressed;
		std::unordered_set<Key> 			_keysPressedCleared;
		std::unordered_set<MouseButton> 	_mouseButtonsPressed;
		std::unordered_set<MouseButton> 	_mouseButtonsPressedCleared;
		glm::vec2  							_mousePositionDelta;
		glm::vec2  							_mouseScrollWheelDelta;

/*********************************************************************************************
	Private Member Functions
 *********************************************************************************************/

	private:

		void			 			clearMousePositionDelta(); // called after mousePositionDelta()
		void 						clearMouseScrollWheelDelta(); // called after mouseScrollWheelDelta()

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		Scene*						_scene;
	};
}

#endif /* AVARA3D_INPUTMANAGER_H */

