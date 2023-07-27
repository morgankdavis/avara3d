//
//  Font.h
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Font_h
#define Font_h


#include <filesystem>
#include <memory>
#include <optional>

//#include <boost/optional.hpp>
//#include <boost/filesystem.hpp>

#include "Types.h"


namespace ae {
	
	
	class Buffer;
	
	
	class Font : public std::enable_shared_from_this<Font> {
		
	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Font(std::filesystem::path& path);
		Font(std::shared_ptr<Buffer> buffer);
		
		~Font();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::optional<std::string>	name() const;
		FONT_TYPE 						type() const;
		std::shared_ptr<Buffer> 		buffer() const;
		
	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/
		
		std::optional<std::string>	_name;
		FONT_TYPE						_type;
		std::shared_ptr<Buffer>			_buffer;
	};
}


#endif /* Font_h */
