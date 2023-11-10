//
// Created by mkd on 11/9/23.
//

#ifndef AVARA_ENGINE_STDLOGGERSINK_H
#define AVARA_ENGINE_STDLOGGERSINK_H


#include "diagnostic/logging/Logger.h"
#include "diagnostic/logging/sinks/LoggerSink.h"


namespace ae {

	class STDLoggerSink : public LoggerSink {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		STDLoggerSink();
		~STDLoggerSink();

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


#endif //AVARA_ENGINE_STDLOGGERSINK_H
