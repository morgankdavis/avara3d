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

#include <glm/glm.hpp>

#include "Geometry.h"
#include "Types.h"


namespace ae {

	
	class Sphere: public Geometry {
		
	public:

/*********************************************************************************************
   	 Lifecycle
 *********************************************************************************************/
		
		Sphere(float radius, int segments);
		
/*********************************************************************************************
    	 Public
 *********************************************************************************************/

		float radius() const;
		
	private:

/*********************************************************************************************
   	 Private
 *********************************************************************************************/
		
		float		m_radius;

//		void generateIcosahedron(std::vector<Vertex>& verticies, int subdivision);
//		void subdivideIcosahedron(std::vector<Vertex>& verticies, glm::vec3 const& A0, glm::vec3 const& B0, glm::vec3 const& C0, int subdivide);
	};	
}


#endif /* Sphere_h */
