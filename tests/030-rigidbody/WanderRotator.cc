//
//  WanderRotator.cc
//  030-rigidbody
//
//  Created by Morgan Davis on 7/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

struct test::rigidbody::WanderRotator {
	// tunables
	float minInterval = 0.8f; // seconds
	float maxInterval = 2.5f; // seconds
	float minSpeed  = 0.5f; // rad/s
	float maxSpeed  = 1.0f; // rad/s
	float smoothing = 1.0f; // bigger = snappier

	// state
	float timer = 0.0f;
	float nextChange = 1.0f;

	vec3 angularVelocity = {0, 0, 0}; // current rad/s (axis * speed)
	vec3 targetAngularVelocity = {0, 0, 0}; // desired rad/s

	void chooseNewTarget() {
		vec3 axis = uniform_spherical(1.0);
		float speed = uniform_linear(minSpeed, maxSpeed);
		targetAngularVelocity = axis * speed;
		nextChange = uniform_linear(minInterval, maxInterval);
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
		float angle = length(angularVelocity) * dt;
		if (angle > 1e-6f) {
			vec3 axis = normalize(angularVelocity);
			quat dq = math::quaternion(axis, angle); // implement or use yours
			n.orientation(normalize(dq * n.orientation())); // or n.rotation *= dq depending on convention
		}
	}
};

