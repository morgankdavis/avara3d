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
	Lifecycle
 *********************************************************************************************/

	public:

#ifndef ANDROID
		Image(const std::filesystem::path& path,
			  bool flipVertical = true,
			  bool flipHorizontal = false);
#endif
		// with header
		Image(std::unique_ptr<Buffer> headerBuffer,
			  bool flipVertical = true,
			  bool flipHorizontal = false);
		// raw
		Image(std::unique_ptr<Buffer> rawBuffer,
			  int width,
			  int height,
			  int bytesPerPixel,
			  bool flipVertical = true,
			  bool flipHorizontal = false);
		~Image();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		int 						width() const;
		int 						height() const;
		int 						bytesPerPixel() const;
		std::unique_ptr<Image> 		inverted() const;
		bool 						writePNG(std::filesystem::path path) const;
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		const Buffer& 				buffer() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		//void 						loadFile(const std::filesystem::path& path, bool flipHorizontal);
		void 						loadBuffer(Buffer& buffer,
											   bool flipVertical,
											   bool flipHorizontal);
		void 						flipVertical(); // "flip"
		void 						flipHorizontal(); // "mirror"

		int							_width;
		int							_height;
		int							_bytesPerPixel;
		std::unique_ptr<Buffer>		_buffer;
	};
}


#endif /* AVARA3D_IMAGE_H */
