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

#include <glm/gtx/matrix_decompose.hpp>
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
m_parent(nullptr), m_transform(mat4(1.0f)) {
	
}

Node::Node(const string& name):
		m_name(name),
		m_hidden(false),
		m_parent(nullptr),
		m_transform(mat4(1.0f)) {
}

Node::Node(const shared_ptr<Geometry> geometry):
		m_hidden(false),
		m_geometry(geometry),
		m_transform(mat4(1.0f)) {

	m_geometry->node(this);
}

Node::Node(const string& name, const mat4 transform):
		m_name(name),
		m_hidden(false),
		m_parent(nullptr),
		m_transform(transform) {

}

Node::Node(const string& name, const mat4 transform, const shared_ptr<Geometry> geometry):
		m_name(name),
		m_hidden(false),
		m_parent(nullptr),
		m_transform(transform),
		m_geometry(geometry) {

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
	//return vec3(0.0f);
	
	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;
	
	decompose(transform(),
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);
	
	cout << "scale: " << scale << endl;
	cout << "orientation: " << orientation << endl;
	cout << "translation: " << translation << endl;
	
	
	
//	http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/
//	angle = 2 * acos(qw)
//	x = qx / sqrt(1-qw*qw)
//	y = qy / sqrt(1-qw*qw)
//	z = qz / sqrt(1-qw*qw)
	
	
//	vec4 angleAxis = vec4(qx / sqrt(1-qw*qw),
//						  qy / sqrt(1-qw*qw),
//						  qz / sqrt(1-qw*qw),
//						  2 * acos(qw));
}

void Node::position(const vec3 position) {
	
}

vec4 Node::rotation() const {
	return vec4(0.0f);
}

void Node::rotation(const vec4 rotation) {
	
}

vec3 Node::eulerAngles() const {
	return vec3(0.0f);
}

void Node::eulerAngles(const vec3 eulerAngles) {
	
}

quat Node::orientation() const {
	return quat();
}

void Node::orientation(const quat orientation) {
	
}

vec3 Node::scale() const {
	
}

void Node::scale(const glm::vec3 scale) {
	return vec3(0.0f);
}

mat4 Node::transform() const {
	return m_transform;
}

void Node::transform(const mat4 transform) {
	m_transform = transform;
	// TODO: set dirty bit for decompose
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
