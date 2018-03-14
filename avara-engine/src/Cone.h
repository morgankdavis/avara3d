//
//  Cone.h
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Cone_h
#define Cone_h


#include "Geometry.h"


namespace ae {

	
	class Cone: public Geometry {
		
	public:

		/***************************************************************************************
	    	 MARK:   Lifecycle
	 	 **************************************************************************************/
		
		Cone(float radius, float height, int slices, int segments);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		float radius() const;
		float height() const;
		
	private:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		float 	m_radius;
		float 	m_height;
	};
}


#endif /* Cone_h */
