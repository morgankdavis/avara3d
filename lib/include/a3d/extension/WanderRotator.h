//
//  WanderRotator.h
//  avara3d
//
//  Created by Morgan Davis on 7/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

// TODO: move this to an 'extensions' library

#ifndef AVARA3D_EXTENSIONS_RANDERROTATOR_H
#define AVARA3D_EXTENSIONS_RANDERROTATOR_H

#include "a3d/Math.h"
#include "a3d/scene/Node.h"

namespace a3d {
	class Node;
}

namespace a3d::ext {

	struct WanderRotator {

		// tunables
		float minInterval = 0.8f; // seconds
		float maxInterval = 2.5f; // seconds
		float minSpeed  = 0.5f; // rad/s
		float maxSpeed  = 1.0f; // rad/s
		float smoothing = 1.0f; // bigger = snappier

		// state
		float timer = 0.0f;
		float nextChange = 1.0f;

		math::vec3 angularVelocity = {0, 0, 0}; // current rad/s (axis * speed)
		math::vec3 targetAngularVelocity = {0, 0, 0}; // desired rad/s

		void chooseNewTarget() {
			math::vec3 axis = math::uniform_spherical(1.0);
			float speed = math::uniform_linear(minSpeed, maxSpeed);
			targetAngularVelocity = axis * speed;
			nextChange = math::uniform_linear(minInterval, maxInterval);
			timer = 0.0f;
		}

		void update(Node& n, float dt) {
			timer += dt;
			if (timer >= nextChange) chooseNewTarget();

			// exponential smoothing toward target
			// alpha = 1 - exp(-smoothing * dt)  (frame-rate independent)
			float alpha = 1.0f - math::exp(-smoothing * dt);
			angularVelocity = angularVelocity + (targetAngularVelocity - angularVelocity) * alpha;

			// integrate into orientation
			float angle = a3d::math::length(angularVelocity) * dt;
			if (angle > 1e-6f) {
				math::vec3 axis = math::normalize(angularVelocity);
				math::quat dq = math::quaternion(axis, angle); // implement or use yours
				n.orientation(math::normalize(dq * n.orientation())); // or n.rotation *= dq depending on convention
			}
		}
	};
}

#endif // AVARA3D_EXTENSIONS_RANDERROTATOR_H
