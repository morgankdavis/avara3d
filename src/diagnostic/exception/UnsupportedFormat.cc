//
// Created by mkd on 1/21/24.
//

#include "a3d/diagnostic/exception/UnsupportedFormat.h"


using namespace a3d;
using namespace std;


UnsupportedFormat::UnsupportedFormat(const string& what):
		Exception{what} {}
