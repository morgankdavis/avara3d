//
//  App.h
//  000-quadrotor
//
//  Created by Morgan Davis on 7/19/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DEMO_QUADROTOR_APP_H
#define AVARA3D_DEMO_QUADROTOR_APP_H

#include <memory>

#include "a3d/Application.h"

namespace a3d {
	class Node;
	class PhysicsWorld;
	class Scene;
	class VisualWorld;
	class Window;
}

namespace test::quadrotor {

	class App : public a3d::Application {

	public:
		/// Public Lifecycle Functions ///

		App(int argc, char* argv[]);
		~App() override;

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		App(App&&) = delete;
		App& operator=(App&&) = delete;

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

		/// PhysicsWorld Callback Overrides ///

		void physicalWorldDidSimulate(
			a3d::PhysicsWorld& world,
			double time,
			double deltaTime) override;

	private:
		/// Private Member Variables ///

		std::unique_ptr<a3d::Window>	_window;
		std::shared_ptr<a3d::Node>		_bananaNode;
	};
}

#endif // AVARA3D_DEMO_QUADROTOR_APP_H
