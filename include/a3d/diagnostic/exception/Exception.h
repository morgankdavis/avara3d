//
//  Exception.h
//	avara3d
//
//  Created by Morgan Davis on 12/24/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXCEPTION_H
#define AVARA3D_EXCEPTION_H


#include <stdexcept>
#include <string>


namespace a3d {
	
	class Exception : public std::runtime_error {

	public:

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		explicit Exception(const std::string& what);
	};
}


#endif /* AVARA3D_EXCEPTION_H */
