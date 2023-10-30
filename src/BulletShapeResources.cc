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

BulletShapeResources::BulletShapeResources(vector<shared_ptr<btCollisionShape>> shapes,
										   shared_ptr<btTriangleIndexVertexArray> indexVertexArrays):
		_shapes(shapes),
		_indexVertexArrays(indexVertexArrays) {

}

//BulletShapeResources::~BulletShapeResources() {
//	AE_LOG_D("Destroying BulletShapeResources {:p}", (void*)this);
//}

/*********************************************************************************************
	Public
 *********************************************************************************************/

vector<shared_ptr<btCollisionShape>>& BulletShapeResources::shapes() {
	return _shapes;
}

shared_ptr<btTriangleIndexVertexArray>& BulletShapeResources::indexVertexArrays() {
	return _indexVertexArrays;
}
