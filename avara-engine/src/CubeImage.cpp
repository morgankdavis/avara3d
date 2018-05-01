//
//  CubeImage.cpp
//	avara-engine
//
//  Created by Morgan Davis on 4/30/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "CubeImage.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

CubeImage::CubeImage(shared_ptr<Image> posX, shared_ptr<Image> negX,
					 shared_ptr<Image> posY, shared_ptr<Image> negY,
					 shared_ptr<Image> posZ, shared_ptr<Image> negZ):
	m_posX(posX),
	m_negX(negX),
	m_posY(posY),
	m_negY(negY),
	m_posZ(posZ),
	m_negZ(negZ) {
	
}

/***************************************************************************************
     Public
 ***************************************************************************************/

shared_ptr<Image> CubeImage::posX() const {
	return m_posX;
}

void CubeImage::posX(shared_ptr<Image> image) {
	m_posX = image;
}

shared_ptr<Image> CubeImage::negX() const {
	return m_negX;
}

void CubeImage::negX(shared_ptr<Image> image) {
	m_negX = image;
}

shared_ptr<Image> CubeImage::posY() const {
	return m_posY;
}

void CubeImage::posY(shared_ptr<Image> image) {
	m_posY = image;
}

shared_ptr<Image> CubeImage::negY() const {
	return m_negY;
}

void CubeImage::negY(shared_ptr<Image> image) {
	m_negY = image;
}

shared_ptr<Image> CubeImage::posZ() const {
	return m_posZ;
}

void CubeImage::posZ(shared_ptr<Image> image) {
	m_posZ = image;
}

shared_ptr<Image> CubeImage::negZ() const {
	return m_negZ;
}

void CubeImage::negZ(shared_ptr<Image> image) {
	m_negZ = image;
}
