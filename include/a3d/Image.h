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
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit Image(const std::filesystem::path& path,
					   bool flipVertical = true,
					   bool flipHorizontal = false);
		explicit Image(std::unique_ptr<Buffer> bufer,
					   bool flipVertical = true,
					   bool flipHorizontal = false);
		Image(std::unique_ptr<Buffer> buffer,
			  unsigned width,
			  unsigned height,
			  unsigned bytesPerPixel,
			  bool flipVertical = true,
			  bool flipHorizontal = false);
		~Image() override;

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		unsigned 					width() const;
		unsigned 					height() const;
		unsigned 					bytesPerPixel() const;
		std::unique_ptr<Image> 		inverted() const;
		const Buffer& 				buffer() const;
		bool 						writePNG(const std::filesystem::path& path) const;

/*********************************************************************************************
	Private Member Functions
 *********************************************************************************************/

	private:

		void 						loadBuffer(Buffer& buffer,
											   bool flipVertical,
											   bool flipHorizontal);
		void 						flipVertical(); // "flip"
		void 						flipHorizontal(); // "mirror"

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		unsigned					_width;
		unsigned					_height;
		unsigned					_bytesPerPixel;
		std::unique_ptr<Buffer>		_buffer;
	};
}


#endif /* AVARA3D_IMAGE_H */
