//
//  Buffer.h
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Buffer_h
#define Buffer_h


#include <filesystem>
#include <memory>
#include <vector>


namespace ae {
	
	class Buffer {
		
	public:
		
/*********************************************************************************************
	 Lifecycle
 *********************************************************************************************/
		
		Buffer(const std::filesystem::path& path);
		Buffer(const unsigned char* buf, std::size_t size);
		Buffer(const std::vector<unsigned char>& buf);
		
		Buffer(const Buffer& other); // copy constructor
		Buffer& operator=(const Buffer& other); // copy assignment
		
		~Buffer();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		unsigned char* 			pointer() const;
		unsigned 				size() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		unsigned char* 			_pointer;
		unsigned 				_size;
	};
}


#endif /* Buffer_h */
