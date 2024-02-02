//
// Created by mkd on 1/21/24.
//

#include "ae/diagnostic/exceptions/UnsupportedFormat.h"


using namespace ae;
using namespace std;


UnsupportedFormat::UnsupportedFormat(const string& what):
		Exception(what) {}
