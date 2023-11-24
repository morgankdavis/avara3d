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

BulletShapeResources::BulletShapeResources():
		_shapes(vector<shared_ptr<btCollisionShape>>()),
		_indexVertexArrays(vector<shared_ptr<btTriangleIndexVertexArray>>()) { }

BulletShapeResources::~BulletShapeResources() {
	AE_LOG_D("Destroying BulletShapeResources {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/


vector <shared_ptr<btCollisionShape>>& BulletShapeResources::shapes() {
	return _shapes;
}

void BulletShapeResources::shapes(vector<shared_ptr<btCollisionShape>> shapes) {
	_shapes = shapes;
}

vector <shared_ptr<btTriangleIndexVertexArray>>& BulletShapeResources::indexVertexArrays() {
	return _indexVertexArrays;
}

void BulletShapeResources::indexVertexArrays(vector<shared_ptr<btTriangleIndexVertexArray>> indexVertexArrays) {
	_indexVertexArrays = indexVertexArrays;
}
