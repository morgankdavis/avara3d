//
//  Node.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Node.h"

#include <algorithm>

#include <glm/gtx/string_cast.hpp>

#include "Camera.h"
#include "Geometry.h"
#include "Light.h"


using namespace ae;
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

//glm::vec3 position() const;
//void position(const glm::vec3& position);
//
//glm::vec4 rotation() const;
//void rotation(const glm::vec4& rotation);
//
//glm::vec3 eulerAngles() const;
//void eulerAngles(const glm::vec3& eulerAngles);
//
//glm::quat orientation() const;
//void orientation(const glm::quat& orientation);
//
//glm::vec3 scale() const;
//void scale(const glm::vec3& scale);

mat4 Node::transform() const {
	return m_transform;
}

void Node::transform(const mat4 transform) {
	m_transform = transform;
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
