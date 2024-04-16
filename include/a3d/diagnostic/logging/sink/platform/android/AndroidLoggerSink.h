//
//  AndroidLoggerSink.h
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_ANDROIDLOGGERSINK_H
#define AVARA3D_ANDROIDLOGGERSINK_H


#include <cstdio>

#include "a3d/diagnostic/logging/sinks/LoggerSink.h"


namespace a3d {

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


#endif //AVARA3D_ANDROIDLOGGERSINK_H
