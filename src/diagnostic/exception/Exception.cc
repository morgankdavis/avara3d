//
//  Exception.cc
//	avara3d
//
//  Created by Morgan Davis on 12/24/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/exception/Exception.h"

#include "a3d/Utilities.h"
#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace a3d::utils;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Exception::Exception(const string& what):
	runtime_error(what) {

#ifdef POSIX
		A3D_LOG_E("Exception: {}\nStack trace:\n{}", what, StackTrace(1));
#else
		A3D_LOG_E("Exception: {}", what);
#endif
}

