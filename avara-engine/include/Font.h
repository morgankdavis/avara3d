//
//  Font.h
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Font_h
#define Font_h


#include <memory>

#include <boost/filesystem.hpp>


namespace ae {
	
	
	class Buffer;
	
	
	class Font {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Font(boost::filesystem::path path);
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		std::string name() const;
//		FONT_TYPE type() const;
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		std::string			m_name;
//		FONT_TYPE			m_type;
	};
}


#endif /* Font_h */
