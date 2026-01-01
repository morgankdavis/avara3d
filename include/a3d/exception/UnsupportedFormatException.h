//
//  UnsupportedFormatException.h
//  avara3d
//
//  Created by Morgan Davis on 1/21/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UNSUPPORTEDFORMATEXCEPTION_H
#define AVARA3D_UNSUPPORTEDFORMATEXCEPTION_H

#include <string>

#include "a3d/exception/Exception.h"

namespace a3d {

	class UnsupportedFormatException : public Exception {

	public:
		/// Internal Lifecycle Functions ///

		explicit UnsupportedFormatException(const std::string& what);

		// add format str?
	};
}

#endif //AVARA3D_UNSUPPORTEDFORMATEXCEPTION_H
