//
//  NotImplementedException.h
//  avara3d
//
//  Created by Morgan Davis on 5/6/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_NOTIMPLEMENTEDEXCEPTION_H
#define AVARA3D_NOTIMPLEMENTEDEXCEPTION_H


#include <string>

#include "a3d/diagnostic/exception/Exception.h"


namespace a3d {

	class NotImplementedException : public Exception {

/*********************************************************************************************
	Internal Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit NotImplementedException(const std::string& what);

		// add format str?
	};
}


#endif //AVARA3D_NOTIMPLEMENTEDEXCEPTION_H
