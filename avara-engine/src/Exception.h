//
//  Exception.h
//	avara-engine
//
//  Created by Morgan Davis on 12/24/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Exception_h
#define Exception_h


#include <stdexcept>
#include <string>


namespace ae {
	
	class Exception : public std::runtime_error {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Exception(const std::string& what);
	};
}

#endif /* Exception_h */
