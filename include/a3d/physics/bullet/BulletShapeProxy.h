//
// Created by mkd on 10/29/23.
//

#ifndef BulletShapeResources_h
#define BulletShapeResources_h


#include <memory>
#include <vector>

#include "a3d/physics/proxy/PhysicsShapeProxy.h"


class btCollisionShape;
class btIndexedMesh;
class btRigidBody;
class btTriangleIndexVertexArray;


namespace a3d {

	class BulletShapeProxy : public PhysicsShapeProxy {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		explicit BulletShapeProxy(PhysicsShape& shape);
		~BulletShapeProxy() override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		const std::vector<std::unique_ptr<btCollisionShape>>& btShapes();

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		std::vector<std::unique_ptr<btCollisionShape>>				_btShapes;
		std::vector<std::unique_ptr<btTriangleIndexVertexArray>>	_btIndexVertexArrays;
	};
}


#endif //BulletShapeResources_h
