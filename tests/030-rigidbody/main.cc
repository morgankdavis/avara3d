//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <string>
#include <utility>
#include <vector>

#include "a3d/a3d.h"

#include "App.h"

using namespace a3d;
using namespace std;

using test::rigidbody::App;

int main(int argc, char* argv[]) {

	struct Context {
		App app;
	};

	auto* context = new Context{
		App(vector<string>(argv + 1, argv + argc))
	};

	Runner runner(context->app.initialize());

	runner.context(context);

	runner.shouldContinuePredicate([](
			Runner& runner,
			Scene& scene,
			void* context) {
			auto *c = static_cast<Context *>(context);
			return c->app.runnerShouldContinue(scene);
	});

	runner.didShutdownCallback([](
			Runner& runner,
			void* context) {
		auto *c = static_cast<Context *>(context);
		c->app.runnerDidShutdown();
		delete c;
	});

	return Runner::Run(std::move(runner));
}
