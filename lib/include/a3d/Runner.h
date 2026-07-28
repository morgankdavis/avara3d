//
//  Runner.h
//  avara3d
//
//  Created by Morgan Davis on 7/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RUNNER_H
#define AVARA3D_RUNNER_H

#include <chrono>

#include "a3d/Execution.h"

#include "a3d/TestAccessFwd.h"

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

		Runner(const Runner&) = delete;
		Runner& operator=(const Runner&) = delete;

		Runner(Runner&&) = delete;
		Runner& operator=(Runner&&) = delete;

		~Runner();

		/// Public Member Functions ///

		void start();
		bool update();
		void stop();

		State state() const;

		Scene& scene();
		const Scene& scene() const;

	private:
		/// Private Types ///

		using Clock = std::chrono::steady_clock;

		/// Private Member Functions ///

		void start(Clock::time_point now);
		bool update(Clock::time_point now);

		/// Private Member Variables ///

		Scene&				_scene;
		State				_state;
		Clock::time_point	_startTime;
		Clock::time_point	_previousUpdateTime;
		HostUpdateInfo		_hostUpdateInfo;
		bool				_hasUpdated;

		/// Test Access ///

		friend class testing::RunnerTestAccess;
	};
}

#endif //AVARA3D_RUNNER_H
