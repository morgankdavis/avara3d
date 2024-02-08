//
// Created by mkd on 1/21/24.
//

#ifndef AVARA_ENGINE_UNSUPPORTEDFORMAT_H
#define AVARA_ENGINE_UNSUPPORTEDFORMAT_H


#include <string>

#include "ae/diagnostic/exception/Exception.h"


namespace ae {

	class UnsupportedFormat : public Exception {

	public:

		UnsupportedFormat(const std::string& what);
	};
}


#endif //AVARA_ENGINE_UNSUPPORTEDFORMAT_H
