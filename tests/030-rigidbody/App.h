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

namespace a3d {
	class Mesh;
	class Node;
	class PhysicsShape;
	class Scene;
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

		/// Runner Callbacks ///

		void							hostUpdate(a3d::Runner& runner,
									               const a3d::Runner::UpdateInfo& info) override;

		/// Scene Callbacks ///

		void							sceneWillStep(a3d::Scene& scene,
									                  const a3d::Scene::StepInfo& info) override;

	private:
		/// Private Member Variables ///

		std::unique_ptr<a3d::Window>				_window;
		a3d::Node*									_duckNode;
		std::vector<DuckFruitDef>					_duckFruit;
		std::unique_ptr<a3d::ext::WanderRotator>	_duckRotator;
		float										_cameraMoveSpeed;
		double										_duckFruitSpawnAccumulator;
		double										_slurmShotAccumulator;
	};
}

#endif // AVARA3D_TEST_RIGIDBODY_APP_H
