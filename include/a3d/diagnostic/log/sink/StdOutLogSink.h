//
//  StdOutLogSink.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_STDOUTLOGSINK_H
#define AVARA3D_STDOUTLOGSINK_H

#include <cstdio>
#include <string>

#include "a3d/Types.h"
#include "a3d/diagnostic/log/sink/LogSink.h"

namespace a3d {

	class StdOutLogSink : public LogSink {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		StdOutLogSink();
		~StdOutLogSink() override;

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		void 	flush() override;

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		void 	write(const std::string& output, LogLevel level);
	};
}

#endif //AVARA3D_STDOUTLOGSINK_H
