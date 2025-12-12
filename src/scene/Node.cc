//
//  Node.cc
//  avara3d
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/scene/Node.h"

#include <algorithm>
#include <format>
#include <utility>

#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/Math.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsShape.h"
#include "a3d/scene/Scene.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/light/Light.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

//#define ALTERNATE_EULERS

/// Pulic Static Members ///

shared_ptr<Node> Node::NamedNode(const string& name) {
	return make_unique<Node>(name);
}

shared_ptr<Node> Node::MeshNode(const shared_ptr<Mesh>& mesh) {
	return make_unique<Node>(mesh);
}

shared_ptr<Node> Node::LightNode(const shared_ptr<Light>& light) {
	return make_unique<Node>(light);
}

shared_ptr<Node> Node::CameraNode(const shared_ptr<Camera>& camera) {
	return make_unique<Node>(camera);
}

/// Public Lifecycle Functions ///

Node::Node():
		_name{},
		_camera{},
		_light{},
		_mesh{},
		_position{0.0f, 0.0f, 0.0f},
		_orientation{},
		_scale{1.0f, 1.0f, 1.0f},
		_physicsBody{},
		_hidden{false},
		_scene{},
		_parent{},
		_dirtyMask{NodeDirtyMask::None} { }

Node::Node(const string& name):
		Node() {
	_name = name;
}

Node::Node(const shared_ptr<Mesh>& mesh):
		Node() {
	_mesh = mesh;
}

Node::Node(const shared_ptr<Light>& light):
		Node() {
	_light = light;
}

Node::Node(const shared_ptr<Camera>& camera):
		Node() {
	_camera = camera;
}

