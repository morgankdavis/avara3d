//
//  PresentationNode.cc
//	avara-engine
//
//  Created by Morgan Davis on 7/2/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PresentationNode.h"

//#include <algorithm>
//#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
//#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/string_cast.hpp>
//#include <glm/gtx/quaternion.hpp>
//
//#include "Camera.h"
//#include "Exception.h"
//#include "Geometry.h"
//#include "Light.h"
#include "Logger.h"
//#include "PhysicsBody.h"
//#include "Scene.h"
//#include "Utilities.h"


using namespace ae;
//using namespace ae::utils;
using namespace std;
using namespace glm;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

PresentationNode::PresentationNode(shared_ptr<Node> owner):
//	m_name(boost::none),
//	m_hidden(false),
//	m_camera(nullptr),
//	m_light(nullptr),
//	m_geometry(nullptr),
	m_position({0.0f, 0.0f, 0.0f}),
	m_orientation(quat()),
	m_scale({1.0f, 1.0f, 1.0f}),
//	m_worldTransform(mat4(1.0f)),
	m_owner(owner) {
//	m_physicsBody(nullptr),
//	m_parent({}),
//	m_scene({}),
//	m_dirtyBits(NODE_DIRTY_BITS::ALL),
//	m_presentation(nullptr),
//	m_model({}) {

}

PresentationNode::~PresentationNode() {
	AE_LOG->debug("Destroying PresentationNode {:p}", (void*)this);
}

/***************************************************************************************
     Public
 ***************************************************************************************/

vec3 PresentationNode::position() const {
	return m_position;
}

vec4 PresentationNode::rotation() const {
	
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

vec3 PresentationNode::eulerAngles() const {  // pitch, yaw, roll

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

quat PresentationNode::orientation() const {
	return m_orientation;
}

vec3 PresentationNode::scale() const {
	return m_scale;
}

mat4 PresentationNode::transform() const {

	mat4 t = translate(mat4(1.0), m_position);
	mat4 r = mat4_cast(m_orientation);
	mat4 s = glm::scale(mat4(1.0), m_scale);
	
	return t * r * s;
}

//vec3 PresentationNode::worldPosition() {
//	auto world = worldTransform();	
//	return vec3(world[3][0], world[3][1], world[3][2]);
//}
//
//vec4 PresentationNode::worldRotation() {
//	return vec4(0.0, 0.0, 0.0, 0.0);
//}
//
//vec3 PresentationNode::worldEulerAngles() {
//	return vec3(0.0, 0.0, 0.0);
//}
//
//quat PresentationNode::worldOrientation() {
//	return quat(1.0, 0.0, 0.0, 0.0);
//}
//
//vec3 PresentationNode::worldScale() {
//	return vec3(0.0, 0.0, 0.0);
//}
//
//vec3 PresentationNode::worldForward() {
//	vec3 scale;
//	quat orientation;
//	vec3 translation;
//	vec3 skew;
//	vec4 perspective;
//
//	decompose(worldTransform(),
//			  scale,
//			  orientation,
//			  translation,
//			  skew,
//			  perspective);
//
//	mat4 rotationMat = mat4_cast(orientation);
//
//	return normalize(rotationMat * vec4(0, 0, -1, 1));
//}
//
//vec3 PresentationNode::worldUp() {
//	vec3 scale;
//	quat orientation;
//	vec3 translation;
//	vec3 skew;
//	vec4 perspective;
//
//	decompose(worldTransform(),
//			  scale,
//			  orientation,
//			  translation,
//			  skew,
//			  perspective);
//
//	mat4 rotationMat = mat4_cast(orientation);
//
//	return normalize(rotationMat * vec4(0, 1, 0, 1));
//}
//
//vec3 PresentationNode::worldRight() {
//	vec3 scale;
//	quat orientation;
//	vec3 translation;
//	vec3 skew;
//	vec4 perspective;
//
//	decompose(worldTransform(),
//			  scale,
//			  orientation,
//			  translation,
//			  skew,
//			  perspective);
//
//	mat4 rotationMat = mat4_cast(orientation);
//
//	return normalize(rotationMat * vec4(1, 0, 0, 1));
//}

mat4 PresentationNode::worldTransform() {

//	if (NODE_DIRTY_BITS_CONTAINS(m_dirtyBits, NODE_DIRTY_BITS::WORLD_TRANSFORM)) {
//		
//		auto t = mat4(1.0f);
//		auto path = pathToRoot();
//		
//		auto iter = path.end();
//		while (iter != path.begin()) {
//			--iter;
//			shared_ptr<Node> node = *iter;
//			t = t * node->transform();
//		}
//		
//		m_worldTransform = t * transform();
//		
//		m_dirtyBits = NODE_DIRTY_BITS_REMOVE(m_dirtyBits, NODE_DIRTY_BITS::WORLD_TRANSFORM);
//	}
//	
//	return m_worldTransform;
	
	mat4 ownerWorld = m_owner.lock()->worldTransform();
	
	return transform() * ownerWorld;
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void PresentationNode::worldTransform(mat4 transform) {

	this->transform(transform * inverse(m_owner.lock()->worldTransform()));
}

weak_ptr<Node> PresentationNode::owner() const {
	return m_owner;
}

//void Node::attachedToModel(shared_ptr<Node> model) {
//	m_model = model;
//}

/***************************************************************************************
     Private
 ***************************************************************************************/

void PresentationNode::transform(const mat4 transform) {
	
	// possibly make ivars of Node protected and re-use Node's transform(mat4) function?
	
	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;
	
	// use GLM 0.9.9 or later. 0.9.8.5 has a bug in orientation calculation
	decompose(transform,
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);
	
	m_position = translation;
	m_scale = scale;
	m_orientation = orientation;
	
	//addDirtyBitsRecursive(NODE_DIRTY_BITS::WORLD_TRANSFORM);
}
