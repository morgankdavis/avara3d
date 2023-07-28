//
//  CubeImage.h
//	avara-engine
//
//  Created by Morgan Davis on 4/30/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef CubeImage_h
#define CubeImage_h


#include <memory>

#include "Aliases.h"
#include "MaterialPropertyContents.h"


namespace ae {

	
	class Image;
	

	class CubeImage : public MaterialPropertyContents {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		CubeImage(ImageSPtr posX, ImageSPtr negX,
				  ImageSPtr posY, ImageSPtr negY,
				  ImageSPtr posZ, ImageSPtr negZ);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		ImageSPtr 		posX() const;
		void 			posX(ImageSPtr image);
		
		ImageSPtr 		negX() const;
		void 			negX(ImageSPtr image);
		
		ImageSPtr 		posY() const;
		void 			posY(ImageSPtr image);
		
		ImageSPtr 		negY() const;
		void 			negY(ImageSPtr image);
		
		ImageSPtr 		posZ() const;
		void 			posZ(ImageSPtr image);
		
		ImageSPtr 		negZ() const;
		void 			negZ(ImageSPtr image);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		ImageSPtr		_posX;
		ImageSPtr		_negX;
		ImageSPtr		_posY;
		ImageSPtr		_negY;
		ImageSPtr		_posZ;
		ImageSPtr		_negZ;
	};
}


#endif /* CubeImage_h */
