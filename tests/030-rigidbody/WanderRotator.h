//
//  WanderRotator.h
//  030-rigidbody
//
//  Created by Morgan Davis on 7/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

// TODO: some day, this will go into a3d::extensions

#ifndef AVARA3D_TEST_RIGIDBODY_RANDERROTATOR_H
#define AVARA3D_TEST_RIGIDBODY_RANDERROTATOR_H

#include "a3d/Math.h"

namespace a3d {
	class Node;
}

namespace test::rigidbody {

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

		a3d::math::vec3 angularVelocity = {0, 0, 0}; // current rad/s (axis * speed)
		a3d::math::vec3 targetAngularVelocity = {0, 0, 0}; // desired rad/s

		void chooseNewTarget() {
			a3d::math::vec3 axis = a3d::math::uniform_spherical(1.0);
			float speed = a3d::math::uniform_linear(minSpeed, maxSpeed);
			targetAngularVelocity = axis * speed;
			nextChange = a3d::math::uniform_linear(minInterval, maxInterval);
			timer = 0.0f;
		}

		void update(a3d::Node& n, float dt) {
			timer += dt;
			if (timer >= nextChange) chooseNewTarget();

			// exponential smoothing toward target
			// alpha = 1 - exp(-smoothing * dt)  (frame-rate independent)
			float alpha = 1.0f - a3d::math::exp(-smoothing * dt);
			angularVelocity = angularVelocity + (targetAngularVelocity - angularVelocity) * alpha;

			// integrate into orientation
			float angle = a3d::math::length(angularVelocity) * dt;
			if (angle > 1e-6f) {
				a3d::math::vec3 axis = a3d::math::normalize(angularVelocity);
				a3d::math::quat dq = a3d::math::quaternion(axis, angle); // implement or use yours
				n.orientation(a3d::math::normalize(dq * n.orientation())); // or n.rotation *= dq depending on convention
			}
		}
	};
}

#endif // AVARA3D_TEST_RIGIDBODY_RANDERROTATOR_H
