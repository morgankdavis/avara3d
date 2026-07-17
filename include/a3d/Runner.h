//
//  Runner.h
//  avara3d
//
//  Created by Morgan Davis on 7/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RUNNER_H
#define AVARA3D_RUNNER_H

namespace a3d {

	class Scene;

	class Runner {

	public:
		/// Public Types ///

		enum class State {
			Idle,
			Running,
			Stopped
		};

		/// Public Lifecycle Functions ///

		explicit Runner(Scene& scene);
		~Runner();

		Runner(const Runner&) = delete;
		Runner& operator=(const Runner&) = delete;

		Runner(Runner&&) = delete;
		Runner& operator=(Runner&&) = delete;

		/// Public Member Functions ///

		void start();
		bool update();
		void stop();

		State state() const;

		Scene& scene();
		const Scene& scene() const;

	private:
		/// Private Member Variables ///

		Scene&	_scene;
		State	_state;
	};
}

#endif //AVARA3D_RUNNER_H


// //
// //  Runner.h
// //  avara3d
// //
// //  Created by Morgan Davis on 7/10/26.
// //  Copyright © 2026 Morgan K Davis. All rights reserved.
// //
//
// #ifndef AVARA3D_RUNNER_H
// #define AVARA3D_RUNNER_H
//
// #include <functional>
// #include <memory>
//
// namespace a3d {
//
// 	class Scene;
//
// 	class Runner {
//
// 	public:
// 		/// Public Types ///
//
// 		enum class State {
// 			Idle,
// 			Running,
// 			Stopping,
// 			Stopped
// 		};
//
// 		using ShouldContinuePredicate =	std::function<bool(
// 				Runner& runner,
// 				Scene& scene,
// 				void* context)>;
//
// 		using DidShutdownCallback =	std::function<void(
// 				Runner& runner,
// 				void* context)>;
//
// 		/// Public Static Member Functions ///
//
// 		// ! desktop implementation blocks.
// 		// ! web implementation installs a browser callback and returns.
// 		static int Run(Runner&& runner);
//
// 		/// Public Lifecycle Functions ///
//
// 		explicit Runner(std::unique_ptr<Scene> scene);
// 		~Runner();
//
// 		Runner(Runner&& other);
// 		Runner& operator=(Runner&& other);
//
// 		Runner(const Runner&) = delete;
// 		Runner& operator=(const Runner&) = delete;
//
// 		/// Public Member Functions ///
//
// 		void						start();
// 		bool						update();
// 		void						stop();
//
// 		State						state() const;
//
// 		const Scene&				scene() const;
//
// 		void*						context() const;
// 		void						context(void* context);
//
// 		ShouldContinuePredicate		shouldContinuePredicate() const;
// 		void						shouldContinuePredicate(ShouldContinuePredicate function);
//
// 		DidShutdownCallback			didShutdownCallback() const;
// 		void						didShutdownCallback(DidShutdownCallback function);
//
// 	private:
// 		/// Private Member Functions ///
//
// 		void						end();
//
// 		/// Private Member Variables ///
//
// 		std::unique_ptr<Scene>		_scene;
// 		void*						_context;
// 		State						_state;
// 		ShouldContinuePredicate		_shouldContinuePredicate;
// 		DidShutdownCallback			_didShutdownCallback;
// 	};
// }
//
// #endif // AVARA3D_RUNNER_H
