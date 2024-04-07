//
//  Buffer.h
//	avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Buffer_h
#define Buffer_h


#include <filesystem>
#include <memory>
#include <vector>


namespace a3d {
	
	class Buffer {
		
	public:
		
/*********************************************************************************************
	 Lifecycle
 *********************************************************************************************/
		
		explicit Buffer(const std::filesystem::path& path);
		explicit Buffer(const std::vector<std::byte>& buf);
		Buffer(const std::byte* buf, std::size_t size);
		~Buffer();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::byte* 					data() const; // should be 'const byte' ?
		std::size_t 				size() const;

/*********************************************************************************************
	Operator Overloads
 *********************************************************************************************/

		std::byte*					operator*() const; // should be 'const byte' ?
		std::byte					operator[](std::size_t idx) const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::unique_ptr<std::byte*> _data;
		std::size_t 				_size;
	};
}


#endif /* Buffer_h */