Node::~Node() {

	if (_name != nullopt) {
		A3D_LOG_D("Destroying Node '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		A3D_LOG_D("Destroying Node {:p}", static_cast<void*>(this));
	}

	for (auto& child : _children) child->detachedFromParent(*this);

	// since PhysicsBody's 'node' is a weak_ptr, all that detachedFromNode did was
	// set 'node' to an empty weak_ptr -- uncesessary.
	//if (_physicsBody) _physicsBody->detachedFromNode(*this);

//	physicsBody(nullptr);
}

/// Public Member Functions ///

const optional<std::string>& Node::name() const {
	return _name;
}

void Node::name(const string& name) {
	_name = name;
}

const shared_ptr<Light>& Node::light() const {
	return _light;
}

void Node::light(const shared_ptr<Light>& light) {
	_light = light;
}

const shared_ptr<Camera>& Node::camera() const {
	return _camera;
}

void Node::camera(const shared_ptr<Camera>& camera) {
	_camera = camera;
}

const shared_ptr<Mesh>& Node::mesh() const {
	return _mesh;
}

void Node::mesh(const shared_ptr<Mesh>& mesh) {

	if (_physicsBody && _mesh) {
		_physicsBody->meshDetachedFromNode(mesh);
	}

	_mesh = mesh;

	if (_physicsBody && _mesh) {
		_physicsBody->meshAttachedToNode(mesh);
	}
}

vec3 Node::position() const {
	return _position;
}

void Node::position(const vec3& position) {
	_position = position;
}

vec4 Node::rotation() const {
	return axis_angle(_orientation);
}

void Node::rotation(const vec3& axis, float angle) {
	_orientation = axis_angle(axis, angle);
}

vec3 Node::eulerAngles() const {  // pitch, yaw, roll

	// eulerAngleYXZ()
	// yawPitchRoll()
	// https://glm.g-truc.net/0.9.3/api/a00164.html#ga4c297724e663cb77cc2cf7e4ab89b77e

	// !? https://glm.g-truc.net/0.9.0/api/a00151.html

	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	// https://download.tuxfamily.org/arakhne/apidocs/afc/org/arakhne/afc/math/geometry/d3/doc-files/euler_plane.gif
	// note that the linked equation seems to have switched attitude and bank

#ifndef ALTERNATE_EULERS

	// works great, but appears to be ZXY order.
	// different ordering? http://graphics.wikia.com/wiki/Conversion_between_quaternions_and_Euler_angles

	auto q = _orientation;

	float pitch = math::atan2(2.0f*q.x*q.w - 2.0f*q.y*q.z, 1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z);
	float yaw = math::atan2(2.0f*q.y*q.w - 2.0f*q.x*q.z, 1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z);
	float roll = math::asin(2*q.x*q.y + 2.0f*q.z*q.w);

	return vec3(pitch, yaw, roll);
	
#else
	// http://bediyap.com/programming/convert-quaternion-to-euler-rotations/

	auto q = _orientation;
	vec3 res = vec3(0.0f, 0.0f, 0.0f);
	res.x = atan2(2*(q.y*q.z + q.w*q.x), q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
	res.y = asin(-2*(q.x*q.z - q.w*q.y));
	res.z = atan2(2*(q.x*q.y + q.w*q.z), q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z);
	
	return res;
#endif

	// clips to +-180
	//return eulerAngles(_orientation);
}

void Node::eulerAngles(const vec3& eulerAngles) { // pitch, yaw, roll

	// NOTE:
	// this first formula works well for one rotation at a time,
	// but it combines multiple rotations in a different order than SceneKit
	// so we break it into three different rotations and apply them how we like.
	
	
#ifndef ALTERNATE_EULERS
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	
	// this works. order appears to be different from SceneKit

	float pitch = eulerAngles.x;
	float yaw = eulerAngles.y;
	float roll = eulerAngles.z;

	// ORIGINAL
	float c1 = math::cos(yaw / 2.0f);
	float c2 = math::cos(roll / 2.0f);
	float c3 = math::cos(pitch / 2.0f);
	float s1 = math::sin(yaw / 2.0f);
	float s2 = math::sin(roll / 2.0f);
	float s3 = math::sin(pitch / 2.0f);

	float w = c1*c2*c3 - s1*s2*s3;
	float x = s1*s2*c3 + c1*c2*s3;
	float y = s1*c2*c3 + c1*s2*s3;
	float z = c1*s2*c3 - s1*c2*s3;

	_orientation = quat(w, x, y, z);
	
#else

	// https://gamedev.stackexchange.com/questions/13436/glm-euler-angles-to-quaternion



//	float sx = sin(eulerAngles.x/2.0), sy = sin(eulerAngles.y/2.0), sz = sin(eulerAngles.z/2.0),
//	cx = cos(eulerAngles.x/2.0), cy = cos(eulerAngles.y/2.0), cz = cos(eulerAngles.z/2.0);
//
//	_orientation = normalize(quat( cx*cy*cz + sx*sy*sz,
//	   sx*cy*cz - cx*sy*sz,
//	   cx*sy*cz + sx*cy*sz,
//	   cx*cy*sz - sx*sy*cz )); // for XYZ application order



	//_orientation = toQuat( orientate3( eulerAngles ) );

	//_orientation = toQuat( yawPitchRoll( eulerAngles.y, eulerAngles.x, eulerAngles.z ) );


//	// https://www.opengl.org/discussion_boards/showthread.php/174858-GLM-Initializing-Quaternion-with-Eular-XYZ
//	quat quatAroundX = angleAxis( eulerAngles.x, vec3(1.0,0.0,0.0) );
//	quat quatAroundY = angleAxis( eulerAngles.y, vec3(0.0,1.0,0.0) );
//	quat quatAroundZ = angleAxis( eulerAngles.z, vec3(0.0,0.0,1.0) );
//	//quat finalOrientation = normalize(quatAroundX * quatAroundY * quatAroundZ);
//	quat finalOrientation = quatAroundZ * quatAroundY * quatAroundX;
//	_orientation = finalOrientation;




	//_orientation = quat(eulerAngles); // WOW this works, but still acts strange after 180


	//return;
	
	auto rotationX = rotate(mat4(1.0f), eulerAngles.x, vec3(1.0f, 0.0f, 0.0f));
	auto rotationY = rotate(mat4(1.0f), eulerAngles.y, vec3(0.0f, 1.0f, 0.0f));
	auto rotationZ = rotate(mat4(1.0f), eulerAngles.z, vec3(0.0f, 0.0f, 1.0f));

	//_orientation = normalize(quat_cast(rotationZ * rotationX * rotationY)); // equation above order
	_orientation = normalize(quat_cast(rotationZ * rotationY * rotationX)); // SceneKit order
#endif
}

quat Node::orientation() const {
	return _orientation;
}

void Node::orientation(const quat& orientation) {
	_orientation = orientation;
}

vec3 Node::scale() const {
	return _scale;
}

void Node::scale(const vec3& scale) {
	_scale = scale;
}

mat4 Node::transform() const {

	mat4 t = translate(mat4(1.0), _position);
	mat4 r = mat4_cast(_orientation);
	mat4 s = math::scale(mat4(1.0), _scale);
	
	return t * r * s;
}

vec3 Node::forward() const {

	// method 1
	vec3 localForward{0.0f, 0.0f, -1.0f}; // assuming local forward is -Z
	return normalize(rotate(_orientation, localForward));

	// method 2
	// mat4 t = transform();
	// column 2 = +Z in world space GLM/OpenGL conventions
	// if local forward is -Z instead of +Z, negate
	// return normalize(-vec3{t.c2.x, t.c2.y, t.c2.z});
}

vec3 Node::up() const {

	// method 1
	vec3 localForward{0.0f, 1.0f, 0.0f};
	return normalize(rotate(_orientation, localForward));

	// method 2
	// mat4 t = transform();
	// return normalize({t.c1.x, t.c1.y, t.c1.z });
}

vec3 Node::right() const {

	// method 1
	vec3 localForward{1.0f, 0.0f, 0.0f};
	return normalize(rotate(_orientation, localForward));

	// method 2
	// mat4 t = transform();
	// return normalize(t.c0.x, t.c0.y, t.c0.z});
}

void Node::transform(const mat4& transform) {

	vec3 scale;
	quat orientation;
	vec3 translation;

	decompose_trs(transform,
				  scale,
				  orientation,
				  translation);

	_position = translation;
	_scale = scale;
	_orientation = orientation;
}

vec3 Node::worldPosition() const {

	return vec3(worldTransform()[3]);
}

vec4 Node::worldRotation() const {

	return axis_angle(worldOrientation());
}

vec3 Node::worldEulerAngles() const {

	throw Exception("worldEulerAngles() not implemented."); // TODO: custom exception
	return {0.0, 0.0, 0.0};
}

quat Node::worldOrientation() const {

	if (auto parent = _parent.lock()) {
		return normalize(parent->worldOrientation() * _orientation);
	}
	else {
		return _orientation;
	}
}

vec3 Node::worldScale() const {

	mat4 w = worldTransform();
	return { length(vec3(w.c0)),
			 length(vec3(w.c1)),
			 length(vec3(w.c2)) };
}

vec3 Node::worldForward() const {

	// method 1
	return normalize(rotate(worldOrientation(), {0.0f, 0.0f, -1.0f})); // local forward is -Z

	// method 2
	// mat4 w = worldTransform();
	// column 2 = +Z in world space GLM/OpenGL conventions
	// if local forward is -Z instead of +Z, negate
	// return normalize(-vec3{w.c2.x, w.c2.y, w.c2.z});
}

vec3 Node::worldUp() const {

	// method 1
	return normalize(rotate(worldOrientation(), {0.0f, 1.0f, 0.0f}));

	// method 2
	// mat4 w = worldTransform();
	// return normalize({w.c1.x, w.c1.y, w.c1.z});
}

vec3 Node::worldRight() const {

	// method 1
	return normalize(rotate(worldOrientation(), {1.0f, 0.0f, 0.0f}));

	// method 2
	// mat4 w = worldTransform();
	// return normalize({w.c0.x, w.c0.y, w.c0.z});
}

mat4 Node::worldTransform() const {

	if (auto parent = _parent.lock()) {
		return parent->worldTransform() * transform();
	}
	else {
		return transform();
	}
}

void Node::addChild(const shared_ptr<Node>& node) {

	if (containsChild(node)) {
		throw Exception(std::format("Node already exists in tree: {:p}, (\"{}\")",
									static_cast<void*>(node.get()),
									(node->name() ? *node->name() : "(unnamed)")));
	}

	_children.push_back(node);

	node->_parent = shared_from_this();
	node->attachedToParent(*this);
}

void Node::addChildren(const vector<shared_ptr<Node>>& nodes) {
	for (auto& node : nodes) {
		addChild(node);
	}
}

void Node::removeFromParent() {

	if (auto parent = _parent.lock()) {
		// https://stackoverflow.com/questions/39912/how-do-i-remove-an-item-from-a-stl-vector-with-a-certain-value
		// https://stackoverflow.com/questions/3385229/c-erase-vector-element-by-value-rather-than-by-position
		// http://en.cppreference.com/w/cpp/algorithm/remove
		// new: https://stackoverflow.com/questions/875103/how-do-i-erase-an-element-from-stdvector-by-index

		auto existingChildren = parent->_children;
		auto newChildren = vector<shared_ptr<Node>>();
		newChildren.reserve(existingChildren.size()-1);
		for (auto& child : existingChildren) {
			if (child.get() != this) {
				newChildren.push_back(child);
			}
		}
		parent->_children = newChildren;

		detachedFromParent(*parent);
	}
	else {
		A3D_LOG_W("Parent is gone!");
		// TODO: throw?
	}
}

vector<shared_ptr<Node>> Node::children(bool resursive) const {

	if (resursive) {
		return children(*this);
	}
	else {
		return _children;
	}
}

shared_ptr<Node> Node::childNamed(const string& name, bool resursive) const {

	for (auto& child : children(resursive)) {
		if (child->name() != nullopt && *child->name() == name) {
			return child;
		}
	}
	return nullptr;
}

PhysicsBody* Node::physicsBody() const {
	return _physicsBody.get();
}

void Node::physicsBody(unique_ptr<PhysicsBody> body) {

	if (_physicsBody) {
		_physicsBody->detachedFromNode(shared_from_this());
	}

	_physicsBody = std::move(body);

	if (_physicsBody) {
		_physicsBody->attachedToNode(shared_from_this());
	}
}

bool Node::hidden() const {
	return _hidden;
}

void Node::hidden(bool hidden) {
	_hidden = hidden;
}

Scene* Node::scene() const {

	if (_scene) {
		return _scene;
	}
	else if (auto parent = _parent.lock()) {
		return parent->scene();
	}
	return nullptr;
}

weak_ptr<Node> Node::parent() const {
	return _parent;
}

/// Internal Member Functions ///

void Node::attachedToParent(Node& parent) {
	A3D_LOG_T("parent: {:p}", static_cast<void*>(&parent));

	// _parent = parent; // moved to Node::addChild() to avoid needing to pass 'parent' as a shared_ptr

	// the only Node with a direct pointer to the Scene is the root node,
	// and attachedToParent() is never called on the root node.
	// if this is another Scene's root node being attached to this scene
	// (such as a scene loaded from a file), we don't want a stale pointer
	// to the old scene.
	_scene = nullptr;

	checkNotifyPhysicsBodyOfReachablePhysicalWorld();

	for (auto& child : _children) {
		child->ancestorAttachedToParent(*this, parent);
	}
}

void Node::detachedFromParent(Node& parent) {
	A3D_LOG_T("parent: {:p}", static_cast<void*>(&parent));

	checkNotifyPhysicsBodyOfUnreachablePhysicalWorld();

//	if (_physicsBody) {
//		_physicsBody->nodeDetachedFromParent(parent);
//	}

	for (auto& child : _children) {
		child->ancestorDetachedFromParent(*this, parent);
	}

	_parent = {};
}

void Node::attachedToScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

//	if (_physicsBody) {
//		_physicsBody->nodeAttachedToScene(scene);
//	}

	_scene = &scene;

	checkNotifyPhysicsBodyOfReachablePhysicalWorld();

	for (auto& child : _children) {
		child->ancestorAttachedToScene(*this, scene);
	}
}

void Node::detachedFromScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

//	if (_physicsBody) {
//		_physicsBody->nodeDetachedFromScene(scene);
//	}

	checkNotifyPhysicsBodyOfUnreachablePhysicalWorld();

	for (auto& child : _children) {
		child->ancestorDetachedFromScene(*this, scene);
	}

	_scene = nullptr;
}

