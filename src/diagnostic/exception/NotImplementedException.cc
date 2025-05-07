//
//  NotImplementedException.cc
//  avara3d
//
//  Created by Morgan Davis on 5/6/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/exception/NotImplementedException.h"


using namespace a3d;
using namespace std;



/*********************************************************************************************
	Internal Lifecycle Functions
 *********************************************************************************************/

NotImplementedException::NotImplementedException(const string& what):
		Exception{what} {}
