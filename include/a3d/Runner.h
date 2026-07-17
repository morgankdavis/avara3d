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
