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

BulletShapeModel::BulletShapeModel():
		_shapes(vector<shared_ptr<btCollisionShape>>()),
		_indexVertexArrays(vector<shared_ptr<btTriangleIndexVertexArray>>()) { }

BulletShapeModel::~BulletShapeModel() {
	AE_LOG_D("Destroying BulletShapeModel {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/


vector <shared_ptr<btCollisionShape>>& BulletShapeModel::shapes() {
	return _shapes;
}

void BulletShapeModel::shapes(vector<shared_ptr<btCollisionShape>> shapes) {
	_shapes = shapes;
}

vector <shared_ptr<btTriangleIndexVertexArray>>& BulletShapeModel::indexVertexArrays() {
	return _indexVertexArrays;
}

void BulletShapeModel::indexVertexArrays(vector<shared_ptr<btTriangleIndexVertexArray>> indexVertexArrays) {
	_indexVertexArrays = indexVertexArrays;
}
