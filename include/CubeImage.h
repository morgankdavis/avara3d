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

#include "MaterialPropertyContents.h"


namespace ae {

	
	class Image;
	

	class CubeImage : public MaterialPropertyContents {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		CubeImage(std::shared_ptr<ae::Image> posX, std::shared_ptr<ae::Image> negX,
				  std::shared_ptr<ae::Image> posY, std::shared_ptr<ae::Image> negY,
				  std::shared_ptr<ae::Image> posZ, std::shared_ptr<ae::Image> negZ);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::shared_ptr<ae::Image> 		posX() const;
		void 			posX(std::shared_ptr<ae::Image> image);
		
		std::shared_ptr<ae::Image> 		negX() const;
		void 			negX(std::shared_ptr<ae::Image> image);
		
		std::shared_ptr<ae::Image> 		posY() const;
		void 			posY(std::shared_ptr<ae::Image> image);
		
		std::shared_ptr<ae::Image> 		negY() const;
		void 			negY(std::shared_ptr<ae::Image> image);
		
		std::shared_ptr<ae::Image> 		posZ() const;
		void 			posZ(std::shared_ptr<ae::Image> image);
		
		std::shared_ptr<ae::Image> 		negZ() const;
		void 			negZ(std::shared_ptr<ae::Image> image);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<ae::Image>		_posX;
		std::shared_ptr<ae::Image>		_negX;
		std::shared_ptr<ae::Image>		_posY;
		std::shared_ptr<ae::Image>		_negY;
		std::shared_ptr<ae::Image>		_posZ;
		std::shared_ptr<ae::Image>		_negZ;
	};
}


#endif /* CubeImage_h */
