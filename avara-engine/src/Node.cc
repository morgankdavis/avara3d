//
//  Node.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Node.h"

#include <algorithm>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"
#include "Exception.h"
#include "Geometry.h"
#include "Light.h"
#include "Logger.h"
#include "PhysicsBody.h"
#include "PhysicsShape.h"
#include "Scene.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;
using namespace glm;


//#define ALTERNATE_EULERS


/***************************************************************************************
     Public Static
 ***************************************************************************************/

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

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

Node::Node():
	m_name(boost::none),
	m_hidden(false),
	m_camera(nullptr),
	m_light(nullptr),
	m_geometry(nullptr),
	m_position({0.0f, 0.0f, 0.0f}),
	m_orientation(quat()),
	m_scale({1.0f, 1.0f, 1.0f}),
	m_worldTransform(mat4(1.0f)),
	m_physicsBody(nullptr),
	m_parent({}),
//	m_scene({}),
	m_dirtyBits(NODE_DIRTY_BITS::ALL) {

}

Node::Node(const string& name):
	Node() {

		m_name = name;
}

Node::~Node() {
	AE_LOG_D("Destroying Node {:p}", (void*)this);
}

/***************************************************************************************
     Public
 ***************************************************************************************/

boost::optional<std::string> Node::name() const {
	return m_name;
}

void Node::name(const string& name) {
	m_name = name;
}

shared_ptr<Light> Node::light() const {
	return m_light;
}

void Node::light(const shared_ptr<Light> light) {
	light->attachedToNode(shared_from_this());
	m_light = light;
}

shared_ptr<Camera> Node::camera() const {
	return m_camera;
}

void Node::camera(const shared_ptr<Camera> camera) {
	camera->attachedToNode(shared_from_this());
	m_camera = camera;
}

shared_ptr<Geometry> Node::geometry() const {
	return m_geometry;
}

void Node::geometry(const shared_ptr<Geometry> geometry) {
	if (geometry) {
		geometry->attachedToNode(shared_from_this());
		if (m_physicsBody) {
			m_physicsBody->geometryAttachedToNode(geometry);
		}
	}
	m_geometry = geometry;
}

bool Node::hidden() const {
	return m_hidden;
}

void Node::hidden(const bool hidden) {
	m_hidden = hidden;
}

vec3 Node::position() const {
	return m_position;
}

void Node::position(const vec3 position) {
	m_position = position;
	
	addDirtyBitsRecursive(NODE_DIRTY_BITS::WORLD_TRANSFORM);
}

vec4 Node::rotation() const {

	//http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/

	/*
		angle = 2 * acos(qw)
		x = qx / sqrt(1-qw*qw)
		y = qy / sqrt(1-qw*qw)
		z = qz / sqrt(1-qw*qw)
	 */

	// WORKS (but clips rotation to 2PI)
	vec4 angleAxis = vec4(m_orientation.x / sqrt(1-m_orientation.w*m_orientation.w),
						  m_orientation.y / sqrt(1-m_orientation.w*m_orientation.w),
						  m_orientation.z / sqrt(1-m_orientation.w*m_orientation.w),
						  2 * acos(m_orientation.w));

	return angleAxis;


	// doesn't really work at all, surprisingly
//	mat4 rotMat = mat4_cast(m_orientation);
//	vec3 axis;
//	float angle;
//	axisAngle(rotMat, axis, angle);
//	return vec4(axis.x, axis.y, axis.z, angle);
}

void Node::rotation(const vec4 rotation) {
	
//	if (m_physicsBody && m_physicsBody->type() == PHYSICS_BODY_TYPE::STATIC) {
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
//		m_orientation = quat(qw, qx, qy, qz);



	vec3 axisNormalized = normalize(vec3(rotation.x, rotation.y, rotation.z));
	float angle = rotation.w;
	m_orientation = angleAxis(angle, axisNormalized);
	
	addDirtyBitsRecursive(NODE_DIRTY_BITS::WORLD_TRANSFORM);
//	}
}

