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
		
	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		CubeImage(std::shared_ptr<Image> posX, std::shared_ptr<Image> negX,
				  std::shared_ptr<Image> posY, std::shared_ptr<Image> negY,
				  std::shared_ptr<Image> posZ, std::shared_ptr<Image> negZ);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::shared_ptr<Image> posX() const;
		void posX(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> negX() const;
		void negX(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> posY() const;
		void posY(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> negY() const;
		void negY(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> posZ() const;
		void posZ(std::shared_ptr<Image> image);
		
		std::shared_ptr<Image> negZ() const;
		void negZ(std::shared_ptr<Image> image);
		
	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/
		
		std::shared_ptr<Image>		m_posX;
		std::shared_ptr<Image>		m_negX;
		std::shared_ptr<Image>		m_posY;
		std::shared_ptr<Image>		m_negY;
		std::shared_ptr<Image>		m_posZ;
		std::shared_ptr<Image>		m_negZ;
	};
}


#endif /* CubeImage_h */
