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
#include <string>

#include "a3d/Types.h"
#include "a3d/diagnostic/logging/sink/LoggerSink.h"


namespace a3d {

	class StdOutLoggerSink : public LoggerSink {

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static bool classof(const LoggerSink* o);
		static bool classof(const LoggerSink& o);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		StdOutLoggerSink();
		~StdOutLoggerSink() override;

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


#endif //AVARA3D_STDOUTLOGGERSINK_H
