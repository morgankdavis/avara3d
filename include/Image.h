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


#include <filesystem>
#include <memory>
#include <string>

//#include <boost/filesystem.hpp>

#include "MaterialPropertyContents.h"


namespace ae {
	
	
	class Buffer;


	class Image : public MaterialPropertyContents {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

#ifndef ANDROID
		Image(const std::filesystem::path& path,
			  bool flipVertical=true);
#endif
		// with header
		Image(std::shared_ptr<Buffer> headerBuffer,
			  bool flipVertical=true);
		// raw
		Image(std::shared_ptr<Buffer> rawBuffer,
			  unsigned width, unsigned height,
			  unsigned bytesPerPixel,
			  bool flipVertical = true);
		~Image();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		unsigned 						width() const;
		unsigned 						height() const;
		unsigned 						bytesPerPixel() const;
		bool 							writePNG(std::filesystem::path path) const;
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		std::shared_ptr<Buffer> 		data() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		//void 							loadFile(const std::filesystem::path& path, bool flipHorizontal);
		void 							loadBuffer(Buffer& buffer, bool flipVertical);
		void 							flipVertical(); // "flip"
		void 							flipHorizontal(); // "mirror"
		
		unsigned						_width;
		unsigned						_height;
		unsigned						_bytesPerPixel;
		std::shared_ptr<Buffer>		_data;
	};
}


#endif /* Image_h */
