//
//  Node.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "scene/Node.h"

#include <algorithm>

// apparently we're not using anything experimental here since at least GLM .9.9.8
//#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/quaternion.hpp"

#include "diagnostic/Exception.h"
#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsShape.h"
#include "rendering/Light.h"
#include "rendering/camera/Camera.h"


using namespace ae;
using namespace std;
using namespace glm;


//#define ALTERNATE_EULERS

/*********************************************************************************************
	Pulic Static
 *********************************************************************************************/

shared_ptr<Node> Node::NamedNode(std::string name) {
	auto node = make_shared<Node>();
	node->name(name);
	return node;
}

shared_ptr<Node> Node::GeometryNode(shared_ptr<Geometry> geometry) {
	auto node = make_shared<Node>();
	node->geometry(geometry);
	return node;
}

shared_ptr<Node> Node::LightNode(shared_ptr<Light> light) {
	auto node = make_shared<Node>();
	node->light(light);
	return node;
}

shared_ptr<Node> Node::CameraNode(shared_ptr<Camera> camera) {
	auto node = make_shared<Node>();
	node->camera(camera);
	return node;
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Node::Node():
		_name(std::nullopt),
		_hidden(false),
		_camera(nullptr),
		_light(nullptr),
		_geometry(nullptr),
		_position({0.0f, 0.0f, 0.0f}),
		_orientation(quat()),
		_scale({1.0f, 1.0f, 1.0f}),
		_worldTransform(mat4(1.0f)),
		_physicsBody(nullptr),
		_parent({}),
//	_scene({}),
	_dirtyMask(NODE_DIRTY_MASK::NONE) {

}

Node::Node(const string& name):
	Node() {

		_name = name;
}

Node::~Node() {
	AE_LOG_D("Destroying Node {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

std::optional<std::string> Node::name() const {
	return _name;
}

void Node::name(const string& name) {
	_name = name;
}

shared_ptr<Light> Node::light() const {
	return _light;
}

void Node::light(const shared_ptr<Light> light) {
	//light->attachedToNode(shared_from_this());
	_light = light;
}

shared_ptr<Camera> Node::camera() const {
	return _camera;
}

void Node::camera(const shared_ptr<Camera> camera) {
//	camera->attachedToNode(shared_from_this());
	_camera = camera;
}

shared_ptr<Geometry> Node::geometry() const {
	return _geometry;
}

void Node::geometry(const shared_ptr<Geometry> geometry) {
	_geometry = geometry;
//	geometry->attachedToNode(shared_from_this());
	if (_physicsBody) {
		_physicsBody->geometryAttachedToNode(geometry);
	}
}

bool Node::hidden() const {
	return _hidden;
}

void Node::hidden(const bool hidden) {
	_hidden = hidden;
}

vec3 Node::position() const {
	return _position;
}

void Node::position(const vec3& position) {
	_position = position;

	addDirtyMaskRecursive(NODE_DIRTY_MASK::WORLD_TRANSFORM);
}

vec4 Node::rotation() const {

	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/

	/*
		angle = 2 * acos(qw)
		x = qx / sqrt(1-qw*qw)
		y = qy / sqrt(1-qw*qw)
		z = qz / sqrt(1-qw*qw)
	 */

	// WORKS (but clips rotation to 2PI)
	vec4 angleAxis = vec4(_orientation.x / sqrt(1-_orientation.w*_orientation.w),
						  _orientation.y / sqrt(1-_orientation.w*_orientation.w),
						  _orientation.z / sqrt(1-_orientation.w*_orientation.w),
						  2 * acos(_orientation.w));

	return angleAxis;


	// doesn't really work at all, surprisingly
//	mat4 rotMat = mat4_cast(_orientation);
//	vec3 axis;
//	float angle;
//	axisAngle(rotMat, axis, angle);
//	return vec4(axis.x, axis.y, axis.z, angle);
}

void Node::rotation(const vec3& axis, float angle) {
	
//	if (_physicsBody && _physicsBody->type() == PHYSICS_BODY_TYPE::STATIC) {
//		throw Exception("Can't manipulate )
//	}
//	else {

	// these methods produce the same results. perhaps GLM is faster...
	
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/

	/*
		qx = ax * sin(angle/2)
		qy = ay * sin(angle/2)
		qz = az * sin(angle/2)
		qw = cos(angle/2)

		where:

		the axis is normalised so: ax*ax + ay*ay + az*az = 1
		the quaternion is also normalised so cos(angle/2)2 + ax*ax * sin(angle/2)2 + ay*ay * sin(angle/2)2+ az*az * sin(angle/2)2 = 1
	 */

//		float qx = rotation.x * sin(rotation.w/2.0f);
//		float qy = rotation.y * sin(rotation.w/2.0f);
//		float qz = rotation.z * sin(rotation.w/2.0f);
//		float qw = cos(rotation.w/2.0f);
//
//		_orientation = quat(qw, qx, qy, qz);



	vec3 axisNormalized = normalize(vec3(axis.x, axis.y, axis.z));
	_orientation = angleAxis(angle, axisNormalized);

	addDirtyMaskRecursive(NODE_DIRTY_MASK::WORLD_TRANSFORM);
//	}
}

vec3 Node::eulerAngles() const {  // pitch, yaw, roll

	// !? https://glm.g-truc.net/0.9.0/api/a00151.html

	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	// https://download.tuxfamily.org/arakhne/apidocs/afc/org/arakhne/afc/math/geometry/d3/doc-files/euler_plane.gif
	// note that the linked equation seems to have switched attitude and bank

#ifndef ALTERNATE_EULERS

	// works great, but appears to be ZXY order.
	// different ordering? http://graphics.wikia.com/wiki/Conversion_between_quaternions_and_Euler_angles

	auto q = _orientation;

	float pitch = atan2(2.0f*q.x*q.w - 2.0f*q.y*q.z, 1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z);
	float yaw = atan2(2.0f*q.y*q.w - 2.0f*q.x*q.z, 1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z);
	float roll = asin(2*q.x*q.y + 2.0f*q.z*q.w);

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
	//return glm::eulerAngles(_orientation);
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
	float c1 = cos(yaw / 2.0f);
	float c2 = cos(roll / 2.0f);
	float c3 = cos(pitch / 2.0f);
	float s1 = sin(yaw / 2.0f);
	float s2 = sin(roll / 2.0f);
	float s3 = sin(pitch / 2.0f);

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

	addDirtyMaskRecursive(NODE_DIRTY_MASK::WORLD_TRANSFORM);
}

quat Node::orientation() const {
	return _orientation;
}

void Node::orientation(const quat& orientation) {
	_orientation = orientation;

	addDirtyMaskRecursive(NODE_DIRTY_MASK::WORLD_TRANSFORM);
}

vec3 Node::scale() const {
	return _scale;
}

void Node::scale(const glm::vec3& scale) {
	
	//checkPhysicsScale(_scale, scale);
	
	_scale = scale;

	addDirtyMaskRecursive(NODE_DIRTY_MASK::WORLD_TRANSFORM);
}

mat4 Node::transform() const {

	mat4 t = translate(mat4(1.0), _position);
	mat4 r = mat4_cast(_orientation);
	mat4 s = glm::scale(mat4(1.0), _scale);
	
	return t * r * s;
}

void Node::transform(const mat4& transform) {
	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;
	
	decompose(transform,
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);
	
	// checkPhysicsScale(_scale, scale);

	_position = translation;
	_scale = scale;

	// https://stackoverflow.com/questions/17918033/glm-decompose-mat4-into-translation-and-rotation
	// "Keep in mind that the resulting quaternion in not correct. It returns its conjugate!
	//
	//To fix this add this to your code:
	//
	//rotation=glm::conjugate(rotation);"
	_orientation = orientation;

	addDirtyMaskRecursive(NODE_DIRTY_MASK::WORLD_TRANSFORM);
}

vec3 Node::worldPosition() {
	auto world = worldTransform();	
	return vec3(world[3][0], world[3][1], world[3][2]);
}

vec4 Node::worldRotation() {
	throw Exception("worldRotation() not implemented.");
	return vec4(0.0, 0.0, 0.0, 0.0);
}

vec3 Node::worldEulerAngles() {
	throw Exception("worldEulerAngles() not implemented.");
	return vec3(0.0, 0.0, 0.0);
}

quat Node::worldOrientation() {
	auto world = worldTransform();
	
	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;
	
	decompose(world,
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);
	
	return orientation;
}

vec3 Node::worldScale() {
	auto world = worldTransform();
	
	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;
	
	decompose(world,
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);
	
	return scale;
}

vec3 Node::worldForward() {
	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;

	decompose(worldTransform(),
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);

	mat4 rotationMat = mat4_cast(orientation);

	return normalize(rotationMat * vec4(0, 0, -1, 1));
}

vec3 Node::worldUp() {
	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;

	decompose(worldTransform(),
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);

	mat4 rotationMat = mat4_cast(orientation);

	return normalize(rotationMat * vec4(0, 1, 0, 1));
}

vec3 Node::worldRight() {
	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;

	decompose(worldTransform(),
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);

	mat4 rotationMat = mat4_cast(orientation);

	return normalize(rotationMat * vec4(1, 0, 0, 1));
}

mat4 Node::worldTransform() {

	if (NODE_DIRTY_MASK_CONTAINS(_dirtyMask, NODE_DIRTY_MASK::WORLD_TRANSFORM)) {

		//AE_LOG_I("DIRTY UPDATING WORLD");

		auto t = mat4(1.0f);
		auto path = pathToRoot();
		
		auto iter = path.end();
		while (iter != path.begin()) {
			--iter;
			shared_ptr<Node> node = *iter;
			t = t * node->transform();
		}
		
		_worldTransform = t * transform();

		_dirtyMask = NODE_DIRTY_MASK_REMOVE(_dirtyMask, NODE_DIRTY_MASK::WORLD_TRANSFORM);
	}
	
	return _worldTransform;
}

void Node::addChildren(vector<shared_ptr<Node>> nodes) {
	for (auto node: nodes) {
		addChild(node);
	}
}

void Node::addChild(shared_ptr<Node> node) {

	if (containsChild(node)) {
		throw Exception("Node already exists in tree.");
	}
	
	node->attachedToParent(weak_from_this());
	_children.push_back(node);
}

void Node::insertChild(const Node& node, int index) {
	// see notes about Node already existing here/elsewhere in addChildNode()
}

void Node::removeFromParent() {
	if (auto parent = _parent.lock()) {
		// https://stackoverflow.com/questions/39912/how-do-i-remove-an-item-from-a-stl-vector-with-a-certain-value
		// https://stackoverflow.com/questions/3385229/c-erase-vector-element-by-value-rather-than-by-position
		// http://en.cppreference.com/w/cpp/algorithm/remove
		auto vec = parent->_children;
		vec.erase(remove(vec.begin(), vec.end(), shared_from_this()), vec.end());
// TODO: can we avoid the copy?
		parent->_children = vec;
	}
}

void Node::replaceChild(const Node& replace, const Node& with) {
	
}

weak_ptr<Node> Node::parent() const {
	return _parent;
}

vector<shared_ptr<Node>> Node::children(bool resursive) {
	// if !resursive, returns immediate children in no particular order
	// if resursive, returns all descendants in topological order

	if (resursive) {
		return preorderChildren(shared_from_this());
	}
	else {
		return _children;
	}
}

shared_ptr<Node> Node::child(const string& name, bool resursive) {
	for (auto child : children(resursive)) {
		if (child->name() == name) return child;
	}
	return nullptr;
}

shared_ptr<PhysicsBody> Node::physicsBody() const {
	return _physicsBody;
}

void Node::physicsBody(shared_ptr<PhysicsBody> body) {
	_physicsBody = body;
	body->attachedToNode(shared_from_this());
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void Node::unrollWorldTransform(mat4 transform) {
	// used for physics simulation to update local transform relative to parent
	
	//this->transform(transform * inverse(_parent.lock()->worldTransform()));
	this->transform(inverse(_parent.lock()->worldTransform()) * transform);
}

//void Node::updateWorldTransform() {
//	// gets called by Scene each frame.
//	// before being called a topological sort if done on the scene, and each node's
//	// updateWorldTransform() is called in order, guaranteeing that its parent's
//	// world tranform is indeed a valid world transform
//
//	if (NODE_DIRTY_MASK_CONTAINS(_dirtyMask, NODE_DIRTY_MASK::WORLD_TRANSFORM)) {
//		if (auto p = parent().lock()) {
//			//auto oldScale = scale();
//			_worldTransform = p->worldTransform() * transform();
//			//auto newScale = scale();
//			//checkPhysicsScale(oldScale, newScale);
//		}
//
//		_dirtyMask = NODE_DIRTY_MASK_REMOVE(_dirtyMask, NODE_DIRTY_MASK::WORLD_TRANSFORM);
//	}
//}

//void Node::updateWorldTransform(mat4& parentWorldTransform) {
//
//	if (NODE_DIRTY_MASK_CONTAINS(_dirtyMask, NODE_DIRTY_MASK::WORLD_TRANSFORM)) {
//		_worldTransform = parentWorldTransform * transform();
//		_dirtyMask = NODE_DIRTY_MASK_REMOVE(_dirtyMask, NODE_DIRTY_MASK::WORLD_TRANSFORM);
//	}
//}

bool Node::containsChild(shared_ptr<Node> node) {
//	auto top = root();
//	if (!top) top = shared_from_this();
//	auto sceneNodes = top->children(true);
//	if (find(sceneNodes.begin(), sceneNodes.end(), node) != sceneNodes.end()) {
//		return true;
//	}
//	return false;
	
	auto nodes = children(true);
	if (find(nodes.begin(), nodes.end(), node) != nodes.end()) {
		return true;
	}
	return false;
}

//void Node::attachedToScene(shared_ptr<Scene> scene) {
////	if (!root()) {
////		_scene = scene;
////	}
////	else {
////		for (auto child : children(true)) {
////			child->attachedToScene(scene); // just in case they want to do something with it
////		}
////	}
//}

void Node::attachedToParent(weak_ptr<Node> parent) {
	_parent = parent;
}

//shared_ptr<Node> Node::root() const {
//	auto path = pathToRoot();
//	if (path.size() > 0) {
//		return path.back();
//	}
//	return nullptr;
//}

//weak_ptr<Scene> Node::scene() const {
//	if (root()) {
//		return root()->scene();
//	}
//	else {
//		return _scene;
//	}
//}

//weak_ptr<Node> Node::model() const {
//	return _model;
//}
//
//void Node::attachedToModel(shared_ptr<Node> model) {
//	_model = model;
//}

void Node::update(PhysicsSimulator& simulator,
				  RenderStats& stats,
				  //shared_ptr<Node> parentNode,
				  map<shared_ptr<Node>, bool>& visited) {

	if (!visited[shared_from_this()]) {

		// - world transform dirty? -> update

		//updateWorldTransform(parentWorldTransform);
		if (NODE_DIRTY_MASK_CONTAINS(_dirtyMask, NODE_DIRTY_MASK::WORLD_TRANSFORM)) {
			//AE_LOG_I("UPDATING WORLD in update()");

			static const auto mat4Identity = mat4(1.0);
			auto parentNode = _parent.lock();
			mat4 parentWorldTransform = (parentNode
										 ? parentNode->worldTransform()
										 : mat4Identity);

			_worldTransform = parentWorldTransform * transform();
			_dirtyMask = NODE_DIRTY_MASK_REMOVE(_dirtyMask, NODE_DIRTY_MASK::WORLD_TRANSFORM);
		}

		// physics body or physics body dirty?
		//		create/update

		if (_physicsBody) {
			_physicsBody->update(simulator,
								 stats);
		}

		// apply visual to kinematic bodies (and static?)

		for (auto& child : _children) {
			child->update(simulator,
						  stats,
						  visited);
		}

		++stats.nodes;
	}
}

void Node::sync(PhysicsSimulator& simulator,
				RenderStats& stats,
				//shared_ptr<Node> parentNode,
				map<shared_ptr<Node>, bool>& visited) {

	if (!visited[shared_from_this()]) {

		// apply physics model to visual

		if (_physicsBody) {
			_physicsBody->sync(simulator,
							   *this,
							   stats);
		}

		for (auto& child : _children) {
			child->sync(simulator,
						stats,
						visited);
		}
	}
}

void Node::draw(Renderer& renderer,
				const mat4& viewMat,
				const mat4& projectionMat,
				const DEBUG_OPTIONS& debugOptions,
				RenderStats& stats,
				//shared_ptr<Node> parentNode,
				map<shared_ptr<Node>, bool>& visited) {

	if (!visited[shared_from_this()]) {

		if (_geometry && !_hidden) {
			_geometry->draw(renderer,
							_worldTransform,
							viewMat,
							projectionMat,
							debugOptions,
							stats);
		}

		for (auto& child : _children) {
			child->draw(renderer,
						viewMat,
						projectionMat,
						debugOptions,
						stats,
						visited);
		}
	}
}

void Node::_printPreorder() {

	int level = 0;
	auto visited = map<shared_ptr<Node>, bool>();

	// don't include the root
	//visited[shared_from_this()] = true;
	for (auto child : children(false)) {
		_printPreorderRec(child, level, visited);
	}
}

void Node::_printPreorderRec(shared_ptr<Node> node,
							 int level,
							 map<shared_ptr<Node>, bool>& visited) {

	if (!visited[node]) {

		AE_LOG_I("[{}] {}", level, *node->name());

		visited[node] = true;

		for (auto child : node->_children) {
			_printPreorderRec(child, level+1, visited);
		}
	}
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

vector<shared_ptr<Node>> Node::pathToRoot() const {
	// walks up the tree to the root node, returning a vector containing the nodes in ascending order
	
	auto parents = vector<shared_ptr<Node>>();
	
	if (auto p = _parent.lock()) {
		if (auto p = _parent.lock()) {
			do {
				parents.push_back(p);
				p = p->parent().lock();
			} while (p != nullptr);
		}
	}
	
	return parents;
}

void Node::addDirtyMaskRecursive(NODE_DIRTY_MASK bits) {

	_dirtyMask = NODE_DIRTY_MASK_ADD(_dirtyMask, bits);
	
	for (auto& c : children(true)) {
		c->_dirtyMask = NODE_DIRTY_MASK_ADD(c->_dirtyMask, bits);
	}
}

vector<shared_ptr<Node>> Node::preorderChildren(shared_ptr<Node> root) {
	
	auto visited = map<shared_ptr<Node>, bool>();
	auto stack = std::stack<shared_ptr<Node>>();

	// don't include the root
	//preorderChildrenRec(root, visited, stack);
	for (auto child : root->children(false)) {
		preorderChildrenRec(child, visited, stack);
	}
	
	// probably a better way to do this
	auto vec = vector<shared_ptr<Node>>();
	vec.reserve(stack.size());
	while (!stack.empty()) {
		vec.emplace_back(stack.top());
		stack.pop();
	}
	return vec;
}

void Node::preorderChildrenRec(shared_ptr<Node> node,
							   map<shared_ptr<Node>, bool> &visited,
							   stack<shared_ptr<Node>> &stack) {
	
	visited[node] = true;
	
	for (auto child : node->_children) {
		if (!visited[child]) {
			preorderChildrenRec(child, visited, stack);
		}
	}

	stack.push(node);
}

//void Node::checkPhysicsScale(const glm::vec3& oldScale, const glm::vec3& newScale) {
//	// check if the physics shape needs to be scaled
//
//	if (_physicsBody && _physicsBody->shape()) {
//		if (!Equal(oldScale, newScale)) {
//			auto shape = _physicsBody->shape();
//			shape->dirtyMask(PHYSICS_SHAPE_DIRTY_MASK_ADD(shape->dirtyMask(),
//														  PHYSICS_SHAPE_DIRTY_MASK::SCALE));
//		}
//	}
//}

NODE_DIRTY_MASK Node::dirtyMask() const {
	return _dirtyMask;
}

void Node::dirtyMask(NODE_DIRTY_MASK mask) {
	_dirtyMask = mask;
}
