//
//  Exception.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/24/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Exception.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Exception::Exception(const string& what):
	runtime_error(what) {
	
}

