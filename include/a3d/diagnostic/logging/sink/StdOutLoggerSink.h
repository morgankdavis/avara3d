//
//  StdOutLoggerSink.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_STDOUTLOGGERSINK_H
#define AVARA3D_STDOUTLOGGERSINK_H


#include <cstdio>

#include "a3d/Types.h"
#include "a3d/diagnostic/logging/sink/LoggerSink.h"


namespace a3d {

	class StdOutLoggerSink : public LoggerSink {

/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

	public:

		StdOutLoggerSink();
		~StdOutLoggerSink() override;

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

		void 	flush() override;

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

		void 	write(const char* message, LogLevel level);
	};
}


#endif //AVARA3D_STDOUTLOGGERSINK_H
