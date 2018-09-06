//
//  Buffer.h
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Buffer_h
#define Buffer_h


#include <vector>

#include <boost/filesystem.hpp>


namespace ae {
	
	
	class Buffer {
		
	public:
		
		/***************************************************************************************
		   	 Lifecycle
		 ***************************************************************************************/
		
		Buffer(boost::filesystem::path& path);
		Buffer(const unsigned char* buf, unsigned len);
		Buffer(const std::vector<unsigned char>& buf);
		
		Buffer(const Buffer& other); // copy constructor
		Buffer& operator=(const Buffer& other); // copy assignment
		
		~Buffer();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		unsigned char* pointer() const;
		unsigned length() const;
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		unsigned char* 		m_pointer;
		unsigned 			m_length;
	};
}


#endif /* Buffer_h */
