//
//  Image.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//
//  Pads to 8 bit RGBA
//  Would like to handle 1, 3 and 4 component pixel formats, but that seemes harder than expected...
//

#ifndef Image_h
#define Image_h


#include <memory>
#include <string>

#include <boost/filesystem.hpp>

#include "MaterialPropertyContents.h"


namespace ae {
	
	
	class Buffer;


	class Image : public MaterialPropertyContents {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

#ifndef ANDROID
		Image(const boost::filesystem::path& path, bool flipHorizontal=true);
#endif
		Image(std::shared_ptr<Buffer> buffer, bool flipHorizontal=true);

		~Image();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		unsigned width() const;
		unsigned height() const;
		unsigned bytesPerPixel() const;
		bool writePNG(boost::filesystem::path path) const;
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		std::shared_ptr<Buffer> data() const;
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		//void loadFile(const boost::filesystem::path& path, bool flipHorizontal);
		void loadBuffer(Buffer& buffer, bool flipHorizontal);
		void flipHorizontal();
		
		unsigned						m_width;
		unsigned						m_height;
		unsigned 						m_bytesPerPixel;
		std::shared_ptr<Buffer>		m_data;
	};
}


#endif /* Image_h */
