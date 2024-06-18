//
//  NoAvailableMiceException.cc
//  avara3d
//
//  Created by Morgan Davis on 1/21/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/exception/NoAvailableMiceException.h"


using namespace a3d;
using namespace std;



/*********************************************************************************************
    Internal Lifecycle
 *********************************************************************************************/

NoAvailableMiceException::NoAvailableMiceException(const string& what):
        Exception{what} {}


