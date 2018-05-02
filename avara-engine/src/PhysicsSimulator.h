//
//  PhysicsSimulator.h
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsSimulator_h
#define PhysicsSimulator_h


#include <memory>


namespace ae {

	class PhysicsSimulator : public std::enable_shared_from_this<PhysicsSimulator> {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		PhysicsSimulator();
		
		PhysicsSimulator(const PhysicsSimulator& other) = delete; // copy constructor
		PhysicsSimulator& operator=(const PhysicsSimulator& other) = delete; // copy assignment
		
		virtual ~PhysicsSimulator();
	};
}


#endif /* PhysicsSimulator_h */
