//
//  Exception.cc
//	avara-engine
//
//  Created by Morgan Davis on 12/24/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "ae/diagnostic/exception/Exception.h"

#include "ae/Utilities.h"
#include "ae/diagnostic/logging/Logger.h"


using namespace ae;
using namespace ae::utils;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Exception::Exception(const string& what):
	runtime_error(what) {

#ifdef POSIX
		AE_LOG_E("Exception: {}\nStack trace:\n{}", what, StackTrace(1));
#else
		AE_LOG_E("Exception: {}", what);
#endif
}

