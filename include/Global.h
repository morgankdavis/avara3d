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
	
	constexpr unsigned 		MAX_PATH_LEN =				2048;
	

#ifdef ANDROID
		void AE_INIT(android_app* app);
#else
		void AE_INIT();
#endif
	
}


#endif /* Global_h */
