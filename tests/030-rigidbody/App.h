//
//  App.h
//  030-rigidbody
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TEST_RIGIDBODY_APP_H
#define AVARA3D_TEST_RIGIDBODY_APP_H

#include <memory>
#include <tuple>
#include <vector>

#include "a3d/Application.h"
#include "a3d/Math.h"

namespace a3d {
	class InputContext;
	class Mesh;
	class Node;
	class PhysicsShape;
	class PhysicsWorld;
	class Scene;
	class VisualWorld;
	class Window;
}

namespace a3d::ext {
	struct WanderRotator;
}

namespace test::rigidbody {

	class App : public a3d::Application {

	public:
		/// Public Lifecycle Functions ///

		App(int argc, char* argv[]);
		~App() override;

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		App(App&&) = delete;
		App& operator=(App&&) = delete;

		/// Internal Types ///

		using DuckFruitDef = std::tuple<
			std::shared_ptr<a3d::Mesh>,
			std::shared_ptr<a3d::PhysicsShape>,
			float>;

	protected:
		/// Protected Member Functions ///

		std::unique_ptr<a3d::Scene>		init() override;
		a3d::SimulationConfig			simulationConfig() const override;
		bool							shouldContinue(const a3d::Scene& scene) override;
		void							didShutdown() override;

		/// Input Context Callbacks ///

		void		inputContextDidUpdate(a3d::InputContext &inputContext,
				                          const a3d::InputContext::UpdateInfo &info) override;

		/// Simulation Callbacks ///

		void		simulationWillTick(a3d::Scene &scene,
				                       const a3d::Scene::TickInfo& info) override;

		/// Visual World Callbacks ///

		void		visualWorldWillRender(a3d::VisualWorld &visualWorld,
				                          const a3d::VisualWorld::RenderInfo& info) override;
		void		visualWorldDidRender(a3d::VisualWorld &visualWorld,
				                         const a3d::VisualWorld::RenderInfo &info) override;

	private:
		/// Private Member Variables ///

		std::unique_ptr<a3d::Window>				_window;
		a3d::Node*									_duckNode;
		std::vector<DuckFruitDef>					_duckFruit;
		std::unique_ptr<a3d::ext::WanderRotator>	_duckRotator;
		float										_cameraMoveSpeed;
		bool										_spawnDuckFruit;
		bool										_spawnDuckFruitPending;
		double										_duckFruitSpawnAccumulator;
		bool										_shootSlurm;
		bool										_shootSlurmPending;
		double										_slurmShotAccumulator;
		a3d::math::vec3								_slurmLocation;
		a3d::math::vec3								_slurmDirection;
	};
}

#endif // AVARA3D_TEST_RIGIDBODY_APP_H
