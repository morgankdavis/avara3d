//
//  StdOutLogSink.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LOG_SINK_STDOUTLOGSINK_H
#define AVARA3D_LOG_SINK_STDOUTLOGSINK_H

#include <cstdio>
#include <string>

#include "a3d/log/sink/LogSink.h"

namespace a3d {

	class StdOutLogSink : public LogSink {

	public:
	/// Public Lifecycle Functions ///

		StdOutLogSink();
		~StdOutLogSink() override;

		StdOutLogSink(const StdOutLogSink&) = delete;
		FileLogSink& operator=(const StdOutLogSink&) = delete;

		StdOutLogSink(StdOutLogSink&&) = delete;
		StdOutLogSink& operator=(StdOutLogSink&&) = delete;

	/// Public Member Functions ///

		void 	flush() override;

	/// Public LogSink Member Functions ///

		void 	write(const std::string& output, Log::Level level) override;
	};
}

#endif //AVARA3D_LOG_SINK_STDOUTLOGSINK_H
