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
		
		Buffer(const std::filesystem::path& path);
		Buffer(const std::byte* buf, std::size_t size);
		Buffer(const std::vector<std::byte>& buf);
		
		Buffer(const Buffer& other); // copy
		Buffer& operator=(const Buffer& other); // copy assignment
		Buffer(Buffer&& other) noexcept; // move
		Buffer& operator=(Buffer&& other) noexcept; // move assignment
		
		~Buffer();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		// TODO: switch to unique_ptr
		std::byte* 			data() const;
		std::size_t 				size() const;

/*********************************************************************************************
	Operator Overloads
 *********************************************************************************************/

		std::byte*			operator*() const;
		std::byte			operator[](std::size_t idx) const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::byte* 			_data; // TODO: make unique
		std::size_t 				_size;
	};
}


#endif /* Buffer_h */
