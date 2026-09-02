//
//  StdOutLogSink.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LOG_SINK_STDOUTLOGSINK_H
#define AVARA3D_LOG_SINK_STDOUTLOGSINK_H

#include <string>

#include "a3d/log/sink/LogSink.h"

namespace a3d::log {

/** @brief LogSink that writes ordinary messages to standard output and errors to standard error. */
class StdOutLogSink : public LogSink {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates a standard-stream log sink. */
    StdOutLogSink();

    StdOutLogSink(const StdOutLogSink&)            = delete;
    StdOutLogSink& operator=(const StdOutLogSink&) = delete;

    StdOutLogSink(StdOutLogSink&&)            = delete;
    StdOutLogSink& operator=(StdOutLogSink&&) = delete;

    ~StdOutLogSink() override;

    // [Public Member Functions]

    /** @brief Flushes buffered standard-stream output. */
    void flush() override;

    // [Public LogSink Member Functions]

    /** @brief Writes Error and Fatal output to standard error and lower severities to standard output. */
    void write(const std::string& output, Level level) override;
};

}

#endif // AVARA3D_LOG_SINK_STDOUTLOGSINK_H
