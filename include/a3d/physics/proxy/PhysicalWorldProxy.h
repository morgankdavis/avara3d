//
//  PhysicalWorldProxy.h
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICALWORLDPROXY_H
#define AVARA3D_PHYSICALWORLDPROXY_H

#include <memory>
#include <vector>

#include "a3d/Types.h"

namespace a3d {

	class PhysicalWorld;
	class PhysicsBody;
	class Renderer;
	class RenderContext;

	class PhysicalWorldProxy {

/*********************************************************************************************
	Internal Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit PhysicalWorldProxy(PhysicalWorld& world);
		virtual ~PhysicalWorldProxy();

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		virtual void 	add(PhysicsBody& body) = 0;
		virtual void 	remove(PhysicsBody& body) = 0;

		virtual float	gravity() const = 0;
		virtual void	gravity(float gravity) = 0;

		virtual void	step(double deltaT,
							 float speed,
							 float timestep,
							 Stats& stats) = 0;

		virtual void 	updateCollisionPairs() = 0;

		virtual void 	drawDebug(Renderer &renderer,
								  const RenderContext& context,
								  const glm::mat4 &viewMat,
								  const glm::mat4 &projectionMat,
								  const DebugOptions &debugOptions) = 0;
	};
}

#endif //AVARA3D_PHYSICALWORLDPROXY_H
