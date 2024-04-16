//
//  ActivityInputManager.h
//	avara3d
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_ACTIVITYINPUTMANAGER_H
#define AVARA3D_ACTIVITYINPUTMANAGER_H

#ifdef ANDROID


#include "a3d/InputManager.h"

#include <memory>
#include <set>

#include <glm/vec2.hpp>

#include "Types.h"


struct AInputEvent;


namespace a3d {


	class Activity;

	
	class ActivityInputManager: public InputManager {

	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		ActivityInputManager(std::shared_ptr<Activity> activity);
		
		ActivityInputManager(const InputManager& other) = delete; // copy constructor
		ActivityInputManager& operator=(const InputManager& other) = delete; // copy assignment
		
		~ActivityInputManager();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		int update(AInputEvent* event);

	private:

/*********************************************************************************************
	Private
 *********************************************************************************************/

		void mouseButton(MOUSE_BUTTON button, bool down);

		std::weak_ptr<Activity> 		_activity;
	};
}

#endif // ANDROID

#endif /* AVARA3D_ACTIVITYINPUTMANAGER_H */

