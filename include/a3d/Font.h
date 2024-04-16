//
//  Font.h
//	avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_FONT_H
#define AVARA3D_FONT_H


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

		explicit Font(const std::filesystem::path& path);
		explicit Font(std::unique_ptr<Buffer> buffer);
		
		~Font();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		const std::optional<std::string>&	name() const;
		FontType 							type() const;
		const Buffer* 						buffer() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>			_name;
		FontType							_type;
		std::unique_ptr<Buffer>				_buffer;
	};
}


#endif /* AVARA3D_FONT_H */
