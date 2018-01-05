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
	GeometryElement(verticies, faces) { 

}

/***************************************************************************************
     MARK:   GeometryElement
 **************************************************************************************/

unsigned SkyboxGeometryElement::draw(const mat4& viewMat,
									 const mat4& projectionMat,
									 const Material* material) {
	
	auto program = material->program();
	
	// gl config
	
	glDepthMask(GL_FALSE);

	// use shader program
	
	program->use();

	// uniforms
	
	program->setUniform("view", viewMat);
	program->setUniform("projection", projectionMat);

	// material
	
	material->ambient()->bind(MaterialPropertyType_Ambient, *program);
	
	//material->prepareToRender((DebugOption)0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // this should be replaced with a "SkyboxMaterial"...

	// draw

	glBindVertexArray(m_glVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);
	unsigned int facesSize = m_faces.size();
	glDrawElements(GL_TRIANGLES, facesSize * sizeof(Face), GL_UNSIGNED_INT, (void*)0);

	// stats

	return facesSize;
}
