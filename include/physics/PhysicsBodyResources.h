//
// Created by mkd on 11/13/23.
//

#ifndef AVARA_ENGINE_PHYSICSBODYRESOURCES_H
#define AVARA_ENGINE_PHYSICSBODYRESOURCES_H


#include "Types.h"


namespace ae {


	class PhysicsSimulator;


	class PhysicsBodyResources {

		virtual void	update(PhysicsSimulator& simulator,
							   FrameStats& stats) = 0;
	};
}


#endif //AVARA_ENGINE_PHYSICSBODYRESOURCES_H
