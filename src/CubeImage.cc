//
//  CubeImage.cc
//  avara3d
//
//  Created by Morgan Davis on 4/30/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/CubeImage.h"

#include <utility>

#include "a3d/Image.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

CubeImage::CubeImage(unique_ptr<Image> posX,
					 unique_ptr<Image> negX,
					 unique_ptr<Image> posY,
					 unique_ptr<Image> negY,
					 unique_ptr<Image> posZ,
					 unique_ptr<Image> negZ):
	_posX{std::move(posX)},
	_negX{std::move(negX)},
	_posY{std::move(posY)},
	_negY{std::move(negY)},
	_posZ{std::move(posZ)},
	_negZ{std::move(negZ)} { }

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

Image* CubeImage::posX() const {
	return _posX.get();
}

void CubeImage::posX(unique_ptr<Image> image) {
	_posX = std::move(image);
}

Image* CubeImage::negX() const {
	return _negX.get();
}

void CubeImage::negX(unique_ptr<Image> image) {
	_negX = std::move(image);
}

Image* CubeImage::posY() const {
	return _posY.get();
}

void CubeImage::posY(unique_ptr<Image> image) {
	_posY = std::move(image);
}

Image* CubeImage::negY() const {
	return _negY.get();
}

void CubeImage::negY(unique_ptr<Image> image) {
	_negY = std::move(image);
}

Image* CubeImage::posZ() const {
	return _posZ.get();
}

void CubeImage::posZ(unique_ptr<Image> image) {
	_posZ = std::move(image);
}

Image* CubeImage::negZ() const {
	return _negZ.get();
}

void CubeImage::negZ(unique_ptr<Image> image) {
	_negZ = std::move(image);
}
