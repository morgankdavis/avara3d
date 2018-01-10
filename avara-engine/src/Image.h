//
//  Image.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//
//  8 bit RGBA
//

#ifndef Image_h
#define Image_h


#include <string>


namespace ae {


	class Image {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Image(const std::string path, bool flipHorizontal=true);
		Image(unsigned char* data, unsigned width, unsigned height, bool flipHorizontal=true);
		~Image();
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		unsigned width() const;
		unsigned height() const;
		bool writePNG(std::string path) const;
		
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		unsigned char* data() const;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		void loadFile(std::string path, bool flipHorizontal);
		void flip();
		
		unsigned			m_width;
		unsigned			m_height;
		unsigned char*		m_data;
	};
}


#endif /* Image_h */
