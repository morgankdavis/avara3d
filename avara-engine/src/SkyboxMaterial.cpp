//
//  SkyboxMaterial.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/8/18.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "SkyboxMaterial.h"

#include <GL/glew.h>

#include "Program.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
	MARK:   Lifecycle
 **************************************************************************************/

SkyboxMaterial::SkyboxMaterial(shared_ptr<MaterialProperty> cubeProperty):
Material(nullptr, nullptr, nullptr, Program::Skybox()),
	m_cubeProperty(cubeProperty) {

}

/***************************************************************************************
   	MARK:   Internal
 **************************************************************************************/

std::shared_ptr<MaterialProperty> SkyboxMaterial::cubeProperty() const {
	return m_cubeProperty;
}

void SkyboxMaterial::cubeProperty(std::shared_ptr<MaterialProperty> property) {
	m_cubeProperty = property;
}

/***************************************************************************************
     MARK:   Material
 **************************************************************************************/

void SkyboxMaterial::prepareToRender(DEBUG_OPTIONS debugOptions) const {
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
}
