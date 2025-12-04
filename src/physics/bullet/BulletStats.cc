//
//  BulletStats.cc
//  avara3d
//
//  Created by Morgan Davis on 4/19/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/bullet/BulletStats.h"

#include "a3d/physics/PhysicsShape.h"

using namespace a3d;
using namespace std;

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

BulletStats::BulletStats():
		numStaticBodies{0},
		numDynamicBodies{0},
		numKinematicBodies{0},
		primitiveShapes{},
		boundingBoxShapes{},
		convexHullShapes{},
		concavePolyhedronShapes{} {}