void Node::ancestorAttachedToParent(Node& ancestor, Node& parent) {
	A3D_LOG_T("ancestor: {:p}, parent: {:p}",
			  static_cast<void*>(&ancestor),
			  static_cast<void*>(&parent));

//	if (_physicsBody) {
//		_physicsBody->ancestorAttachedToParent(ancestor, parent);
//	}

	checkNotifyPhysicsBodyOfReachablePhysicalWorld();

	for (auto& child : _children) {
		child->ancestorAttachedToParent(ancestor, parent);
	}
}

void Node::ancestorDetachedFromParent(Node& ancestor, Node& parent) {
	A3D_LOG_T("ancestor: {:p}, parent: {:p}",
			  static_cast<void*>(&ancestor),
			  static_cast<void*>(&parent));

//	if (_physicsBody) {
//		_physicsBody->ancestorDetachedFromParent(ancestor, parent);
//	}

	checkNotifyPhysicsBodyOfUnreachablePhysicalWorld();

	for (auto& child : _children) {
		child->ancestorDetachedFromParent(ancestor, parent);
	}
}

void Node::ancestorAttachedToScene(Node& ancestor, Scene& scene) {
	A3D_LOG_T("ancestor: {:p}, scene: {:p}",
			  static_cast<void*>(&ancestor),
			  static_cast<void*>(&scene));

//	if (_physicsBody) {
//		_physicsBody->ancestorAttachedToScene(ancestor, scene);
//	}

	checkNotifyPhysicsBodyOfReachablePhysicalWorld();

	for (auto& child : _children) {
		child->ancestorAttachedToScene(ancestor, scene);
	}
}

