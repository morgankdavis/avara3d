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


struct AInputEvent;


namespace ae {


	class Activity;

	
	class AndroidInputManager : public InputManager {

	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		AndroidInputManager(std::shared_ptr<Activity> activity);
		
		AndroidInputManager(const InputManager& other) = delete; // copy constructor
		AndroidInputManager& operator=(const InputManager& other) = delete; // copy assignment
		
		~AndroidInputManager();

		/***************************************************************************************
		     Internal
		 ***************************************************************************************/

		int update(AInputEvent* event);

	private:

		/***************************************************************************************
     		Private
		 ***************************************************************************************/

		std::weak_ptr<Activity> 		m_activity;
	};
}

#endif // ANDROID

#endif /* AndroidInputManager_h */

