//
//  Global.h
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Global_h
#define Global_h


#define MAX_DYNAMIC_LIGHTS		8

#define LOG_MAIN_FILE_NAME		"ae"
#define LOG_QUEUE_SIZE	    	12
#define LOG_FILE_SIZE 			1024 * 1024 * 5
#define LOG_FILE_ROTATIONS		3
#define LOG_ENABLE_NATIVE		true
#define LOG_START_LEVEL			LOG_LEVEL::DEBUG_
#define LOG_FLUSH_LEVEL			LOG_START_LEVEL

#define MAX_PATH_LEN  			1024


#ifdef ANDROID
struct android_app;
#endif


namespace ae {
	
#ifdef ANDROID
	void AE_INIT(android_app* app);
#else
	void AE_INIT();
#endif
}


#endif /* Global_h */
