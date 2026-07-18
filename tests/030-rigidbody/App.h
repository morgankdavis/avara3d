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
#include <vector>

#include "a3d/Application.h"

namespace a3d {
	class GLFWWindow;
	class Mesh;
	class Node;
	class PhysicsShape;
	class PhysicalWorld;
	class Scene;
	class VisualWorld;
}

namespace test::rigidbody {

	struct WanderRotator;

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

		std::unique_ptr<a3d::Scene> init() override;

		bool shouldContinue(
			const a3d::Scene& scene) override;

		void didShutdown() override;

		/// Scene Callback Overrides ///

		void sceneUpdate(
			a3d::Scene& scene,
			double time,
			double deltaTime) override;

		/// VisualWorld Callback Overrides ///

		void visualWorldDidRender(
			a3d::VisualWorld &world,
			double time,
			double deltaTime) override;

		void visualWorldWillRender(
			a3d::VisualWorld& world,
			double time,
			double deltaTime) override;

		/// PhysicalWorld Callback Overrides ///

		void physicalWorldDidSimulate(
			a3d::PhysicalWorld& world,
			double time,
			double deltaTime) override;

	private:
		/// Private Member Variables ///

		std::unique_ptr<a3d::GLFWWindow>	_window;
		a3d::Node*							_palmNode;
		a3d::Node*							_duckNode;
		std::vector<DuckFruitDef>			_duckFruit;
		std::unique_ptr<WanderRotator>		_duckRotator;
	};
}

#endif // AVARA3D_TEST_RIGIDBODY_APP_H
