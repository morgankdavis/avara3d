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

#include "ae/rendering/material/Sampleable.h"


namespace ae {

	
	class Image;
	

	class CubeImage : public Sampleable {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		CubeImage(std::shared_ptr<Image> posX,
				  std::shared_ptr<Image> negX,
				  std::shared_ptr<Image> posY,
				  std::shared_ptr<Image> negY,
				  std::shared_ptr<Image> posZ,
				  std::shared_ptr<Image> negZ);
		CubeImage(std::shared_ptr<Image> one);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::shared_ptr<Image>		posX() const;
		void 						posX(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> 		negX() const;
		void 						negX(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> 		posY() const;
		void 						posY(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> 		negY() const;
		void 						negY(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> 		posZ() const;
		void 						posZ(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> 		negZ() const;
		void 						negZ(std::shared_ptr<Image> image);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<Image>		_posX;
		std::shared_ptr<Image>		_negX;
		std::shared_ptr<Image>		_posY;
		std::shared_ptr<Image>		_negY;
		std::shared_ptr<Image>		_posZ;
		std::shared_ptr<Image>		_negZ;
	};
}


#endif /* CubeImage_h */
