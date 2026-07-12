//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <vector>

#include "a3d/a3d.h"

#include "App.h"

int main(int argc, const char* argv[]) {

	struct Context {
		test::rigidbody::App app;
	};

	auto* context = new Context{
		test::rigidbody::App(std::vector<std::string>(argv + 1, argv + argc))
	};

	a3d::Runner runner(context->app.init());

	runner.context(context);

	runner.continueCallback([](
			a3d::Runner& runner,
			a3d::Scene& scene,
			void* context) {

			auto *c = static_cast<Context *>(context);
			return c->app.shouldContinue(scene);
	});

	runner.shutdownCallback([](
			a3d::Runner& runner,
			void* context) {

		a3d::log::d();

		auto *c = static_cast<Context *>(context);
		c->app.shutdown();

		delete c;
	});

	return a3d::Runner::Run(std::move(runner));
}
