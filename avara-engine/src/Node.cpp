//
//  Node.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Node.h"

#include <algorithm>
#include <iostream>

//#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Camera.h"
#include "Geometry.h"
#include "Light.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;
using namespace glm;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Node::Node():
	m_parent(nullptr),
	m_position(vec3(0.0f, 0.0f, 0.0f)),
	m_orientation(quat()),
	m_scale(vec3(1.0f, 1.0f, 1.0f)) {
	
}

Node::Node(const string& name):
	m_name(name),
	m_hidden(false),
	m_parent(nullptr),
	m_position(vec3(0.0f, 0.0f, 0.0f)),
	m_orientation(quat()),
	m_scale(vec3(1.0f, 1.0f, 1.0f)) {
}

Node::Node(const shared_ptr<Geometry> geometry):
	m_hidden(false),
	m_position(vec3(0.0f, 0.0f, 0.0f)),
	m_orientation(quat()),
	m_scale(vec3(1.0f, 1.0f, 1.0f)) {

		m_geometry->node(this);
}

Node::Node(const string& name, const mat4 t):
	m_name(name),
	m_hidden(false),
	m_parent(nullptr),
	m_position(vec3(0.0f, 0.0f, 0.0f)),
	m_orientation(quat()),
	m_scale(vec3(1.0f, 1.0f, 1.0f)) {
		
		transform(t);
}

