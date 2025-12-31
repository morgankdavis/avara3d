//
//  Exception.cc
//  avara3d
//
//  Created by Morgan Davis on 12/24/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/exception/Exception.h"

#include "a3d/Utilities.h"
#include "a3d/diagnostic/log/Log.h"

using namespace a3d;
using namespace std;

/// Internal Lifecycle Functions ///

Exception::Exception(const string& what):
	runtime_error{what} {

#ifdef A3D_POSIX
		log::e()("Exception: {}\nStack trace:\n{}", what, utils::StackTrace(1));
#else
		log::e()("Exception: {}", what);
#endif
}

