//
//  App.h
//  002-import
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TEST_IMPORT_APP_H
#define AVARA3D_TEST_IMPORT_APP_H

#include <memory>

#include "a3d/Application.h"

namespace a3d {
	class InputContext;
	class Node;
	class Scene;
	class VisualWorld;
	class Window;
}

namespace test::import {

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

		std::unique_ptr<a3d::Scene>		init() override;
		bool							shouldContinue(const a3d::Scene& scene) override;
		void							didShutdown() override;

		/// Input Context Callbacks ///

		void		inputContextDidUpdate(a3d::InputContext &inputContext,
				                          const a3d::InputContext::UpdateInfo &info) override;

		/// Visual World Callbacks ///

		void		visualWorldWillRender(a3d::VisualWorld &visualWorld,
				                          const a3d::VisualWorld::RenderInfo& info) override;
		void		visualWorldDidRender(a3d::VisualWorld &visualWorld,
				                         const a3d::VisualWorld::RenderInfo &info) override;

	private:
		/// Private Member Variables ///

		std::unique_ptr<a3d::Window>	_window;
		a3d::Node*						_importMeshRoot;
	};
}

#endif // AVARA3D_TEST_IMPORT_APP_H
