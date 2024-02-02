//
// Created by mkd on 11/9/23.
//

#ifndef AVARA_ENGINE_STDOUTLOGGERSINK_H
#define AVARA_ENGINE_STDOUTLOGGERSINK_H


#include "ae/diagnostic/logging/Logger.h"
#include "ae/diagnostic/logging/sinks/LoggerSink.h"


namespace ae {

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

		void 	write(const char* message, LOG_LEVEL level);
	};
}


#endif //AVARA_ENGINE_STDOUTLOGGERSINK_H
