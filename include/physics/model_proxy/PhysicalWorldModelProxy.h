//
// Created by mkd on 12/8/23.
//

#ifndef AVARA_ENGINE_PHYSICALWORLDMODELPROXY_H
#define AVARA_ENGINE_PHYSICALWORLDMODELPROXY_H


#include <memory>
#include <vector>

#include "Types.h"


namespace ae {


	class PhysicalWorld;
	class PhysicsBody;
	class Renderer;


	class PhysicalWorldModelProxy {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicalWorldModelProxy(PhysicalWorld* world);
		//virtual PhysicalWorldModelProxyProxy();
		~PhysicalWorldModelProxy();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		virtual void 	add(PhysicsBody& body) = 0;
		virtual void 	remove(PhysicsBody& body) = 0;

		virtual float	gravity() const = 0;
		virtual void	gravity(float gravity) = 0;

		virtual void	step(double deltaT, float speed, float timestep) = 0;

		virtual void 	updateCollisionPairs() = 0;

		virtual void 	drawDebug(Renderer &renderer,
								  const glm::mat4 &viewMat,
								  const glm::mat4 &projectionMat,
								  const DEBUG_OPTIONS &debugOptions) = 0;
	};
}


#endif //AVARA_ENGINE_PHYSICALWORLDMODELPROXY_H
