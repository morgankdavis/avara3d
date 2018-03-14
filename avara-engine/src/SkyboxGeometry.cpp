//
//  SkyboxGeometry.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/12/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "SkyboxGeometry.h"

#include <iostream>

#include "GeometryElement.h"
#include "SkyboxGeometryElement.h"
#include "SkyboxMaterial.h"
#include "Material.h"
#include "Utilities.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
	MARK:   Lifecycle
 **************************************************************************************/

SkyboxGeometry::SkyboxGeometry(const shared_ptr<SkyboxMaterial> material):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {
//	m_name(boost::none),
//	m_elements(elements),
//	m_materials(materials) {

		m_materials = vector<shared_ptr<SkyboxMaterial>>();
		m_materials.emplace_back(material);

		//addMaterial(material);

		Vertex verts[] = {
			{ vec3(-0.5f,	-0.5f, 	0.5f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(-0.5f, 	0.5f, 	0.5f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(0.5f, 	0.5f, 	0.5f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(0.5f, 	-0.5f, 	0.5f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			
			{ vec3(-0.5f, 	-0.5f, 	-0.5f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(-0.5f, 	0.5f, 	-0.5f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(0.5f, 	0.5f, 	-0.5f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(0.5f, 	-0.5f, 	-0.5f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) }
		};
		
		// INWARD facing
		Face faces[] = {
			// front
			{ 0, 1, 2 },
			{ 2, 3, 0 },
			// back
			{ 7, 6, 5 },
			{ 5, 4, 7 },
			// left
			{ 4, 5, 1 },
			{ 1, 0, 4 },
			// right
			{ 3, 2, 6 },
			{ 6, 7, 3 },
			// top
			{ 1, 5, 6 },
			{ 6, 2, 1 },
			// bottom
			{ 4, 0, 3 },
			{ 3, 7, 4 },
		};
		
//		// OUTWARD facing
//		Face faces[] = {
//			// front
//			{ 2, 1, 0 },
//			{ 0, 3, 2 },
//			// back
//			{ 5, 6, 7 },
//			{ 7, 4, 5 },
//			// left
//			{ 1, 5, 4 },
//			{ 4, 0, 1 },
//			// right
//			{ 6, 2, 3 },
//			{ 3, 7, 6 },
//			// top
//			{ 6, 5, 1 },
//			{ 1, 2, 6 },
//			// bottom
//			{ 3, 0, 4 },
//			{ 4, 7, 3 },
//		};
		
		auto vertsVector = vector<Vertex>();
		vertsVector.assign(verts, verts+8);
		
		auto facesVector = vector<Face>();
		facesVector.assign(faces, faces+12);
		
		auto element = make_shared<SkyboxGeometryElement>(vertsVector, facesVector);
		m_elements.push_back(element);
		
		element->loadVertexData(*material->program());
}

/***************************************************************************************
   	MARK:   Internal
 **************************************************************************************/

void SkyboxGeometry::material(std::shared_ptr<SkyboxMaterial> material) {
	m_materials.clear();
	m_materials.emplace_back(material);
	//m_materials.push_back(material);
}

/***************************************************************************************
     MARK:   Geometry
 **************************************************************************************/

void SkyboxGeometry::draw(const mat4& viewMat, const mat4& projectionMat, DrawStats& stats) {

	GeometryElement geoElem = *(m_elements[0]);
	SkyboxGeometryElement& element = static_cast<SkyboxGeometryElement&>(geoElem);
	//Material mat = *(m_materials[0]);
	//SkyboxMaterial& skyMaterial = static_cast<SkyboxMaterial&>(mat);
	
	SkyboxMaterial skyMaterial = *m_materials[0];

	element.draw(viewMat, projectionMat, skyMaterial, stats);
}
