//
//  Configuration.h
//  avara3d
//
//  Created by Morgan Davis on 1/19/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CONFIGURATION_H
#define AVARA3D_CONFIGURATION_H

namespace a3d::config {

	const std::chrono::milliseconds	FRAMETIME_AVERAGING_INTERVAL	{250};
	const std::chrono::milliseconds	FRAME_STATS_HISTORY_DURATION 	{3000};
	const unsigned 					MAX_PHYSICS_SUBSTEPS 			{1};

	// for now, just add all lights.
	// when we start doing spacial partitioning we will be smarter about lights.
	const unsigned 					MAX_AMBIENT_LIGHTS				{16};
	const unsigned 					MAX_DIRECTIONAL_LIGHTS			{16};
	const unsigned 					MAX_POINT_LIGHTS				{128};
	const unsigned 					MAX_SPOT_LIGHTS					{64};
}

#endif //AVARA3D_CONFIGURATION_H
