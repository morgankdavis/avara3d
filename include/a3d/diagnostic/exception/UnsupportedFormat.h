//
// Created by mkd on 1/21/24.
//

#ifndef AVARA3D_UNSUPPORTEDFORMAT_H
#define AVARA3D_UNSUPPORTEDFORMAT_H


#include <string>

#include "a3d/diagnostic/exception/Exception.h"


namespace a3d {

	class UnsupportedFormat : public Exception {

	public:

		UnsupportedFormat(const std::string& what);
	};
}


#endif //AVARA3D_UNSUPPORTEDFORMAT_H
