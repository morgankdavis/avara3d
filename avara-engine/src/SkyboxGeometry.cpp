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
#include "Material.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
	MARK:   Lifecycle
 **************************************************************************************/

SkyboxGeometry::SkyboxGeometry(const shared_ptr<Material> material):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {

		addMaterial(material);

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

void SkyboxGeometry::material(std::shared_ptr<Material> material) {
	m_materials.clear();
	m_materials.push_back(material);
}

//shared_ptr<vector<shared_ptr<Image>>> SkyboxGeometry::cube() const {
//	return m_cube;
//}

/***************************************************************************************
     MARK:   Geometry
 **************************************************************************************/

unsigned SkyboxGeometry::draw(const mat4& viewMat, const mat4& projectionMat) {

	GeometryElement geoElem = *(m_elements[0]);
	SkyboxGeometryElement& element = static_cast<SkyboxGeometryElement&>(geoElem);

	return element.draw(viewMat, projectionMat, &(*m_materials[0]));
}




//unsigned Geometry::draw(const mat4& modelMat,
//						const mat4& viewMat,
//						const mat4& projectionMat) {
//
//	unsigned numPolygons = 0;
//
//	for (int e=0; e < m_elements.size(); ++e) {
//
//		auto element = m_elements[e];
//
//		shared_ptr<Material> material = nullptr;
//		if (m_materials.size() > e) {
//			material = m_materials[e];
//		}
//		else if (m_materials.size() > 0) {
//			material = m_materials[m_materials.size()-1 % e];
//		}
//
//		numPolygons += element->draw(modelMat, viewMat, projectionMat, &(*material));
//	}
//
//	return numPolygons;
//}
