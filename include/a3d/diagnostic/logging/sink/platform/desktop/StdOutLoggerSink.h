//
// Created by mkd on 11/9/23.
//

#ifndef AVARA3D_STDOUTLOGGERSINK_H
#define AVARA3D_STDOUTLOGGERSINK_H


#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/diagnostic/logging/sink/LoggerSink.h"


namespace a3d {

	class StdOutLoggerSink : public LoggerSink {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		StdOutLoggerSink();
		~StdOutLoggerSink();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		void 	flush() override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 	write(const char* message, LogLevel level);
	};
}


#endif //AVARA3D_STDOUTLOGGERSINK_H
