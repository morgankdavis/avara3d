//
//  PhysicalWorld.h
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICALWORLD_H
#define AVARA3D_PHYSICALWORLD_H


#include <functional>
#include <memory>
#include <optional>

#include "glm/glm.hpp"

#include "a3d/Types.h"


namespace a3d {
	

	class HitTestResult;
	class PhysicsBody;
	class PhysicsContact;
	class PhysicsShape;
	class PhysicalWorldProxy;
	class Scene;
	
	
	class PhysicalWorld {

/*********************************************************************************************
	Public Types
 *********************************************************************************************/

	public:

		using DidSimulateCallback = 	std::function<void(PhysicalWorld& world, double time, double deltaTime)>;
		using BeginContactCallback = 	std::function<void(PhysicalWorld& world, PhysicsContact& contact)>;
		using ContinueContactCallback =	std::function<void(PhysicalWorld& world, PhysicsContact& contact)>;
		using EndContactCallback = 		std::function<void(PhysicalWorld& world, PhysicsContact& contact)>;

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		PhysicalWorld();
		PhysicalWorld(const PhysicalWorld& other) = delete; // copy constructor
		PhysicalWorld& operator=(const PhysicalWorld& other) = delete; // copy assignment
		~PhysicalWorld();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		const glm::vec3&					gravity() const;
		void 								gravity(const glm::vec3& gravity);

		float								speed() const;
		void 								speed(float speed);

		float 								timestep() const;
		void 								timestep(float timestep);

		std::optional<PhysicsContact>		contactTest(const PhysicsBody& bodyA,
														 const PhysicsBody& bodyB);
		std::optional<PhysicsContact> 		contactTest(const PhysicsBody& body);
		std::optional<HitTestResult> 		rayTest(const glm::vec3& fromVec,
													const glm::vec3& toVec);
		std::optional<PhysicsContact> 		convexSweepTest(const PhysicsContact& contact,
															 const glm::mat4& fromMat,
															 const glm::mat4& toMat);

		void 								updateCollisionPairs();

		Scene*								scene() const;

		// TODO: willSimulateCallback ?

		DidSimulateCallback					didSimulateCallback() const;
		void								didSimulateCallback(DidSimulateCallback function);

		BeginContactCallback 				beginContactCallback() const;
		void 								beginContactCallback(PhysicalWorld::BeginContactCallback function);

		ContinueContactCallback				continueContactCallback() const;
		void 								continueContactCallback(PhysicalWorld::ContinueContactCallback function);

		EndContactCallback 					endContactCallback() const;
		void 								endContactCallback(PhysicalWorld::EndContactCallback function);

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		void								attachedToScene(Scene& scene);
		void								detachedFromScene(Scene& scene);

		void 								add(PhysicsBody& body);
		void 								remove(PhysicsBody& body);

		void								step(const Scene& scene,
												 double runT,
												 double deltaRunT,
												 Stats& stats);

		PhysicalWorldProxy*					proxy() const;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		glm::vec3 								_gravity;
		float 									_speed;
		float 									_timestep;
		std::unique_ptr<PhysicalWorldProxy>		_proxy;
		Scene*									_scene;
		DidSimulateCallback						_didSimulate;
		BeginContactCallback					_beginContact;
		ContinueContactCallback					_continueContact;
		EndContactCallback						_endContact;
	};
}


#endif /* AVARA3D_PHYSICALWORLD_H */
