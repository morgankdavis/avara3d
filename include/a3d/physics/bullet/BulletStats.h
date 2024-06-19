//
//  BulletStats.h
//  avara3d
//
//  Created by Morgan Davis on 4/19/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BULLETSTATS_H
#define AVARA3D_BULLETSTATS_H


#include <memory>
#include <unordered_set>


namespace a3d {


	class PhysicsShape;


	struct BulletStats {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		BulletStats();

/*********************************************************************************************
	Internal Member Variables
 *********************************************************************************************/

		unsigned 	numStaticBodies;
		unsigned 	numDynamicBodies;
		unsigned 	numKinematicBodies;
		std::unordered_set<PhysicsShape*> 	primitiveShapes;
		std::unordered_set<PhysicsShape*> 	boundingBoxShapes;
		std::unordered_set<PhysicsShape*> 	convexHullShapes;
		std::unordered_set<PhysicsShape*>	concavePolyhedronShapes;
	};
}


#endif //AVARA3D_BULLETSTATS_H
