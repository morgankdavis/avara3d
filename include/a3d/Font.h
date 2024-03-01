//
//  Font.h
//	avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Font_h
#define Font_h


#include <filesystem>
#include <memory>
#include <optional>

#include "a3d/Types.h"


namespace a3d {
	
	
	class Buffer;
	
	
	class Font {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Font(std::filesystem::path& path);
		Font(std::shared_ptr<Buffer> buffer);
		
		~Font();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::optional<std::string>		name() const;
		FontType 						type() const;
		std::shared_ptr<Buffer> 		buffer() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>		_name;
		FontType						_type;
		std::shared_ptr<Buffer>			_buffer;
	};
}


#endif /* Font_h */
