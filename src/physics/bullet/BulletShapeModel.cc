//
// Created by mkd on 10/29/23.
//

#include "physics/bullet/BulletShapeModel.h"

#include "diagnostic/logging/Logger.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletShapeModel::BulletShapeModel(PhysicsShape* shape):
		PhysicsShapeModel(shape),
		_btShapes(vector<shared_ptr<btCollisionShape>>()),
		_btIndexVertexArrays(vector<shared_ptr<btTriangleIndexVertexArray>>()) { }

BulletShapeModel::~BulletShapeModel() {
	AE_LOG_D("Destroying BulletShapeModel {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/


vector <shared_ptr<btCollisionShape>>& BulletShapeModel::btShapes() {
	return _btShapes;
}

void BulletShapeModel::btShapes(vector<shared_ptr<btCollisionShape>> shapes) {
	_btShapes = shapes;
}

vector <shared_ptr<btTriangleIndexVertexArray>>& BulletShapeModel::btIndexVertexArrays() {
	return _btIndexVertexArrays;
}

void BulletShapeModel::btIndexVertexArrays(vector<shared_ptr<btTriangleIndexVertexArray>> indexVertexArrays) {
	_btIndexVertexArrays = indexVertexArrays;
}
