//
//  CubeImage.h
//  avara3d
//
//  Created by Morgan Davis on 4/30/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CUBEIMAGE_H
#define AVARA3D_CUBEIMAGE_H


#include <filesystem>
#include <memory>

#include "a3d/rendering/material/Sampleable.h"


namespace a3d {

	
	class Image;
	

	class CubeImage : public Sampleable {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		CubeImage(std::unique_ptr<Image> posX,
				  std::unique_ptr<Image> negX,
				  std::unique_ptr<Image> posY,
				  std::unique_ptr<Image> negY,
				  std::unique_ptr<Image> posZ,
				  std::unique_ptr<Image> negZ);

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		Image*		posX() const;
		void 		posX(std::unique_ptr<Image> image);
		
		Image* 		negX() const;
		void 		negX(std::unique_ptr<Image> image);
		
		Image* 		posY() const;
		void 		posY(std::unique_ptr<Image> image);
		
		Image* 		negY() const;
		void 		negY(std::unique_ptr<Image> image);
		
		Image* 		posZ() const;
		void 		posZ(std::unique_ptr<Image> image);
		
		Image* 		negZ() const;
		void 		negZ(std::unique_ptr<Image> image);
		
/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		std::unique_ptr<Image>		_posX;
		std::unique_ptr<Image>		_negX;
		std::unique_ptr<Image>		_posY;
		std::unique_ptr<Image>		_negY;
		std::unique_ptr<Image>		_posZ;
		std::unique_ptr<Image>		_negZ;
	};
}


#endif /* AVARA3D_CUBEIMAGE_H */
