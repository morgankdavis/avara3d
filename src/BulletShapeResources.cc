//
// Created by mkd on 10/29/23.
//

#include "BulletShapeResources.h"

#include "Logger.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletShapeResources::BulletShapeResources(shared_ptr<btCollisionShape> shape,
										   shared_ptr<btTriangleIndexVertexArray> indexVertexArray,
										   vector<shared_ptr<btCollisionShape>> childShapes,
										   vector<shared_ptr<btIndexedMesh>> childIndexedMeshes,
										   vector<shared_ptr<btTriangleIndexVertexArray>> childIndexVertexArrays):
		_shape(shape),
		_indexVertexArray(indexVertexArray),
		_childShapes(childShapes),
		_childIndexedMeshes(childIndexedMeshes),
		_childIndexVertexArrays(childIndexVertexArrays) {

}

//BulletShapeResources::~BulletShapeResources() {
//	AE_LOG_D("Destroying BulletShapeResources {:p}", (void*)this);
//}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<btCollisionShape>& BulletShapeResources::shape() {
	return _shape;
}

shared_ptr<btTriangleIndexVertexArray>& BulletShapeResources::indexVertexArray() {
	return _indexVertexArray;
}

vector<shared_ptr<btCollisionShape>>& BulletShapeResources::childShapes() {
	return _childShapes;
}

vector<shared_ptr<btIndexedMesh>>& BulletShapeResources::childIndexedMeshes() {
	return _childIndexedMeshes;
}

vector<shared_ptr<btTriangleIndexVertexArray>>&	BulletShapeResources::childIndexVertexArrays() {
	return _childIndexVertexArrays;
}

