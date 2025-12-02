
#ifndef AVARA3D_DESKTOPINPUTMANAGER_H
#define AVARA3D_DESKTOPINPUTMANAGER_H


#include "a3d/input/InputManager.h"


namespace a3d {

	class DesktopInputManager : public InputManager {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		DesktopInputManager();
		DesktopInputManager(const DesktopInputManager& other) = delete; // copy constructor
		DesktopInputManager& operator=(const DesktopInputManager& other) = delete; // copy assignment
		virtual ~DesktopInputManager() = 0;

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
	};
}


#endif //AVARA3D_DESKTOPINPUTMANAGER_H
