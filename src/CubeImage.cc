//
//  CubeImage.cc
//	avara-engine
//
//  Created by Morgan Davis on 4/30/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "ae/CubeImage.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

CubeImage::CubeImage(shared_ptr<Image> posX,
					 shared_ptr<Image> negX,
					 shared_ptr<Image> posY,
					 shared_ptr<Image> negY,
					 shared_ptr<Image> posZ,
					 shared_ptr<Image> negZ):
	_posX(posX),
	_negX(negX),
	_posY(posY),
	_negY(negY),
	_posZ(posZ),
	_negZ(negZ) {
	
}

CubeImage::CubeImage(shared_ptr<Image> one):
	_posX(one),
	_negX(one),
	_posY(one),
	_negY(one),
	_posZ(one),
	_negZ(one) {

}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<Image> CubeImage::posX() const {
	return _posX;
}

void CubeImage::posX(shared_ptr<Image> image) {
	_posX = image;
}

shared_ptr<Image> CubeImage::negX() const {
	return _negX;
}

void CubeImage::negX(shared_ptr<Image> image) {
	_negX = image;
}

shared_ptr<Image> CubeImage::posY() const {
	return _posY;
}

void CubeImage::posY(shared_ptr<Image> image) {
	_posY = image;
}

shared_ptr<Image> CubeImage::negY() const {
	return _negY;
}

void CubeImage::negY(shared_ptr<Image> image) {
	_negY = image;
}

shared_ptr<Image> CubeImage::posZ() const {
	return _posZ;
}

void CubeImage::posZ(shared_ptr<Image> image) {
	_posZ = image;
}

shared_ptr<Image> CubeImage::negZ() const {
	return _negZ;
}

void CubeImage::negZ(shared_ptr<Image> image) {
	_negZ = image;
}
