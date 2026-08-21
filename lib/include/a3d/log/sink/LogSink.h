//
//  LogSink.h
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LOG_SINK_LOGSINK_H
#define AVARA3D_LOG_SINK_LOGSINK_H

#include <string>

#include "a3d/log/Log.h"

namespace a3d {

    /** @brief Interface for destinations that receive formatted Log output. */
    class LogSink {

// [Public Lifecycle Functions]

    public:
        virtual ~LogSink() = 0;

// [Public Member Functions]

        /** @brief Writes already-formatted @p output associated with @p level. */
        virtual void write(const std::string& output, Log::Level level) = 0;

        /** @brief Flushes buffered sink output; the base implementation does nothing. */
        virtual void flush();
    };

}

#endif // AVARA3D_LOG_SINK_LOGSINK_H
