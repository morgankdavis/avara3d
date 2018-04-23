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

#include <boost/filesystem.hpp>


namespace ae {


	class Image {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Image(const boost::filesystem::path& path, bool flipHorizontal=true);
		Image(unsigned char* data, unsigned width, unsigned height, bool flipHorizontal=true);
		~Image();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		unsigned width() const;
		unsigned height() const;
		bool writePNG(boost::filesystem::path path) const;
		
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		unsigned char* data() const;
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		void loadFile(boost::filesystem::path path, bool flipHorizontal);
		void flip();
		
		unsigned			m_width;
		unsigned			m_height;
		unsigned char*		m_data;
	};
}


#endif /* Image_h */
