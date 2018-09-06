//
//  Box.h
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Box_h
#define Box_h


#include "Geometry.h"


namespace ae {

	
	class Box: public Geometry {
		
	public:

		/***************************************************************************************
	    	 Lifecycle
	 	 ***************************************************************************************/
		
		Box(float width, float height, float length);
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		float width() const;
		float height() const;
		float length() const;
		
	private:
		
		/***************************************************************************************
			 Private
		 ***************************************************************************************/
		
		float	m_width;
		float	m_height;
		float	m_length;
	};
}


#endif /* Box_h */
