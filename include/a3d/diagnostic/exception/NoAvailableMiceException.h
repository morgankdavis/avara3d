//
//  NoAvailableMiceException.h
//  avara3d
//
//  Created by Morgan Davis on 6/16/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef NOAVAILABLEMICEEXCEPTION_H
#define NOAVAILABLEMICEEXCEPTION_H


#include <string>

#include "a3d/diagnostic/exception/Exception.h"


namespace a3d {

    class NoAvailableMiceException : public Exception {

        /*********************************************************************************************
            Internal Lifecycle
         *********************************************************************************************/

    public:

        explicit NoAvailableMiceException(const std::string& what);
    };
}


#endif /* NOAVAILABLEMICEEXCEPTION_H */


