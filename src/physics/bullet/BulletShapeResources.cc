//
// Created by mkd on 10/29/23.
//

#include "physics/bullet/BulletShapeResources.h"

#include "diagnostic/logging/Logger.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletShapeResources::BulletShapeResources(vector<shared_ptr<btCollisionShape>> shapes,
										   vector<shared_ptr<btTriangleIndexVertexArray>> indexVertexArrays):
		_shapes(shapes),
		_indexVertexArrays(indexVertexArrays) {


}

//	BulletShapeResources::BulletShapeResources(shared_ptr <btCollisionShape> shape,
//											   shared_ptr <btTriangleIndexVertexArray> indexVertexArray,
//											   vector <shared_ptr<btCollisionShape>> childShapes,
//											   vector <shared_ptr<btTriangleIndexVertexArray>> childIndexVertexArrays) :
//			_shape(shape),
//			_indexVertexArray(indexVertexArray),
//			_childShapes(childShapes),
//			_childIndexVertexArrays(childIndexVertexArrays) {
//
//	}

BulletShapeResources::~BulletShapeResources() {
	AE_LOG_D("Destroying BulletShapeResources {:p}", (void*)this);

	// SIMULATOR -> DESTROY()
//	for (auto shape : _shapes) {
//		delete shape;
//	}
	//_simulator->DestroyShapResources(_shapes);
}

void BulletShapeResources::get(PhysicsSimulator& simulator,

							   shared_ptr<PhysicsShape>* shape,
							   bool& newlyCreated,
							   FrameStats& stats) {

}

/*********************************************************************************************
	Public
 *********************************************************************************************/


vector <shared_ptr<btCollisionShape>> &BulletShapeResources::shapes() {
	return _shapes;
}

vector <shared_ptr<btTriangleIndexVertexArray>> &BulletShapeResources::indexVertexArrays() {
	return _indexVertexArrays;
}

//	shared_ptr <btCollisionShape> &BulletShapeResources::shape() {
//		return _shape;
//	}
//
//	shared_ptr <btTriangleIndexVertexArray> &BulletShapeResources::indexVertexArray() {
//		return _indexVertexArray;
//	}
//
//	vector <shared_ptr<btCollisionShape>> &BulletShapeResources::childShapes() {
//		return _childShapes;
//	}
//
//	vector <shared_ptr<btTriangleIndexVertexArray>> &BulletShapeResources::childIndexVertexArrays() {
//		return _childIndexVertexArrays;
//	}

