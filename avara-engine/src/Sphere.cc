//
//  Sphere.cc
//	avara-engine
//
//  Created by Morgan Davis on 11/6/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Sphere.h"

#include <iostream>
#include <memory>

#include <generator/generator.hpp>

#include "GeometryElement.h"
#include "Utilities.h"
#include "Types.h"


using namespace ae;
using namespace ae::utils;
using namespace generator;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

Sphere::Sphere(float radius, int segments):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {

		m_radius = radius;
		
		/// @param radius The radius of the containing sphere.
		/// @param segments The number of segments per icosahedron edge. Must be >= 1.
		
		IcoSphereMesh icoSphere{radius, segments};

		auto verts = vector<Vertex>();
		for (const MeshVertex& v : icoSphere.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}
		
		auto faces = vector<Face>();
		for (const Triangle& t : icoSphere.triangles()) {
			Face face = { t.vertices[0], t.vertices[1], t.vertices[2] };
			faces.push_back(face);
		}

		auto element = make_shared<GeometryElement>(verts, faces);
		m_elements.push_back(element);
		
		//loadVertexData();
}




//Geometry::Geometry(const vector<shared_ptr<GeometryElement>> elements,
//				   const vector<shared_ptr<Material>> materials):
//m_name(boost::none),
//m_elements(elements),
//m_materials(materials) {
//
//	for (int e=0; e < m_elements.size(); ++e) {
//		auto element = m_elements[e];
//
//		if (m_materials.size() > e) {
//			auto element = m_elements[e];
//			auto material = m_materials[e];
//			element->loadVertexData(*(material->program()));
//		}
//		else {
//			element->loadVertexData(*(Material::DefaultMaterial()->program()));
//		}
//	}
//
//
//	//		for (int m=0; m < m_elements.size() - m_materials.size(); ++m) {
//	//			m_materials.push_back(Material::DefaultMaterial());
//	//		}
//}


//Sphere::Sphere(float radius, int segments):
//	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {
//	generateIcosahedron(verts, subdivisions);
//
//	// "real" faces: https://stackoverflow.com/questions/30912865/how-to-index-the-faces-of-a-icosahedron
//	unsigned face = 0;
//	for (unsigned f=0 ; f<verts.size()/3 ; ++f) {
//		// not 100% sure why the winding order appears to reverse for odd numbers of subdivions...
//		if (subdivisions % 2 == 0) {
//			faces.push_back((Face) {face + 0, face + 1, face + 2});
//		}
//		else {
//			faces.push_back((Face) {face + 2, face + 1, face + 0});
//		}
//		face += 3;
//	}
//
//	auto element = make_shared<GeometryElement>(verts, faces);
//	m_elements.push_back(element);
//
//	generateFlatNormals();
//	//generateSmoothNormals();
//
//	mat4 scale = glm::scale(mat4(1.0f), vec3(1.0f) * radius);
//	hardTransform(scale, true);
//}

/*********************************************************************************************
    	 Public
 *********************************************************************************************/

float Sphere::radius() const {
	return m_radius;
}

/*********************************************************************************************
	Private
*********************************************************************************************/

//void Sphere::generateIcosahedron(vector<Vertex>& verticies, int subdivision) {
//	// https://github.com/g-truc/ogl-samples/blob/master/framework/mesh.cpp
//
//	// the golden ratio
//	float t = (1 + sqrt(5)) / 2;
//	float size = 1.0f;
//
//	vec3 const A = normalize(vec3(-size, t * size, 0.0f));	// 0
//	vec3 const B = normalize(vec3(+size, t * size, 0.0f));	// 1
//	vec3 const C = normalize(vec3(-size,-t * size, 0.0f));	// 2
//	vec3 const D = normalize(vec3(+size,-t * size, 0.0f));	// 3
//
//	vec3 const E = normalize(vec3(0.0f,-size, t * size));	// 4
//	vec3 const F = normalize(vec3(0.0f, size, t * size));	// 5
//	vec3 const G = normalize(vec3(0.0f,-size,-t * size));	// 6
//	vec3 const H = normalize(vec3(0.0f, size,-t * size));	// 7
//
//	vec3 const I = normalize(vec3( t * size, 0.0f,-size));	// 8
//	vec3 const J = normalize(vec3( t * size, 0.0f, size));	// 9
//	vec3 const K = normalize(vec3(-t * size, 0.0f,-size));	// 10
//	vec3 const L = normalize(vec3(-t * size, 0.0f, size));	// 11
//
//	subdivideIcosahedron(verticies, A, L, F, subdivision);
//	subdivideIcosahedron(verticies, A, F, B, subdivision);
//	subdivideIcosahedron(verticies, A, B, H, subdivision);
//	subdivideIcosahedron(verticies, A, H, K, subdivision);
//	subdivideIcosahedron(verticies, A, K, L, subdivision);
//
//	subdivideIcosahedron(verticies, B, F, J, subdivision);
//	subdivideIcosahedron(verticies, F, L, E, subdivision);
//	subdivideIcosahedron(verticies, L, K, C, subdivision);
//	subdivideIcosahedron(verticies, K, H, G, subdivision);
//	subdivideIcosahedron(verticies, H, B, I, subdivision);
//
//	subdivideIcosahedron(verticies, D, J, E, subdivision);
//	subdivideIcosahedron(verticies, D, E, C, subdivision);
//	subdivideIcosahedron(verticies, D, C, G, subdivision);
//	subdivideIcosahedron(verticies, D, G, I, subdivision);
//	subdivideIcosahedron(verticies, D, I, J, subdivision);
//
//	subdivideIcosahedron(verticies, E, J, F, subdivision);
//	subdivideIcosahedron(verticies, C, E, L, subdivision);
//	subdivideIcosahedron(verticies, G, C, K, subdivision);
//	subdivideIcosahedron(verticies, I, G, H, subdivision);
//	subdivideIcosahedron(verticies, J, I, B, subdivision);
//}
//
//void Sphere::subdivideIcosahedron(vector<Vertex>& verticies, vec3 const& A0, vec3 const& B0, vec3 const& C0, int subdivide) {
//	if (subdivide == 0) {
//		verticies.push_back({A0, vec3(0.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)});
//		verticies.push_back({B0, vec3(0.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)});
//		verticies.push_back({C0, vec3(0.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)});
//	}
//	else {
//		vec3 A1 = (B0 + C0) * 0.5f;
//		vec3 B1 = (C0 + A0) * 0.5f;
//		vec3 C1 = (A0 + B0) * 0.5f;
//
//		if (length(A1) > 0.0f) A1 = normalize(A1);
//		if (length(B1) > 0.0f) B1 = normalize(B1);
//		if (length(C1) > 0.0f) C1 = normalize(C1);
//
//		subdivideIcosahedron(verticies, A0, B1, C1, subdivide - 1);
//		subdivideIcosahedron(verticies, B0, C1, A1, subdivide - 1);
//		subdivideIcosahedron(verticies, C0, A1, B1, subdivide - 1);
//		subdivideIcosahedron(verticies, B1, A1, C1, subdivide - 1);
//	}
//}

