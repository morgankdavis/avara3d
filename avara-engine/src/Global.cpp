//
//  Global.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Global.h"

#ifdef ANDROID
#include <NDKHelper.h>
#endif

#include "Logger.h"


using namespace ae;


#ifdef ANDROID
void ae::AE_INIT(android_app* app) {
#else
	void ae::AE_INIT() {
#endif
#ifdef ANDROID
	ndk_helper::JNIHelper::Init(app->activity, "com/mkdinteractive/helper/NDKHelper");
#endif
	Logger::Init();
	Logger::Level(LOG_START_LEVEL);
}
