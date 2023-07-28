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

#include "Aliases.h"
#include "Types.h"


namespace ae {
	
	
	class Geometry;
	class Node;
	class PhysicsBody;
	

	class PhysicsShape : public std::enable_shared_from_this<PhysicsShape> {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsShape(GeometrySPtr geometry, PHYSICS_SHAPE_TYPE type);
		PhysicsShape(NodeSPtr node, PHYSICS_SHAPE_TYPE type);
		~PhysicsShape();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		GeometryWPtr 						sourceGeometry() const;
		NodeWPtr 							sourceNode() const;
		PHYSICS_SHAPE_TYPE 					type() const;
		std::vector<glm::mat4> 				transforms() const;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void								sourceGeometry(GeometryWPtr geometry);
		void 								sourceNode(NodeWPtr node);

		void 								attachedToBody(PhysicsBodySPtr body);

		PhysicsBodyWPtr						physicsBody() const;
		void 								physicsBody(PhysicsBodySPtr body);

		PHYSICS_SHAPE_DIRTY_BITS 			dirtyBits() const;
		void 								dirtyBits(PHYSICS_SHAPE_DIRTY_BITS bits);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		GeometryWPtr 						_sourceGeometry;
		NodeWPtr							_sourceNode;
		PHYSICS_SHAPE_TYPE 					_type;
		// the array of transforms that was used to create a compound shape.
		std::vector<glm::mat4> 				_transforms;

		std::weak_ptr<PhysicsBody>			_physicsBody;
		
		PHYSICS_SHAPE_DIRTY_BITS 			_dirtyBits;
	};
}


#endif /* PhysicsShape_h */
