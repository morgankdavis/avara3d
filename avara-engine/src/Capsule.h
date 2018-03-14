//
//  Capsule.h
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Capsule_h
#define Capsule_h


#include "Geometry.h"


namespace ae {

	
	class Capsule: public Geometry {
		
	public:

		/***************************************************************************************
	    	 MARK:   Lifecycle
	 	 **************************************************************************************/
		
		Capsule(float radius, float height, int slices, int segments, int rings);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		float radius() const;
		float height() const;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		float		m_radius;
		float		m_height;
	};
}


#endif /* Capsule_h */
