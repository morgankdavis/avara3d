//
//  PhysicsShape.h
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsShape_h
#define PhysicsShape_h


#include <memory>
#include <vector>

#include "Types.h"


namespace ae {
	
	
	class Geometry;
	

	class PhysicsShape {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		PhysicsShape(std::shared_ptr<Geometry> geometry, PhysicsShapeOption options);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::shared_ptr<Geometry> sourceGrometry() const;
		PhysicsShapeOption options() const;
		std::vector<glm::mat4> transforms() const;
	};
}


#endif /* PhysicsShape_h */
