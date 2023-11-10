//
// Created by mkd on 11/9/23.
//

#ifndef AVARA_ENGINE_ANDROIDLOGGERSINK_H
#define AVARA_ENGINE_ANDROIDLOGGERSINK_H


#include "diagnostic/logging/Logger.h"
#include "diagnostic/logging/sinks/LoggerSink.h"


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

		void 	write(const char* message, const char* tag, LOG_LEVEL level);
	};
}


#endif //AVARA_ENGINE_ANDROIDLOGGERSINK_H
