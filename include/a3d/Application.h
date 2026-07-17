//
//  Application.h
//  avara3d
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_APPLICATION_H
#define AVARA3D_APPLICATION_H

#include <memory>
#include <vector>

namespace a3d {

	class PhysicalWorld;
	class Runner;
	class Scene;
	class VisualWorld;

	class Application {

	public:
		/// Public Static Member Functions ///

		static int Run(std::unique_ptr<Application> application);

		/// Public Lifecycle Functions ///

		Application(int argc, char* argv[]);
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;

	protected:
		/// Protected Lifecycle Functions ///

		Application();

		/// Protected Member Functions ///

		virtual std::unique_ptr<Scene> initialize() = 0;

		virtual bool shouldContinue(const Scene& scene);
		virtual void didShutdown();

		const std::vector<std::string>& args() const;

		/// Scene Callback Templates ///

		virtual void sceneUpdate(Scene& scene, double time, double deltaTime);

		/// VisualWorld Callback Templates ///

		virtual void visualWorldWillRender(VisualWorld& world, double time, double deltaTime);

		virtual void visualWorldDidRender(VisualWorld& world, double time, double deltaTime);

		/// PhysicalWorld Callback Templates ///

		virtual void physicalWorldDidSimulate(PhysicalWorld& world, double time, double deltaTime);

	private:
		/// Private Member Functions ///

		void prepare();
		bool update();
		void shutdown() noexcept;

		void registerCallbacks();

		/// Private Member Variables ///

		std::vector<std::string>	_args;
		std::unique_ptr<Scene>		_scene;
		std::unique_ptr<Runner>		_runner; // Runner must be destroyed before Scene
		bool						_didShutdown;;
	};
}

#endif //AVARA3D_APPLICATION_H
