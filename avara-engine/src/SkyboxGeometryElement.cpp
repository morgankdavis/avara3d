//
//  SkyboxGeometryElement.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/12/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "SkyboxGeometryElement.h"

#include <iostream>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "Material.h"
#include "MaterialProperty.h"
#include "Program.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
	MARK:   Lifecycle
 **************************************************************************************/

SkyboxGeometryElement::SkyboxGeometryElement(vector<Vertex>& verticies,
											 vector<Face>& faces):
	GeometryElement(verticies, faces, "skybox") {
	
}

/***************************************************************************************
     MARK:   GeometryElement
 **************************************************************************************/

unsigned SkyboxGeometryElement::draw(const mat4& viewMat,
									 const mat4& projectionMat,
									 const Material& material) {
	
	// gl config

//	glEnable(GL_DEPTH_TEST); // not needed
//	glDepthFunc(GL_LESS); // not needed
	
	glDepthMask(GL_FALSE);

	// use shader program
	
	m_program->use();

	// uniforms
	
	m_program->setUniform("view", viewMat);
	m_program->setUniform("projection", projectionMat);

	// material
	
	material.ambient()->bind(MaterialPropertyType_Ambient, *m_program);

	// draw

	glBindVertexArray(m_glVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);
	unsigned int facesSize = m_faces.size();
	glDrawElements(GL_TRIANGLES, facesSize * sizeof(Face), GL_UNSIGNED_INT, (void*)0);

	// stats

	return facesSize;
}
