//
// Created by mkd on 11/9/23.
//

#ifndef AVARA_ENGINE_ANDROIDLOGGERSINK_H
#define AVARA_ENGINE_ANDROIDLOGGERSINK_H


#include "ae/diagnostic/logging/Logger.h"
#include "ae/diagnostic/logging/sinks/LoggerSink.h"


namespace ae {

	class AndroidLoggerSink : public LoggerSink {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		AndroidLoggerSink();
		~AndroidLoggerSink();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		void 	flush() override;

/**************************************************************************************
	Internal
 **************************************************************************************/

		void 	write(const char* message, const char* tag, LogLevel level);
	};
}


#endif //AVARA_ENGINE_ANDROIDLOGGERSINK_H
