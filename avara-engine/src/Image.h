//
//  Image.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Image_h
#define Image_h


#include <string>


namespace ae {


	class Image { // 8 bit RGBA
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Image(const std::string path);
		~Image();
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		bool load();

		unsigned width() const;
		unsigned height() const;
		unsigned char* data() const;
		std::string path() const;
		bool loaded() const;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		unsigned			m_width;
		unsigned			m_height;
		unsigned char*		m_data;
		std::string			m_path;
		bool				m_loaded;
	};
}


#endif /* Image_h */
