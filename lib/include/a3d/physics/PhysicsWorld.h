//
//  PhysicsWorld.h
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PHYSICSWORLD_H
#define AVARA3D_PHYSICS_PHYSICSWORLD_H

#include <functional>
#include <memory>
#include <optional>

#include "a3d/scene/Scene.h"

namespace a3d {

	struct FrameStats;

	class HitTestResult;
	class Line;
	class PhysicsBody;
	class PhysicsContact;
	class PhysicsShape;
	class PhysicsWorldProxy;
	class Profiler;

	class PhysicsWorld {

	public:
		/// Public Types ///

		using DidSimulateCallback = 	std::function<void(PhysicsWorld& world, double time, double deltaTime)>;
		using BeginContactCallback = 	std::function<void(PhysicsWorld& world, PhysicsContact& contact)>;
		using ContinueContactCallback =	std::function<void(PhysicsWorld& world, PhysicsContact& contact)>;
		using EndContactCallback = 		std::function<void(PhysicsWorld& world, PhysicsContact& contact)>;

		/// Public Lifecycle Functions ///

		PhysicsWorld();

		PhysicsWorld(const PhysicsWorld&) = delete;
		PhysicsWorld& operator=(const PhysicsWorld&) = delete;

		PhysicsWorld(PhysicsWorld&&) = delete;
		PhysicsWorld& operator=(PhysicsWorld&&) = delete;

		~PhysicsWorld();

		/// Public Member Functions ///

		const math::vec3&					gravity() const;
		void 								gravity(const math::vec3& gravity);

		float								speed() const;
		void 								speed(float speed);

		float 								timestep() const;
		void 								timestep(float timestep);

		std::optional<PhysicsContact>		contactTest(const PhysicsBody& bodyA,
														 const PhysicsBody& bodyB);
		std::optional<PhysicsContact> 		contactTest(const PhysicsBody& body);
		std::optional<HitTestResult> 		rayTest(const math::vec3& fromVec,
													const math::vec3& toVec);
		std::optional<PhysicsContact> 		convexSweepTest(const PhysicsContact& contact,
															 const math::mat4& fromMat,
															 const math::mat4& toMat);

		void 								updateCollisionPairs();

		Scene*								scene() const;

		// TODO: willSimulateCallback ?

		DidSimulateCallback					didSimulateCallback() const;
		void								didSimulateCallback(DidSimulateCallback function);

		BeginContactCallback 				beginContactCallback() const;
		void 								beginContactCallback(PhysicsWorld::BeginContactCallback function);

		ContinueContactCallback				continueContactCallback() const;
		void 								continueContactCallback(PhysicsWorld::ContinueContactCallback function);

		EndContactCallback 					endContactCallback() const;
		void 								endContactCallback(PhysicsWorld::EndContactCallback function);

		/// Internal Member Functions ///

		void								attachedToScene(Scene& scene);
		void								detachedFromScene(Scene& scene);

		void 								add(PhysicsBody& body);
		void 								remove(PhysicsBody& body);

		void								step(const Scene& scene,
												 double runT,
												 double deltaRunT,
												 FrameStats& stats,
												 Profiler& profiler);

		void 								appendDebugLines(std::vector<Line>& out,
															 Scene::DebugOptions debugOptions) const;

		PhysicsWorldProxy*					proxy() const;

	private:
		/// Private Member Variables ///

		math::vec3 								_gravity;
		float 									_speed;
		float 									_timestep;
		std::unique_ptr<PhysicsWorldProxy>		_proxy;
		Scene*									_scene;
		DidSimulateCallback						_didSimulateCallback;
		BeginContactCallback					_beginContactCallback;
		ContinueContactCallback					_continueContactCallback;
		EndContactCallback						_endContactCallback;
	};
}

#endif /* AVARA3D_PHYSICS_PHYSICSWORLD_H */
