//
//  PhysicsWorld.h
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PHYSICSWORLD_H
#define AVARA3D_PHYSICS_PHYSICSWORLD_H

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "a3d/Math.h"
#include "a3d/physics/PhysicsInventory.h"
#include "a3d/scene/Scene.h"

namespace a3d {

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

		struct StepInfo {

			// index of the simulation tick containing this physics step
			std::uint64_t tickIndex{0};

			// simulation time before the containing tick
			double startTime{0.0};

			// simulation time after the containing tick completes
			double endTime{0.0};

			// delta passed to the physics backend
			double deltaTime{0.0};
		};

		using WillStepCallback = std::function<void(PhysicsWorld &physicsWorld,
		                                            const StepInfo& info)>;
		using DidStepCallback = std::function<void(PhysicsWorld &physicsWorld,
		                                           const StepInfo &info)>;
		using BeginContactCallback = std::function<void(PhysicsWorld &physicsWorld,
		                                                PhysicsContact &contact)>;
		using ContinueContactCallback = std::function<void(PhysicsWorld &physicsWorld,
		                                                   PhysicsContact &contact)>;
		using EndContactCallback = std::function<void(PhysicsWorld &physicsWorld,
		                                              PhysicsContact& contact)>;

		/// Public Lifecycle Functions ///

		PhysicsWorld();

		PhysicsWorld(const PhysicsWorld&) = delete;
		PhysicsWorld& operator=(const PhysicsWorld&) = delete;

		PhysicsWorld(PhysicsWorld&&) = delete;
		PhysicsWorld& operator=(PhysicsWorld&&) = delete;

		~PhysicsWorld();

		/// Public Member Functions ///

		const math::vec3&				gravity() const;
		void 							gravity(const math::vec3& gravity);

		std::optional<PhysicsContact>	contactTest(const PhysicsBody& bodyA,
			                                         const PhysicsBody& bodyB);
		std::optional<PhysicsContact> 	contactTest(const PhysicsBody& body);
		std::optional<HitTestResult> 	rayTest(const math::vec3& fromVec,
		                                        const math::vec3& toVec);
		std::optional<PhysicsContact> 	convexSweepTest(const PhysicsContact& contact,
		                                                 const math::mat4 &fromMat,
		                                                 const math::mat4& toMat);

		void 							updateCollisionPairs();

		Scene*							scene() const;

		WillStepCallback				willStepCallback() const;
		void							willStepCallback(WillStepCallback function);

		DidStepCallback					didStepCallback() const;
		void							didStepCallback(DidStepCallback function);

		BeginContactCallback 			beginContactCallback() const;
		void 							beginContactCallback(BeginContactCallback function);

		ContinueContactCallback			continueContactCallback() const;
		void 							continueContactCallback(ContinueContactCallback function);

		EndContactCallback 				endContactCallback() const;
		void 							endContactCallback(EndContactCallback function);

		/// Internal Member Functions ///

		void							attachedToScene(Scene& scene);
		void							detachedFromScene(Scene& scene);

		void 							add(PhysicsBody& body);
		void 							remove(PhysicsBody& body);

		bool							acceptsStepDelta(double deltaTime) const;

		PhysicsInventory				step(const StepInfo& info, Profiler& profiler);

		PhysicsInventory				inventory() const;

		void 							appendDebugLines(std::vector<Line>& out,
								                         Scene::DebugOptions debugOptions) const;

		PhysicsWorldProxy*				proxy() const;

	private:
		/// Private Member Variables ///

		math::vec3 							_gravity;
		std::unique_ptr<PhysicsWorldProxy>	_proxy;
		Scene*								_scene;
		WillStepCallback					_willStepCallback;
		DidStepCallback						_didStepCallback;
		BeginContactCallback				_beginContactCallback;
		ContinueContactCallback				_continueContactCallback;
		EndContactCallback					_endContactCallback;
	};
}

#endif /* AVARA3D_PHYSICS_PHYSICSWORLD_H */