vec3 Node::eulerAngles() const {  // pitch, yaw, roll

	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	// https://download.tuxfamily.org/arakhne/apidocs/afc/org/arakhne/afc/math/geometry/d3/doc-files/euler_plane.gif
	// note that the linked equation seems to have switched attitude and bank

#ifndef ALTERNATE_EULERS

	// works great, but appears to be ZXY order.
	// different ordering? http://graphics.wikia.com/wiki/Conversion_between_quaternions_and_Euler_angles

	auto q = m_orientation;

	float pitch = atan2(2.0f*q.x*q.w - 2.0f*q.y*q.z, 1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z);
	float yaw = atan2(2.0f*q.y*q.w - 2.0f*q.x*q.z, 1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z);
	float roll = asin(2*q.x*q.y + 2.0f*q.z*q.w);

	return vec3(pitch, yaw, roll);
	
#else
	// http://bediyap.com/programming/convert-quaternion-to-euler-rotations/

	auto q = m_orientation;
	vec3 res = vec3(0.0f, 0.0f, 0.0f);
	res.x = atan2(2*(q.y*q.z + q.w*q.x), q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
	res.y = asin(-2*(q.x*q.z - q.w*q.y));
	res.z = atan2(2*(q.x*q.y + q.w*q.z), q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z);
	
	return res;
#endif

	// clips to +-180
	// return glm::eulerAngles(m_orientation);
}

void Node::eulerAngles(const vec3 eulerAngles) { // pitch, yaw, roll

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

	m_orientation = quat(w, x, y, z);
	
#else

	// https://gamedev.stackexchange.com/questions/13436/glm-euler-angles-to-quaternion



//	float sx = sin(eulerAngles.x/2.0), sy = sin(eulerAngles.y/2.0), sz = sin(eulerAngles.z/2.0),
//	cx = cos(eulerAngles.x/2.0), cy = cos(eulerAngles.y/2.0), cz = cos(eulerAngles.z/2.0);
//
//	m_orientation = normalize(quat( cx*cy*cz + sx*sy*sz,
//	   sx*cy*cz - cx*sy*sz,
//	   cx*sy*cz + sx*cy*sz,
//	   cx*cy*sz - sx*sy*cz )); // for XYZ application order



	//m_orientation = toQuat( orientate3( eulerAngles ) );

	//m_orientation = toQuat( yawPitchRoll( eulerAngles.y, eulerAngles.x, eulerAngles.z ) );


//	// https://www.opengl.org/discussion_boards/showthread.php/174858-GLM-Initializing-Quaternion-with-Eular-XYZ
//	quat quatAroundX = angleAxis( eulerAngles.x, vec3(1.0,0.0,0.0) );
//	quat quatAroundY = angleAxis( eulerAngles.y, vec3(0.0,1.0,0.0) );
//	quat quatAroundZ = angleAxis( eulerAngles.z, vec3(0.0,0.0,1.0) );
//	//quat finalOrientation = normalize(quatAroundX * quatAroundY * quatAroundZ);
//	quat finalOrientation = quatAroundZ * quatAroundY * quatAroundX;
//	m_orientation = finalOrientation;




	//m_orientation = quat(eulerAngles); // WOW this works, but still acts strange after 180


	//return;
	
	auto rotationX = rotate(mat4(1.0f), eulerAngles.x, vec3(1.0f, 0.0f, 0.0f));
	auto rotationY = rotate(mat4(1.0f), eulerAngles.y, vec3(0.0f, 1.0f, 0.0f));
	auto rotationZ = rotate(mat4(1.0f), eulerAngles.z, vec3(0.0f, 0.0f, 1.0f));

	//m_orientation = normalize(quat_cast(rotationZ * rotationX * rotationY)); // equation above order
	m_orientation = normalize(quat_cast(rotationZ * rotationY * rotationX)); // SceneKit order
#endif
	
	addDirtyBitsRecursive(NODE_DIRTY_BITS::WORLD_TRANSFORM);
}

quat Node::orientation() const {
	return m_orientation;
}

void Node::orientation(const quat orientation) {
	m_orientation = orientation;
	
	addDirtyBitsRecursive(NODE_DIRTY_BITS::WORLD_TRANSFORM);
}

vec3 Node::scale() const {
	return m_scale;
}

void Node::scale(const glm::vec3 scale) {
	
	//checkPhysicsScale(m_scale, scale);
	
	m_scale = scale;
	
	addDirtyBitsRecursive(NODE_DIRTY_BITS::WORLD_TRANSFORM);
}

mat4 Node::transform() const {

	mat4 t = translate(mat4(1.0), m_position);
	mat4 r = mat4_cast(m_orientation);
	mat4 s = glm::scale(mat4(1.0), m_scale);
	
	return t * r * s;
}

void Node::transform(const mat4 transform) {
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
	
	//checkPhysicsScale(m_scale, scale);

	m_position = translation;
	m_scale = scale;
	m_orientation = orientation;
	
	addDirtyBitsRecursive(NODE_DIRTY_BITS::WORLD_TRANSFORM);
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

	if (NODE_DIRTY_BITS_CONTAINS(m_dirtyBits, NODE_DIRTY_BITS::WORLD_TRANSFORM)) {
		
		auto t = mat4(1.0f);
		auto path = pathToRoot();
		
		auto iter = path.end();
		while (iter != path.begin()) {
			--iter;
			shared_ptr<Node> node = *iter;
			t = t * node->transform();
		}
		
		m_worldTransform = t * transform();
		
		m_dirtyBits = NODE_DIRTY_BITS_REMOVE(m_dirtyBits, NODE_DIRTY_BITS::WORLD_TRANSFORM);
	}
	
	return m_worldTransform;
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
	
	node->attachedToParent(shared_from_this());
	m_children.push_back(node);
}

void Node::insertChild(const Node& node, int index) {
	// see notes about Node already existing here/elsewhere in addChildNode()
}

void Node::removeFromParent() {
	if (auto parent = m_parent.lock()) {
		// https://stackoverflow.com/questions/39912/how-do-i-remove-an-item-from-a-stl-vector-with-a-certain-value
		// https://stackoverflow.com/questions/3385229/c-erase-vector-element-by-value-rather-than-by-position
		// http://en.cppreference.com/w/cpp/algorithm/remove
		auto vec = parent->m_children;
		vec.erase(remove(vec.begin(), vec.end(), shared_from_this()), vec.end());
// TODO: can we avoid the copy?
		parent->m_children = vec;
	}
}

void Node::replaceChild(const Node& replace, const Node& with) {
	
}

weak_ptr<Node> Node::parent() const {
	return m_parent;
}

vector<shared_ptr<Node>> Node::children(bool resursive) {
	// if !resursive, returns immediate children in no particular order
	// if resursive, returns all descendants in topological order

	if (resursive) {
		return topologicalChildren(shared_from_this());
	}
	else {
		return m_children;
	}
}

shared_ptr<Node> Node::child(const string& name, bool resursive) {
	for (auto child : children(resursive)) {
		if (child->name() == name) return child;
	}
	return nullptr;
}

shared_ptr<PhysicsBody> Node::physicsBody() const {
	return m_physicsBody;
}

void Node::physicsBody(shared_ptr<PhysicsBody> body) {
	body->attachedToNode(shared_from_this());
	m_physicsBody = body;
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void Node::unrollWorldTransform(mat4 transform) {
	// used for physics simulation to update local transform relative to parent
	
	//this->transform(transform * inverse(m_parent.lock()->worldTransform()));
	this->transform(inverse(m_parent.lock()->worldTransform()) * transform);
}

void Node::updateWorldTransform() {
	// gets called by Scene each frame.
	// before being called a topological sort if done on the scene, and each node's 
	// updateWorldTransform() is called in order, guaranteeing that its parent's
	// world tranform is indeed a valid world transform
	
	if (NODE_DIRTY_BITS_CONTAINS(m_dirtyBits, NODE_DIRTY_BITS::WORLD_TRANSFORM)) {
		if (auto p = parent().lock()) {
			//auto oldScale = scale();
			m_worldTransform = p->worldTransform() * transform();
			//auto newScale = scale();
			//checkPhysicsScale(oldScale, newScale);
		}
		
		m_dirtyBits = NODE_DIRTY_BITS_REMOVE(m_dirtyBits, NODE_DIRTY_BITS::WORLD_TRANSFORM);
	}
}

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

void Node::attachedToScene(shared_ptr<Scene> scene) {
//	if (!root()) {
//		m_scene = scene;
//	}
//	else {
//		for (auto child : children(true)) {
//			child->attachedToScene(scene); // just in case they want to do something with it
//		}
//	}
}

void Node::attachedToParent(shared_ptr<Node> parentNode) {
	m_parent = parentNode;
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
//		return m_scene;
//	}
//}

//weak_ptr<Node> Node::model() const {
//	return m_model;
//}
//
//void Node::attachedToModel(shared_ptr<Node> model) {
//	m_model = model;
//}

/***************************************************************************************
     Private
 ***************************************************************************************/

vector<shared_ptr<Node>> Node::pathToRoot() const {
	// walks up the tree to the root node, returning a vector containing the nodes in ascending order
	
	auto parents = vector<shared_ptr<Node>>();
	
	if (auto p = m_parent.lock()) {
		if (auto p = m_parent.lock()) {
			do {
				parents.push_back(p);
				p = p->parent().lock();
			} while (p != nullptr);
		}
	}
	
	return parents;
}

void Node::addDirtyBitsRecursive(NODE_DIRTY_BITS bits) {
	
	m_dirtyBits = NODE_DIRTY_BITS_ADD(m_dirtyBits, bits);
	
	for (auto& c : children(true)) {
		c->m_dirtyBits = NODE_DIRTY_BITS_ADD(c->m_dirtyBits, bits);
	}
}

vector<shared_ptr<Node>> Node::topologicalChildren(shared_ptr<Node> top) {
	
	auto visited = std::map<shared_ptr<Node>, bool>();
	auto stack = std::stack<shared_ptr<Node>>();
	
	topologicalChildrenRec(top, visited, stack);
	
	// probably a better way to do this
	auto vec = vector<shared_ptr<Node>>();
	vec.reserve(stack.size());
	while (!stack.empty()) {
		vec.emplace_back(stack.top());
		stack.pop();
	}
	return vec;
}

void Node::topologicalChildrenRec(shared_ptr<Node> node,
								  map<shared_ptr<Node>, bool>& visited,
								  stack<shared_ptr<Node>>& stack) {
	
	visited[node] = true;
	
	for (auto child : node->m_children) {
		if (!visited[child]) {
			topologicalChildrenRec(child, visited, stack);
		}
	}

	stack.push(node);
}

//void Node::checkPhysicsScale(const glm::vec3& oldScale, const glm::vec3& newScale) {
//	// check if the physics shape needs to be scaled
//	
//	if (m_physicsBody && m_physicsBody->shape()) {
//		if (!Equal(oldScale, newScale)) {
//			auto shape = m_physicsBody->shape();
//			shape->dirtyBits(PHYSICS_SHAPE_DIRTY_BITS_ADD(shape->dirtyBits(),
//														  PHYSICS_SHAPE_DIRTY_BITS::SCALE));
//		}
//	}
//}

NODE_DIRTY_BITS Node::dirtyBits() const {
	return m_dirtyBits;
}

void Node::dirtyBits(NODE_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}
