//
// Created by mkd on 11/13/23.
//

#include "physics/PhysicsBodyModel.h"


using namespace ae;


PhysicsBodyModel::PhysicsBodyModel(PhysicsBody* body):
		_body(body),
		_shapeModel(nullptr) { }

PhysicsBodyModel::~PhysicsBodyModel() { }
