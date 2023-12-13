//
// Created by mkd on 10/29/23.
//

#ifndef BulletShapeResources_h
#define BulletShapeResources_h


#include <memory>
#include <vector>

#include "physics/PhysicsShapeModel.h"


class btCollisionShape;
class btIndexedMesh;
class btRigidBody;
class btTriangleIndexVertexArray;


namespace ae {

	class BulletShapeModel : public PhysicsShapeModel {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletShapeModel();
		~BulletShapeModel();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		std::vector<std::shared_ptr<btCollisionShape>>&				shapes();
		void shapes(std::vector<std::shared_ptr<btCollisionShape>> 	shapes);

		std::vector<std::shared_ptr<btTriangleIndexVertexArray>>&	indexVertexArrays();
		void indexVertexArrays(std::vector<std::shared_ptr<btTriangleIndexVertexArray>> indexVertexArrays);

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		std::vector<std::shared_ptr<btCollisionShape>>				_shapes;
		std::vector<std::shared_ptr<btTriangleIndexVertexArray>>	_indexVertexArrays;
	};
}


#endif //BulletShapeResources_h
