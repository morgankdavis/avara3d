//
//  PhysicsShape.cc
//	avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicsShape.h"


#include "magic_enum.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/proxy/PhysicsShapeProxy.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/physics/bullet/BulletShapeProxy.h"
#include "a3d/scene/Node.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsShape::PhysicsShape(PhysicsShapeType type, Mesh* mesh):
		_sourceObject(mesh),
		_bodies({}),
		_type(type),
		_proxy(nullptr)
		/*_model(make_unique<BulletShapeProxy>(this))*/ {

	if (auto name = mesh->name()) {
		A3D_LOG_D("Creating PhysicsShape type {} for source mesh: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		A3D_LOG_D("Creating PhysicsShape type {} for source mesh: {:p}...",
				 magic_enum::enum_name(type), static_cast<void*>(mesh));
	}
}

// construct a compound shape based on meshes under this node
PhysicsShape::PhysicsShape(PhysicsShapeType type, Node* node):
		_sourceObject(node),
		_bodies({}),
		_type(type),
		_proxy(nullptr)
		/*_model(make_unique<BulletShapeProxy>(this))*/ {

	if (auto name = node->name()) {
		A3D_LOG_D("Creating PhysicsShape type {} for source node: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		A3D_LOG_D("Creating PhysicsShape type {} for source node: {:p}...",
				 magic_enum::enum_name(type),static_cast<void*>(node));
	}
}

PhysicsShape::PhysicsShape():
		_sourceObject(monostate{}),
		_bodies({}),
		_proxy(nullptr)
		/*_model(make_unique<BulletShapeProxy>(this))*/ { }

PhysicsShape::~PhysicsShape() {
	A3D_LOG_D("Destroying PhysicsShape {:p}", static_cast<void*>(this));

//	for (auto& body : _bodies) {
//
//	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

PhysicsShape::SourceObject PhysicsShape::sourceObject() const {
	return _sourceObject;
}

PhysicsShapeType PhysicsShape::type() const {
	return _type;
}

void PhysicsShape::type(PhysicsShapeType type) {
	A3D_LOG_T("type: {}", magic_enum::enum_name(type));

	_type = type;
	_proxy = nullptr;

	checkCreateProxy();

//	for (auto body : _bodies) {
//		body->shapeUpdated();
//	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsShape::attachedToBody(PhysicsBody* body) {
	A3D_LOG_T("body: {:p}", static_cast<void*>(body));

	if (!_bodies.count(body)) {
		_bodies.insert(body);

		checkCreateProxy();
	}
}

void PhysicsShape::detachedFromBody(PhysicsBody* body) {
	A3D_LOG_T("body: {:p}", static_cast<void*>(body));

	_bodies.erase(body);
}

void PhysicsShape::physicalWorldReachable(PhysicalWorld* world) {
	A3D_LOG_T("world: {:p}", static_cast<void*>(world));

	checkCreateProxy();
}

void PhysicsShape::physicalWorldUnreachable(PhysicalWorld* world) {
	A3D_LOG_T("world: {:p}", static_cast<void*>(world));
}

void PhysicsShape::sourceObject(SourceObject sourceObject) {

	_sourceObject = sourceObject;
}

void PhysicsShape::checkCreateProxy() {
	A3D_LOG_T("");

	if (!_proxy) {
		_proxy = make_unique<BulletShapeProxy>(this);

		for (auto body : _bodies) {
			body->shapeUpdated();
		}
	}
}

unordered_set<PhysicsBody*> PhysicsShape::bodies() const {
	return _bodies;
}

PhysicsShapeProxy* PhysicsShape::proxy() const {
	return _proxy.get();
}
