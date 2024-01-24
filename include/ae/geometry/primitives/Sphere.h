//
//  Sphere.h
//	avara-engine
//
//  Created by Morgan Davis on 11/6/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Sphere_h
#define Sphere_h


#include <vector>

#include "glm/glm.hpp"

#include "ae/geometry/Geometry.h"
#include "ae/Types.h"


namespace ae {

	class Sphere: public Geometry {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Sphere(float radius, int segments);
		
/*********************************************************************************************
 	Public
 *********************************************************************************************/

		float radius() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float		_radius;

//		void generateIcosahedron(std::vector<Vertex>& verticies, int subdivision);
//		void subdivideIcosahedron(std::vector<Vertex>& verticies, glm::vec3 const& A0, glm::vec3 const& B0, glm::vec3 const& C0, int subdivide);
	};	
}


#endif /* Sphere_h */
