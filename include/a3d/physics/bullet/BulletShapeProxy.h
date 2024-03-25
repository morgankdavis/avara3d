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

		std::vector<std::shared_ptr<btCollisionShape>>& btShapes();
//		void btShapes(std::vector<std::shared_ptr<btCollisionShape>> shapes);

//		std::vector<std::shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays();
//		void btIndexVertexArrays(std::vector<std::shared_ptr<btTriangleIndexVertexArray>> indexVertexArrays);

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		std::vector<std::shared_ptr<btCollisionShape>>				_btShapes;
		std::vector<std::shared_ptr<btTriangleIndexVertexArray>>	_btIndexVertexArrays;
	};
}


#endif //BulletShapeResources_h