void Node::ancestorDetachedFromScene(Node& ancestor, Scene& scene) {
	A3D_LOG_T("ancestor: {:p}, scene: {:p}",
			  static_cast<void*>(&ancestor),
			  static_cast<void*>(&scene));

//	if (_physicsBody) {
//		_physicsBody->ancestorDetachedFromScene(ancestor, scene);
//	}

	checkNotifyPhysicsBodyOfUnreachablePhysicalWorld();

	for (auto& child : _children) {
		child->ancestorDetachedFromScene(ancestor, scene);
	}
}

void Node::visualWorldAttachedToScene(VisualWorld& world, Scene& scene) {
	A3D_LOG_T("world: {:p}, scene: {:p}", static_cast<void*>(&world), static_cast<void*>(&scene));

	for (auto& child : _children) {
		child->visualWorldAttachedToScene(world, scene);
	}
}

void Node::visualWorldDetachedFromScene(VisualWorld& world, Scene& scene) {
	A3D_LOG_T("world: {:p}, scene: {:p}", static_cast<void*>(&world), static_cast<void*>(&scene));

	for (auto& child : _children) {
		child->visualWorldDetachedFromScene(world, scene);
	}
}

void Node::physicalWorldAttachedToScene(PhysicalWorld& world, Scene& scene) {
	A3D_LOG_T("world: {:p}, scene: {:p}", static_cast<void*>(&world), static_cast<void*>(&scene));

//	if (_physicsBody) {
//		_physicsBody->physicalWorldAttachedToScene(world, scene);
//	}

	checkNotifyPhysicsBodyOfReachablePhysicalWorld();

	for (auto& child : _children) {
		child->physicalWorldAttachedToScene(world, scene);
	}
}

