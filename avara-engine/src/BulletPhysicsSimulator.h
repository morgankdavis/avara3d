//
//  BulletPhysicsSimulator.h
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef BulletPhysicsSimulator_h
#define BulletPhysicsSimulator_h


#include <memory>

#include "PhysicsSimulator.h"


namespace ae {
	
	class BulletPhysicsSimulator : public PhysicsSimulator {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		BulletPhysicsSimulator();
		
		BulletPhysicsSimulator(const BulletPhysicsSimulator& other) = delete; // copy constructor
		BulletPhysicsSimulator& operator=(const BulletPhysicsSimulator& other) = delete; // copy assignment
		
		virtual ~BulletPhysicsSimulator();
	};
}


#endif /* BulletPhysicsSimulator_h */
