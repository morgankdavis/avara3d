//
//  Configuration.h
//  avara3d
//
//  Created by Morgan Davis on 1/19/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CONFIGURATION_H
#define AVARA3D_CONFIGURATION_H


namespace a3d {

	constexpr double 	FRAMETIME_AVERAGING_INTERVAL = 	0.5;
	constexpr unsigned 	MAX_PHYSICS_SUBSTEPS = 			1;

	// for now, just add all lights.
	// when we start doing spacial partitioning we will be smarter about lights.
//	constexpr unsigned 	MAX_AMBIENT_LIGHTS =			4;
//	constexpr unsigned 	MAX_DIRECTIONAL_LIGHTS =		4;
//	constexpr unsigned 	MAX_POINT_LIGHTS =				64;
//	constexpr unsigned 	MAX_SPOT_LIGHTS =				16;
}


#endif //AVARA3D_CONFIGURATION_H