Node::Node(const string& name, const mat4 t, const shared_ptr<Geometry> geometry):
	m_name(name),
	m_hidden(false),
	m_parent(nullptr),
	m_position(vec3(0.0f, 0.0f, 0.0f)),
	m_orientation(quat()),
	m_scale(vec3(1.0f, 1.0f, 1.0f)),
	m_geometry(geometry) {
		
		transform(t);
		m_geometry->node(this);
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

string Node::name() const {
	return m_name;
}

void Node::name(const string& name) {
	m_name = name;
}

shared_ptr<Light> Node::light() const {
	return m_light;
}

void Node::light(const shared_ptr<Light> light) {
	m_light = light;
	//m_light->node(this);
}

shared_ptr<Camera> Node::camera() const {
	return m_camera;
}

void Node::camera(const shared_ptr<Camera> camera) {
	camera->node(this);
	m_camera = camera;
	//m_camera->node(this);
}

shared_ptr<Geometry> Node::geometry() const {
	return m_geometry;
}

void Node::geometry(const shared_ptr<Geometry> geometry) {
	m_geometry = geometry;
	m_geometry->node(this);
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
}

vec4 Node::rotation() const {

	vec4 angleAxis = vec4(m_orientation.x / sqrt(1-m_orientation.w*m_orientation.w),
						  m_orientation.y / sqrt(1-m_orientation.w*m_orientation.w),
						  m_orientation.z / sqrt(1-m_orientation.w*m_orientation.w),
						  2 * acos(m_orientation.w));

	return angleAxis;
}

void Node::rotation(const vec4 rotation) {
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/

	float qx = rotation.x * sin(rotation.w/2.0f);
	float qy = rotation.y * sin(rotation.w/2.0f);
	float qz = rotation.z * sin(rotation.w/2.0f);
	float qw = cos(rotation.w/2.0f);

	m_orientation = quat(qx, qy, qz, qw);
}

vec3 Node::eulerAngles() const {
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	
	float heading = atan2(2.0f*m_orientation.y*m_orientation.w - 2.0f*m_orientation.x*m_orientation.z,
						  1.0f - 2.0f*m_orientation.y*m_orientation.y - 2.0f*m_orientation.z*m_orientation.z);
	float attitude = asin(2*m_orientation.x*m_orientation.y + 2.0f*m_orientation.z*m_orientation.w);
	float bank = atan2(2.0f*m_orientation.x*m_orientation.w - 2.0f*m_orientation.y*m_orientation.z,
					   1.0f - 2.0f*m_orientation.x*m_orientation.x - 2.0f*m_orientation.z*m_orientation.z);
	
	return vec3(heading, attitude, bank);
}

void Node::eulerAngles(const vec3 eulerAngles) {

	float heading = eulerAngles.x;
	float attitude = eulerAngles.y;
	float bank = eulerAngles.z;
	
	float c1 = cos(heading / 2.0f);
	float c2 = cos(attitude / 2.0f);
	float c3 = cos(bank / 2.0f);
	float s1 = sin(heading / 2.0f);
	float s2 = sin(attitude / 2.0f);
	float s3 = sin(bank / 2.0f);
	
	float w = c1*c2*c3 - s1*s2*s3;
	float x = s1*s2*c3 + c1*c2*s3;
	float y = s1*c2*c3 + c1*s2*s3;
	float z = c1*s2*c3 - s1*c2*s3;
	
	m_orientation = quat(w, x, y, z);
}

quat Node::orientation() const {
	return m_orientation;
}

void Node::orientation(const quat orientation) {
	m_orientation = orientation;
}

vec3 Node::scale() const {
	return m_scale;
}

void Node::scale(const glm::vec3 scale) {
	m_scale = scale;
}

mat4 Node::transform() const {
	// t = p * r * s ?
	
	// TranslationMatrix * RotationMatrix * ScaleMatrix
	// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
	
	
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
	
	m_position = translation;
	m_scale = scale;
	m_orientation = orientation;
}

mat4 Node::worldTransform() {

	auto t = mat4(1.0f);
	auto path = pathToRoot();

	auto iter = path.end();
	while (iter != path.begin()) {
		--iter;
		Node* node = *iter;
		 t = t * node->transform();
	}

	t = t * transform();

	return t;
}

vec3 Node::worldFormard() {
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

	vec4 forward = inverse(rotationMat) * vec4(0, 0, -1, 1);
	return normalize(vec3(forward));
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

	vec4 up = inverse(rotationMat) * vec4(0, 1, 0, 1);
	return normalize(vec3(up));
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

	vec4 right = inverse(rotationMat) * vec4(1, 0, 0, 1);
	return normalize(vec3(right));
}




void Node::addChildNodes(vector<shared_ptr<Node>> nodes) {
	for (auto node: nodes) {
		addChildNode(node);
	}
}

void Node::addChildNode(shared_ptr<Node> node) {
	node->m_parent = this;
	m_childNodes.push_back(node);
}

void Node::insertChildNode(const Node& node, const int index) {
	
}

void Node::removeFromParentNode() {
	if (m_parent != nullptr) {
		// https://stackoverflow.com/questions/39912/how-do-i-remove-an-item-from-a-stl-vector-with-a-certain-value
		// https://stackoverflow.com/questions/3385229/c-erase-vector-element-by-value-rather-than-by-position
		// http://en.cppreference.com/w/cpp/algorithm/remove
		auto vec = m_parent->m_childNodes;
		vec.erase(remove(vec.begin(), vec.end(), shared_from_this()), vec.end());
// TODO: can we avoid the copy?
		m_parent->m_childNodes = vec;
	}
}

void Node::replaceChildNode(const Node& replace, const Node& with) {
	
}

Node* Node::parent() const {
	return m_parent;
}

vector<shared_ptr<Node>> Node::allChildNodes() { // why no const?
	// returns all decendants in BFS order
		
	auto children = allChildNodesRec();
	children.erase(children.begin());
	return children;
}

shared_ptr<Node> Node::childNode(const string& name, const bool resursive) {

	auto children = vector<shared_ptr<Node>>();
	if (resursive) children = allChildNodes();
	else children = m_childNodes;
	
	for (auto child : children) {
		if (child->name() == name) return child;
	}
	
	return nullptr;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void Node::parent(Node* parent) {
	m_parent = parent;
}

vector<Node*> Node::pathToRoot() {
	// walks up the tree to the root node, returning a vector containing the nodes in ascending order
	
	auto parents = vector<Node*>();
	
	auto p = this->parent();
	if (p != nullptr) {
		do {
			parents.push_back(p);
			p = p->parent();
		} while (p != nullptr);
	}
	
	return parents;
}

std::vector<std::shared_ptr<Node>> Node::immediateChildNodes() {
	return m_childNodes;
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

vector<shared_ptr<Node>> Node::allChildNodesRec() { // why no const?
	// recursive algorithm to do BFS, returning all children as well as this node

	auto children = vector<shared_ptr<Node>>();
	
	auto thisShared = shared_from_this();
	children.push_back(thisShared);
	
	for (auto child : m_childNodes) {
		auto allChildCNodes = child->allChildNodesRec();
		children.insert(children.end(), allChildCNodes.begin(), allChildCNodes.end());
	}

	return children;
}
