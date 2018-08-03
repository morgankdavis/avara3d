//
//  Global.h
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Global_h
#define Global_h


#include "Types.h"


#ifdef ANDROID
struct android_app;
#endif


namespace ae {
	
	constexpr unsigned 		MAX_DYNAMIC_LIGHTS =		8;
	
	constexpr char 			LOG_MAIN_FILE_NAME[] =		"ae";
	constexpr unsigned 		LOG_QUEUE_SIZE =			12;
	constexpr unsigned 		LOG_FILE_SIZE =				1024 * 1024 * 5;
	constexpr unsigned 		LOG_FILE_ROTATIONS =		3;
	constexpr bool 			LOG_ENABLE_NATIVE =			true;
	constexpr LOG_LEVEL 	LOG_START_LEVEL =			LOG_LEVEL::DEBUG_;
	constexpr LOG_LEVEL 	LOG_FLUSH_LEVEL =			LOG_START_LEVEL;
	
	constexpr unsigned 		MAX_PATH_LEN =				1024;
	
	
//#ifdef ANDROID
//	struct android_app;
//#endif
	
#ifdef ANDROID
		void AE_INIT(android_app* app);
#else
		void AE_INIT();
#endif
	
}


#endif /* Global_h */
