//
//  Exception.h
//	avara-engine
//
//  Created by Morgan Davis on 12/24/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Exception_h
#define Exception_h


#include <exception>
#include <string>


//logic_error
//
//		invalid_argument
//		domain_error
//		length_error
//		out_of_range
//		future_error(C++11)
//		bad_optional_access(C++17)
//runtime_error
//		range_error
//		overflow_error
//		underflow_error
//		regex_error(C++11)
//		tx_exception(TM TS)
//		system_error(C++11)
//			ios_base::failure(C++11)
//			filesystem::filesystem_error(C++17)
//bad_typeid
//		bad_cast
//bad_any_cast(C++17)
//bad_weak_ptr(C++11)
//bad_function_call(C++11)
//bad_alloc
//		bad_array_new_length(C++11)
//bad_exception
//ios_base::failure(until C++11)
//bad_variant_access(C++17)


namespace ae {
	
	class Exception : public std::exception {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Exception();
		Exception(const std::string what);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		// superclass:
		// virtual const char* what() const;
		//const char* what() const;
		//void what(const std::string what);

	private:

		//std::string		m_what;
	};
}

#endif /* Exception_h */
