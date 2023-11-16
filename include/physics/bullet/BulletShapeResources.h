//
// Created by mkd on 10/29/23.
//

#ifndef BulletShapeResources_h
#define BulletShapeResources_h


#include <memory>
#include <vector>

#include "physics/PhysicsShapeResources.h"


class btCollisionShape;
class btIndexedMesh;
class btRigidBody;
class btTriangleIndexVertexArray;


namespace ae {


//	class PhysicsShape;
//	class PhysicsSimulator;


	class BulletShapeResources : public PhysicsShapeResources {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletShapeResources(std::vector<std::shared_ptr<btCollisionShape>> shapes,
							 std::vector<std::shared_ptr<btTriangleIndexVertexArray>> indexVertexArrays);

//		BulletShapeResources(std::shared_ptr<btCollisionShape> shape,
//							 std::shared_ptr<btTriangleIndexVertexArray> indexVertexArray,
//							 std::vector<std::shared_ptr<btCollisionShape>> childShapes,
//							 std::vector<std::shared_ptr<btTriangleIndexVertexArray>> childIndexVertexArrays);
		~BulletShapeResources();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		// - if 'shape' is a normal btCollisionShape with dynamic or kinematic body,
		// 'indexVertexArray', 'childShapes', and 'childIndexVertexArrays' will be empty.
		// - if 'shape' is a normal btCollisionShape with static body,
		// 'indexVertexArray' will have vertex data shared with one or more GeometryElements,
		// and 'childShapes' and 'childIndexVertexArrays' will be empty.
		// - if 'shape' is a btCompoundShape with dynamic or kinematic body,
		// 'indexVertexArray' and 'childIndexVertexArrays' will be empty, and
		// 'childIndexVertexArrays' will contain child shapes.
		// - if 'shape' is a btCompoundShape with static body,
		// 'indexVertexArray' will be empty, 'childShapes' will contain the child shapes,
		// and 'childIndexVertexArrays' will contain the child index vertex arrays.
//x		 - if 'shape' type is CONVEX_POLYHEDRON attached to a dynamic body, 'hacdGeometry'
//x		 will contain the HACD data for the mesh.

		std::vector<std::shared_ptr<btCollisionShape>>				shapes();
		std::vector<std::shared_ptr<btTriangleIndexVertexArray>>	indexVertexArrays();

//		std::shared_ptr<btCollisionShape>&							shape();
//		std::shared_ptr<btTriangleIndexVertexArray>&				indexVertexArray();
//		std::vector<std::shared_ptr<btCollisionShape>>&				childShapes();
//		std::vector<std::shared_ptr<btTriangleIndexVertexArray>>&	childIndexVertexArrays();
//		std::shared_ptr<Geometry>&									hacdGeometry();

//		void									get(PhysicsSimulator& simulator,
//													std::shared_ptr<PhysicsShape>* shape,
//													bool& newlyCreated,
//													FrameStats& stats) override;

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		std::vector<std::shared_ptr<btCollisionShape>>				_shapes;
		std::vector<std::shared_ptr<btTriangleIndexVertexArray>>	_indexVertexArrays;
//		std::shared_ptr<btCollisionShape>							_shape;
//		std::shared_ptr<btTriangleIndexVertexArray>					_indexVertexArray;
//		std::vector<std::shared_ptr<btCollisionShape>>				_childShapes;
//		std::vector<std::shared_ptr<btTriangleIndexVertexArray>>	_childIndexVertexArrays;
//		std::shared_ptr<Geometry>									_hacdGeometry;

//		std::weak_ptr<BulletPhysicsSimulator>						_simulator;
	};
}


#endif //BulletShapeResources_h
