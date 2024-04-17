//
//  Image.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_IMAGE_H
#define AVARA3D_IMAGE_H


#include <filesystem>
#include <memory>
#include <string>

#include "a3d/rendering/material/Sampleable.h"


namespace a3d {
	
	
	class Buffer;


	class Image : public Sampleable {

/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

		explicit Image(const std::filesystem::path& path,
			  bool flipVertical = true,
			  bool flipHorizontal = false);
		explicit Image(std::unique_ptr<Buffer> headerBuffer, // with header
			  bool flipVertical = true,
			  bool flipHorizontal = false);
		Image(std::unique_ptr<Buffer> rawBuffer, // raw
			  unsigned width,
			  unsigned height,
			  unsigned bytesPerPixel,
			  bool flipVertical = true,
			  bool flipHorizontal = false);
		~Image() override;

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

		unsigned 					width() const;
		unsigned 					height() const;
		unsigned 					bytesPerPixel() const;
		std::unique_ptr<Image> 		inverted() const;
		bool 						writePNG(std::filesystem::path path) const;
		
/*********************************************************************************************
	Internal Members
 *********************************************************************************************/
		
		const Buffer& 				buffer() const;

/*********************************************************************************************
	Private Members
 *********************************************************************************************/

	private:

		void 						loadBuffer(Buffer& buffer,
											   bool flipVertical,
											   bool flipHorizontal);
		void 						flipVertical(); // "flip"
		void 						flipHorizontal(); // "mirror"

/*********************************************************************************************
	Private IVars
 *********************************************************************************************/

		unsigned					_width;
		unsigned					_height;
		unsigned					_bytesPerPixel;
		std::unique_ptr<Buffer>		_buffer;
	};
}


#endif /* AVARA3D_IMAGE_H */