void Node::physicalWorldDetachedFromScene(PhysicalWorld& world, Scene& scene) {
	A3D_LOG_T("world: {:p}, scene: {:p}", static_cast<void*>(&world), static_cast<void*>(&scene));

//	if (_physicsBody) {
//		_physicsBody->physicalWorldDetachedFromScene(world, scene);
//	}

	checkNotifyPhysicsBodyOfUnreachablePhysicalWorld();

	for (auto& child : _children) {
		child->physicalWorldDetachedFromScene(world, scene);
	}
}

VisualWorld* Node::visualWorld() const {

	if (auto scene = Node::scene(); scene) {
		if (auto visualWorld = scene->visualWorld(); visualWorld) {
			return visualWorld;
		}
	}
	return nullptr;
}

PhysicalWorld* Node::physicalWorld() const {

	if (auto scene = Node::scene(); scene) {
		if (auto physicalWorld = scene->physicalWorld(); physicalWorld) {
			return physicalWorld;
		}
	}
	return nullptr;
}

void Node::checkNotifyPhysicsBodyOfReachablePhysicalWorld() const {

	if (_physicsBody) {
		if (auto physicalWorld = Node::physicalWorld()) {
			_physicsBody->physicalWorldReachable(*physicalWorld);
		}
	}
}

