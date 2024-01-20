//
//  Global.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "ae/Global.h"

#ifdef ANDROID
#include <NDKHelper.h>
#endif


using namespace ae;


#ifdef ANDROID
void ae::AE_INIT(android_app* app) {
#else
	void ae::AE_INIT() {
#endif
#ifdef ANDROID
	ndk_helper::JNIHelper::Init(app->activity, "com/mkdinteractive/helper/NDKHelper");
#endif
}
