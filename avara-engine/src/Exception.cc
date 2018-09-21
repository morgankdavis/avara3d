//
//  Exception.cc
//	avara-engine
//
//  Created by Morgan Davis on 12/24/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Exception.h"

#include "Logger.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

Exception::Exception(const string& what):
	runtime_error(what) {

#if defined(MACOS) || defined(LINUX)
		AE_LOG_E("Exception: {}\nStack trace:\n{}", what, StackTrace(1));
#else
		AE_LOG_E("Exception: {}", what);
#endif
}