void Node::checkNotifyPhysicsBodyOfUnreachablePhysicalWorld() const {
	// called just before something in the upward path to the PhysicalWorld is broken.
	// so if we HAVE a path to PhysicalWorld now, we won't mush longer.

	if (_physicsBody) {
		if (auto physicalWorld = Node::physicalWorld()) {
			_physicsBody->physicalWorldUnreachable(*physicalWorld);
		}
	}
}

bool Node::containsChild(const shared_ptr<Node>& node) {

	auto nodes = children(true);
	if (find(nodes.begin(), nodes.end(), node) != nodes.end()) {
		return true;
	}
	return false;
}

AABB Node::aabb() {

	static const float maxFloat = numeric_limits<float>::max();
	static const float minFloat = numeric_limits<float>::min();

	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	getAABBRec(aabb);

	if (aabb.min.x == maxFloat
		|| aabb.min.y == maxFloat
		|| aabb.min.z == maxFloat
		|| aabb.max.x == minFloat
		|| aabb.max.y == minFloat
		|| aabb.max.z == minFloat) {
		static AABB zeroAABB = { {0, 0, 0},
								 {0, 0, 0} };
		return zeroAABB;
	}

	return aabb;
}

vec3 Node::extent() {
	auto aabb = Node::aabb();
	return {aabb.max.x - aabb.min.x,
			aabb.max.y - aabb.min.y,
			aabb.max.z - aabb.min.z};
}

void Node::applyPhysicsTransform(const mat4& transform) {

	if (auto parent = _parent.lock()) {
		this->transform(inverse(parent->worldTransform()) * transform);
	}
	else {
		this->transform(transform);
	}
}

void Node::draw(Renderer& renderer,
				const RenderContext& context,
				const mat4& viewMat,
				const mat4& projectionMat,
				const DebugOptions& debugOptions,
				std::vector<Node*>& lightNodes,
				FrameStats& stats) {

	++stats.numNodes;

	if (!_hidden) {

		if (_light) {
			lightNodes.push_back(this);
		}

		if (_mesh) {

			_mesh->draw(renderer,
						context,
						worldTransform(),
						viewMat,
						projectionMat,
						debugOptions,
						stats);
		}
	}

	for (auto& child : _children) {
		child->draw(renderer,
					context,
					viewMat,
					projectionMat,
					debugOptions,
					lightNodes,
					stats);
	}
}

void Node::_debugPrint() {

	int level = 0;
	_debugPrintRec(*this, level);
}

void Node::_debugPrintRec(Node& node,
						  unsigned level) {

	A3D_LOG_I("[{}] {}", level, *node.name());

	for (auto& child : node._children) {
		_debugPrintRec(*child, level + 1);
	}
}

/// Private Member Functions ///

void Node::getAABBRec(AABB& aabb) {

	if (_mesh) {
		auto geoAABB = _mesh->aabb(this);
		aabb.min.x = std::min(aabb.min.x, geoAABB.min.x);
		aabb.max.x = std::max(aabb.max.x, geoAABB.max.x);
		aabb.min.y = std::min(aabb.min.y, geoAABB.min.y);
		aabb.max.y = std::max(aabb.max.y, geoAABB.max.y);
		aabb.min.z = std::min(aabb.min.z, geoAABB.min.z);
		aabb.max.z = std::max(aabb.max.z, geoAABB.max.z);
	}

	for (auto& child : _children) {
		child->getAABBRec(aabb);
	}
}

vector<shared_ptr<Node>> Node::children(const Node& root) const {

	auto children = vector<shared_ptr<Node>>();

	for (auto& child : root._children) {
		childrenRec(child, children);
	}

	return children;
}

void Node::childrenRec(const shared_ptr<Node>& node,
					   vector<shared_ptr<Node>>& children) const {

	children.push_back(node);

	for (auto& child : node->_children) {
		childrenRec(child, children);
	}
}

NodeDirtyMask Node::dirtyMask() const {
	return _dirtyMask;
}

void Node::dirtyMask(NodeDirtyMask mask) {
	_dirtyMask = mask;
}
