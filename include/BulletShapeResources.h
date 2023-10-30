
//
// Created by mkd on 10/29/23.
//

#ifndef BulletShapeResources_h
#define BulletShapeResources_h


#include <memory>
#include <vector>


class btCollisionShape;
class btRigidBody;
class btTriangleIndexVertexArray;


namespace ae {


	class BulletShapeResources {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletShapeResources(std::vector<std::shared_ptr<btCollisionShape>> shapes,
							 std::shared_ptr<btTriangleIndexVertexArray> indexVertexArrays);
//		~BulletShapeResources();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::vector<std::shared_ptr<btCollisionShape>>&		shapes();
		std::shared_ptr<btTriangleIndexVertexArray>&		indexVertexArrays();

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		std::vector<std::shared_ptr<btCollisionShape>>		_shapes;
		std::shared_ptr<btTriangleIndexVertexArray>			_indexVertexArrays;
	};
}


#endif //BulletShapeResources_h
