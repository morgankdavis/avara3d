//
//  Init.h
//	avara-engine
//
//  Created by Morgan Davis on 1/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Init_h
#define Init_h


#include <memory>

#include <spdlog/spdlog.h>
//#include <spdlog/fmt/ostr.h>


extern std::shared_ptr<spdlog::logger>		LOG;

namespace ae {
	

	/***************************************************************************************
	     MARK:   Internal
	 **************************************************************************************/
	
	int initLog();
	int initGLFW();
	int initGLEW();
	
//	// not marking extern creates seemingly unrelated duplicate symbol errors
//	extern std::shared_ptr<spdlog::logger>		g_logger;
}


#endif /* Init_h */
