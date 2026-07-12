//
//  App.h
//  avara3d
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TEST_RIGIDBODY_APP_H
#define AVARA3D_TEST_RIGIDBODY_APP_H

#include <memory>
#include <string>
#include <tuple>
#include <vector>

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

	struct WanderRotate;

	class App {

	public:

		using DuckFruitDef = std::tuple<
			std::shared_ptr<a3d::Mesh>,
			std::shared_ptr<a3d::PhysicsShape>,
			float>;


		App(std::vector<std::string> args);

		~App();

		App(const App &) = delete;

		App &operator=(const App &) = delete;

		App(App &&) = delete;

		App &operator=(App &&) = delete;

		std::unique_ptr<a3d::Scene> initialize();

		bool runnerShouldContinue(const a3d::Scene &scene);
		void runnerDidShutdown();

	private:

		void sceneUpdate(a3d::Scene &scene, double time, double deltaTime);
		void visualWorldWillRender(a3d::VisualWorld &world, double time, double deltaTime);
		void visualWorldDidRender(a3d::VisualWorld &world, double time, double deltaTime);
		void physicalWorldDidSimulate(a3d::PhysicalWorld &world, double time, double deltaTime);

		a3d::Node *g_palmNode;
		a3d::Node *g_duckNode;

		std::unique_ptr<a3d::GLFWWindow> _window;

		std::vector<DuckFruitDef> g_duckFruit;


		WanderRotate *wr;
	};
}

#endif //AVARA3D_TEST_RIGIDBODY_APP_H
