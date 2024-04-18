//
//  BulletShapeProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 10/29/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/bullet/BulletShapeProxy.h"

#include <utility>
#include <variant>

#include "btBulletCollisionCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "magic_enum.hpp"

#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/mesh/primitive/Capsule.h"
#include "a3d/mesh/primitive/Cone.h"
#include "a3d/mesh/primitive/Cylinder.h"
#include "a3d/mesh/primitive/Plane.h"
#include "a3d/mesh/primitive/Sphere.h"
#include "a3d/physics/ConvexDecomposer.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsShape.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/physics/bullet/BulletWorldProxy.h"
#include "a3d/physics/bullet/BulletUtilities.h"
#include "a3d/physics/proxy/PhysicsBodyProxy.h"
#include "a3d/physics/shape_primitive/BoxPhysicsShape.h"
#include "a3d/physics/shape_primitive/CapsulePhysicsShape.h"
#include "a3d/physics/shape_primitive/ConePhysicsShape.h"
#include "a3d/physics/shape_primitive/CylinderPhysicsShape.h"
#include "a3d/physics/shape_primitive/PlanePhysicsShape.h"
#include "a3d/physics/shape_primitive/SpherePhysicsShape.h"
#include "a3d/scene/Node.h"
#include "a3d/Types.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Private Static Non-Member Prototypes
 *********************************************************************************************/

static unique_ptr<btCollisionShape>
BTShapeFromSourceMesh(Mesh& mesh,
					  PhysicsShapeType shapeType,
					  PhysicsBodyType bodyType,
					  vector<unique_ptr<btCollisionShape>>& btShapes,
					  vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);

static unique_ptr<btCollisionShape>
BTShapeFromSourceNode(Node& node,
					  PhysicsShapeType shapeType,
					  PhysicsBodyType bodyType,
					  vector<unique_ptr<btCollisionShape>>& btShapes,
					  vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);

static unique_ptr<btCollisionShape>
BTShapeFromPrimitiveShape(PhysicsShape& shape);

static unique_ptr<btCollisionShape>
BTShapeFromMeshElement(MeshElement& element,
					   PhysicsShapeType shapeType,
					   PhysicsBodyType bodyType,
					   vector<unique_ptr<btCollisionShape>>& btShapes,
					   btTriangleIndexVertexArray& btIndexVertexArray);

static unique_ptr<btCompoundShape>
BTShapeFromMesh(Mesh& mesh,
				PhysicsShapeType shapeType,
				PhysicsBodyType bodyType,
				vector<unique_ptr<btCollisionShape>>& btShapes,
				vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static void
AddBTShapeFromNodeRec(Node& node,
					  PhysicsShapeType shapeType,
					  PhysicsBodyType bodyType,
					  btCompoundShape& compoundShape,
					  vector<unique_ptr<btCollisionShape>>& btShapes,
					  vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);

static unique_ptr<btConvexHullShape>
BTConvexHullShapeFromMeshElement(MeshElement& element);

static unique_ptr<btGImpactMeshShape>
BTGImpactMeshShapeFromMeshElement(MeshElement& element,
								  btTriangleIndexVertexArray& indexVertexArray);

static unique_ptr<btBvhTriangleMeshShape>
BTBvhTriangleMeshShapeFromMeshElement(MeshElement& element,
									  btTriangleIndexVertexArray& indexVertexArray);

static unique_ptr<btCompoundShape>
BTCompoundConvexHullHACDShapeFromMeshElement(MeshElement& element,
											 vector<unique_ptr<btCollisionShape>>& btShapes);

static vector<unique_ptr<MeshElement>>
HACDMeshElementsFromMeshElement(MeshElement& element);

/*********************************************************************************************
	Internal Lifecycle
 *********************************************************************************************/

BulletShapeProxy::BulletShapeProxy(PhysicsShape& shape):
		PhysicsShapeProxy{shape},
		_btShapes{},
		_btIndexVertexArrays{} {

	A3D_LOG_D("shape: {:p}", static_cast<void*>(&shape));

	auto bodyType = (*shape.bodies().begin())->type();

	unique_ptr<btCollisionShape> newShape = nullptr;

	auto btShapes = vector<unique_ptr<btCollisionShape>>();
	auto btIndexVertexArrays = vector<unique_ptr<btTriangleIndexVertexArray>>();

	auto sourceObject = shape.source();

	// souce MESH
	if (holds_alternative<weak_ptr<Mesh>>(sourceObject)) {
		auto sourceMesh = get<weak_ptr<Mesh>>(sourceObject);
		if (auto sSourceMesh = sourceMesh.lock()) {
			newShape = BTShapeFromSourceMesh(*sSourceMesh,
											 shape.type(),
											 bodyType,
											 btShapes,
											 btIndexVertexArrays);
		}
		else {
			A3D_LOG_W("sourceMesh is null.");
		}
	}

	// source NODE
	else if (holds_alternative<weak_ptr<Node>>(sourceObject)) {
		auto sourceNode = get<weak_ptr<Node>>(sourceObject);
		if (auto sSourceNode = sourceNode.lock()) {
			newShape = BTShapeFromSourceNode(*sSourceNode,
											 shape.type(),
											 bodyType,
											 btShapes,
											 btIndexVertexArrays);
		}
		else {
			A3D_LOG_W("sourceNode is null.");
		}
	}

	// primitive subclass
	else if (holds_alternative<monostate>(sourceObject)) {

		newShape = BTShapeFromPrimitiveShape(shape);
	}

	if (newShape) {

		newShape->setUserPointer(static_cast<void*>(&shape));
		btShapes.insert(btShapes.begin(), std::move(newShape));

		_btShapes.insert(_btShapes.begin(),
						 std::make_move_iterator(btShapes.begin()),
						 std::make_move_iterator(btShapes.end()));

		_btIndexVertexArrays.insert(_btIndexVertexArrays.begin(),
									std::make_move_iterator(btIndexVertexArrays.begin()),
									std::make_move_iterator(btIndexVertexArrays.end()));

//		shape->dirtyMask(PHYSICS_SHAPE_DIRTY_MASK_REMOVE(shape->dirtyMask(),
//														PHYSICS_SHAPE_DIRTY_MASK::MODEL));
	}
	else {
		A3D_LOG_E("PhysicsShape with no mesh or source node.");
	}
}

BulletShapeProxy::~BulletShapeProxy() {
	A3D_LOG_D("Destroying BulletShapeProxy {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

const vector <unique_ptr<btCollisionShape>>& BulletShapeProxy::btShapes() {
	return _btShapes;
}

/*********************************************************************************************
	Static Non-Members
 *********************************************************************************************/

static unique_ptr<btCollisionShape>
BTShapeFromSourceMesh(Mesh& mesh,
					  PhysicsShapeType shapeType,
					  PhysicsBodyType bodyType,
					  vector<unique_ptr<btCollisionShape>>& btShapes,
					  vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	unique_ptr<btCollisionShape> newShape = nullptr;

	if (mesh.elements().size() == 1) {
		// make a single shape

		auto indexVertexArray = make_unique<btTriangleIndexVertexArray>();
		newShape = BTShapeFromMeshElement(*(mesh.elements().front()),
										  shapeType,
										  bodyType,
										  btShapes,
										  *indexVertexArray);

		//btShapes.push_back(newShape); // handled in caller
		btIndexVertexArrays.push_back(std::move(indexVertexArray));
	}
	else if (mesh.elements().size() > 1) {
		// make compound shape, loop BTShapeFromMeshElement()

		newShape = BTShapeFromMesh(mesh,
								   shapeType,
								   bodyType,
								   btShapes,
								   btIndexVertexArrays);
	}
	else {
		A3D_LOG_E("Can't create physic shape for Mesh {:p}: has no elements.",
				 static_cast<void*>(&mesh));
	}

	return newShape;
}

static unique_ptr<btCollisionShape>
BTShapeFromSourceNode(Node& node,
					  PhysicsShapeType shapeType,
					  PhysicsBodyType bodyType,
					  vector<unique_ptr<btCollisionShape>>& btShapes,
					  vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	// *** won't work for most static, kinematic? ***
	// "adding the following shapes to a btCompoundShape is not supported: btTriangleShape,
	// btBvhTriangleMeshShape, btGImpact*Shape, btStaticPlaneShape and a bunch more."
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=17718#p17718

	auto rootShape = make_unique<btCompoundShape>(true); // added to btShapes by caller

	// add the root mesh
	auto& mesh = node.mesh();
	if (mesh) {

		auto nodeGeoShape = BTShapeFromMesh(*mesh,
											shapeType,
											bodyType,
											btShapes,
											btIndexVertexArrays);
		rootShape->addChildShape(btTransform::getIdentity(),
								 nodeGeoShape.get());
		btShapes.push_back(std::move(nodeGeoShape));
	}

	// add child geometries recursively
	for (auto& childNode : node.children(false)) {
		AddBTShapeFromNodeRec(*childNode,
							  shapeType,
							  bodyType,
							  *rootShape,
							  btShapes,
							  btIndexVertexArrays);
	}

	return rootShape;
}

static unique_ptr<btCollisionShape>
BTShapeFromPrimitiveShape(PhysicsShape& shape) {

	if (auto boxShape = dynamic_cast<BoxPhysicsShape*>(&shape)) {
		return make_unique<btBoxShape>(btVector3((btScalar)boxShape->width()/2.0f,
												 (btScalar)boxShape->height()/2.0f,
												 (btScalar)boxShape->length()/2.0f));
	}
	else if (auto capsuleShape = dynamic_cast<CapsulePhysicsShape*>(&shape)) {
		return make_unique<btCapsuleShape>((btScalar)capsuleShape->radius(),
										   (btScalar)capsuleShape->height());
	}
	else if (auto coneShape = dynamic_cast<ConePhysicsShape*>(&shape)) {
		return make_unique<btConeShape>((btScalar)coneShape->radius(),
										(btScalar)coneShape->height());
	}
	else if (auto cylinderShape = dynamic_cast<CylinderPhysicsShape*>(&shape)) {
		return make_unique<btCylinderShape>(btVector3((btScalar)cylinderShape->radius(),
													  (btScalar)round(cylinderShape->height()/2.0),
													  (btScalar)cylinderShape->radius()));
	}
	else if (auto planeShape = dynamic_cast<PlanePhysicsShape*>(&shape)) {
		return make_unique<btBoxShape>(btVector3((btScalar)planeShape->width()/2.0f,
												 (btScalar)planeShape->height()/2.0f,
												 (btScalar)0));
	}
	else if (auto sphereShape = dynamic_cast<SpherePhysicsShape*>(&shape)) {
		return make_unique<btSphereShape>((btScalar)sphereShape->radius());
	}
	else {
		A3D_LOG_E("PhysicsShape {:p} is not a valid subclass.",
				 static_cast<void*>(&shape));
	}

	return nullptr;
}

unique_ptr<btCollisionShape>
BTShapeFromMeshElement(MeshElement& element,
					   PhysicsShapeType shapeType,
					   PhysicsBodyType bodyType,
					   vector<unique_ptr<btCollisionShape>>& btShapes,
					   btTriangleIndexVertexArray& btIndexVertexArray) {

	if (shapeType == PhysicsShapeType::BoundingBox) {
		A3D_LOG_I("Creating box physics shape for MeshElement {:p}...",
				 static_cast<void*>(&element));

		auto extent = element.extent();
		return make_unique<btBoxShape>(btVector3((btScalar)extent.x/2.0f,
												 (btScalar)extent.y/2.0f,
												 (btScalar)extent.z/2.0f));
	}
	else if (auto box = dynamic_cast<Box*>(&element)) {
		A3D_LOG_I("Creating box physics shape for MeshElement {:p}... "
				 "(ignoring physics shape type '{}')",
				 static_cast<void*>(&element), magic_enum::enum_name(shapeType));

		return make_unique<btBoxShape>(btVector3((btScalar)box->length()/2.0f,
												 (btScalar)box->width()/2.0f,
												 (btScalar)box->height()/2.0f));
	}
	else if (auto capsule = dynamic_cast<Capsule*>(&element)) {
		A3D_LOG_I("Creating capsule physics shape for MeshElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 static_cast<void*>(&element), magic_enum::enum_name(shapeType));

		return make_unique<btCapsuleShape>((btScalar)capsule->radius(),
										   (btScalar)capsule->height());
	}
	else if (auto cone  = dynamic_cast<Cone*>(&element)) {
		A3D_LOG_I("Creating cone physics shape for MeshElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 static_cast<void*>(&element), magic_enum::enum_name(shapeType));

		return make_unique<btConeShape>((btScalar)cone->radius(),
										(btScalar)cone->height());
	}
	else if (auto cylinder = dynamic_cast<Cylinder*>(&element)) {
		A3D_LOG_I("Creating cylinder physics shape for MeshElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 static_cast<void*>(&element), magic_enum::enum_name(shapeType));

		return make_unique<btCylinderShape>(btVector3((btScalar)cylinder->radius(),
													  (btScalar)round(cylinder->height()/2.0),
													  (btScalar)cylinder->radius()));
	}
	else if (auto plane = dynamic_cast<Plane*>(&element)) {
		// a3d::Plane is not a true plane, it has a length and width, so we need to use a btBoxShape
		return make_unique<btBoxShape>(btVector3((btScalar)plane->width()/2.0f,
												 (btScalar)plane->height()/2.0f,
												 (btScalar)0));
	}
	else if (auto sphere = dynamic_cast<Sphere*>(&element)) {
		A3D_LOG_I("Creating sphere physics shape for MeshElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 static_cast<void*>(&element), magic_enum::enum_name(shapeType));

		return make_unique<btSphereShape>((btScalar)sphere->radius());
	}
		// * no Bullet primitives for Torus or Tube *
	else if (shapeType == PhysicsShapeType::ConvexHull) {

		return BTConvexHullShapeFromMeshElement(element);
	}
	else if (bodyType == PhysicsBodyType::Dynamic) {

		return BTCompoundConvexHullHACDShapeFromMeshElement(element, btShapes);
	}
	else if (bodyType == PhysicsBodyType::Kinematic) {

		return BTGImpactMeshShapeFromMeshElement(element, btIndexVertexArray);
	}
	else if (bodyType == PhysicsBodyType::Static) {

		return BTBvhTriangleMeshShapeFromMeshElement(element, btIndexVertexArray);
	}

	return nullptr;
}

unique_ptr<btCompoundShape>
BTShapeFromMesh(Mesh& mesh,
				PhysicsShapeType shapeType,
				PhysicsBodyType bodyType,
				vector<unique_ptr<btCollisionShape>>& btShapes,
				vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	auto newShape = make_unique<btCompoundShape>(true); // added to btShapes bt caller

	for (auto& element : mesh.elements()) {

		auto indexVertexArray = make_unique<btTriangleIndexVertexArray>();
		auto childShape = BTShapeFromMeshElement(*element,
												 shapeType,
												 bodyType,
												 btShapes,
												 *indexVertexArray);

		// the Mesh's transform is added to the btRigidBody's localInertia
		newShape->addChildShape(btTransform::getIdentity(),
								childShape.get());

		btShapes.push_back(std::move(childShape));
		btIndexVertexArrays.push_back(std::move(indexVertexArray));
	}

	return newShape;
}

void AddBTShapeFromNodeRec(Node& node,
						   PhysicsShapeType shapeType,
						   PhysicsBodyType bodyType,
						   btCompoundShape& btParentShape,
						   vector<unique_ptr<btCollisionShape>>& btShapes,
						   vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	auto newShape = make_unique<btCompoundShape>(true);

	if (node.name() != nullopt) {
		A3D_LOG_I("name: {}", *node.name());
	}

	auto& mesh = node.mesh();
	if (mesh) {
		auto nodeGeoShape = BTShapeFromMesh(*mesh,
											shapeType,
											bodyType,
											btShapes,
											btIndexVertexArrays);
		newShape->addChildShape(btTransform::getIdentity(),
								nodeGeoShape.get());
		btShapes.push_back(std::move(nodeGeoShape));
	}

	// add child geometries recursively
	for (auto& childNode : node.children(false)) {
		AddBTShapeFromNodeRec(*childNode,
							  shapeType,
							  bodyType,
							  *newShape,
							  btShapes,
							  btIndexVertexArrays);
	}

	btParentShape.addChildShape(BTTransformFromGLMMat4(node.transform()),
								newShape.get());
	btShapes.push_back(std::move(newShape));
}

unique_ptr<btConvexHullShape>
BTConvexHullShapeFromMeshElement(MeshElement& element) {
	A3D_LOG_I("Creating convex hull physics shape for MeshElement {:p}...", static_cast<void*>(&element));

	// tips here: https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11385

	// https://pybullet.org/Bullet/BulletFull/classbtConvexHullShape.html#a069cf26ba277f9f5f141128fee345eaf
	btConvexHullShape originalShape{};
	for (const auto& vertex : element.vertices()) {
		originalShape.addPoint(BTVector3FromGLMVec3(vertex.position), false);
	}
	originalShape.recalcLocalAabb();

	// reduce number of verticies
	// http://www.bulletphysics.org/mediawiki-1.5.8/index.php/BtShapeHull_vertex_reduction_utility
	auto hull = btShapeHull(&originalShape);
	btScalar margin = originalShape.getMargin();
	hull.buildHull((btScalar)margin);

	auto reducedShape = make_unique<btConvexHullShape>((btScalar*)hull.getVertexPointer(),
													   hull.numVertices(),
													   sizeof(btVector3));

	reducedShape->optimizeConvexHull();

	// for debug drawing
	if (!reducedShape->initializePolyhedralFeatures()) {
		A3D_LOG_W("Could not initialize polyhedral features for reduced btConvexHullShape.");
	}

	return reducedShape;
}

unique_ptr<btGImpactMeshShape>
BTGImpactMeshShapeFromMeshElement(MeshElement& element,
								  btTriangleIndexVertexArray& indexVertexArray) {
	A3D_LOG_I("Creating concave polyhedron physics shape for MeshElement {:p}...", static_cast<void*>(&element));

	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997
	// "You can use btGImpactMeshShape (or btCompoundShapes plus HACD) for concave dynamic rigidbodies"
	// doesn't seem to want to collide with static shapes.
	// -> https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43020#p43020
	// "- BvhTriangleMeshShapes work well as static concave or convex shapes. But since they are meant to be static, there is no algorithm to make them collide with each other.
	// - ConvexTriangleMeshShapes are efficient as dynamic convex shapes.
	// - GImpact shapes are well optimized for when you need dynamic concave shapes.
	// - Convex decomposition can be used to decompose concave shapes into convex shapes. The resulting convex shapes can then be combined into a CompoundShape, which is also an efficient way to model dynamic concave shapes."
	// More: https://stackoverflow.com/questions/32668218/concave-collision-detection-in-bullet

	const auto& verts = element.vertices();
	const auto& faces = element.faces();

	btIndexedMesh indexedMesh{};
	indexedMesh.m_numTriangles = static_cast<int>(faces.size());
	indexedMesh.m_triangleIndexBase = reinterpret_cast<const unsigned char *>(faces.data());
	indexedMesh.m_triangleIndexStride = sizeof(Face);
	indexedMesh.m_numVertices = static_cast<int>(verts.size());
	indexedMesh.m_vertexBase = reinterpret_cast<const unsigned char *>(verts.data());
	indexedMesh.m_vertexStride = sizeof(Vertex);
	indexedMesh.m_vertexType = PHY_FLOAT;
	indexVertexArray.addIndexedMesh(indexedMesh, PHY_INTEGER);

	auto gImpactMeshShape = make_unique<btGImpactMeshShape>(&indexVertexArray);
	// https://pybullet.org/Bullet/BulletFull/classbtGImpactShapeInterface.html#a7d26525396fa957d10e36c099c58480f
	gImpactMeshShape->updateBound();

	return gImpactMeshShape;
}

unique_ptr<btBvhTriangleMeshShape>
BTBvhTriangleMeshShapeFromMeshElement(MeshElement& element,
									  btTriangleIndexVertexArray& indexVertexArray) {
	A3D_LOG_I("Creating concave polyhedron physics shape for MeshElement {:p}...", static_cast<void*>(&element));

	// static objects ALWAYS use btBvhTriangleMeshShape
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997

	const auto& verts = element.vertices();
	const auto& faces = element.faces();

	// ^^ asked about on Bullet forum:
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=44462#p44462

	btIndexedMesh indexedMesh{};
	indexedMesh.m_numTriangles = static_cast<int>(faces.size());
	indexedMesh.m_triangleIndexBase = reinterpret_cast<const unsigned char *>(faces.data());
	indexedMesh.m_triangleIndexStride = sizeof(Face);
	indexedMesh.m_numVertices = static_cast<int>(verts.size());
	indexedMesh.m_vertexBase = reinterpret_cast<const unsigned char *>(verts.data());
	indexedMesh.m_vertexStride = sizeof(Vertex);
	indexedMesh.m_vertexType = PHY_FLOAT;
	indexVertexArray.addIndexedMesh(indexedMesh, PHY_INTEGER);

	return make_unique<btBvhTriangleMeshShape>(&indexVertexArray, true);
}

unique_ptr<btCompoundShape>
BTCompoundConvexHullHACDShapeFromMeshElement(MeshElement& element,
											 vector<unique_ptr<btCollisionShape>>& btShapes) {
	A3D_LOG_I("Creating convex hull compound physics shape for HACD MeshElement {:p}...",
			 static_cast<void*>(&element));

	auto compoundShape = make_unique<btCompoundShape>(true);

	auto hacdElements = HACDMeshElementsFromMeshElement(element);
	for (auto& hacdElement : hacdElements) {
		auto convextHullShape = BTConvexHullShapeFromMeshElement(*hacdElement);
		compoundShape->addChildShape(btTransform::getIdentity(), convextHullShape.get());
		btShapes.push_back(std::move(convextHullShape));
	}

	return compoundShape;
}

vector<unique_ptr<MeshElement>>
HACDMeshElementsFromMeshElement(MeshElement& element) {
	A3D_LOG_I("Creating HACD MeshElement for MeshElement {:p}...", static_cast<void*>(&element));

	ConvexDecomposer::Options options;
	options.maxConvexHulls = options.maxConvexHulls / 8;
	options.resolution = options.resolution / 8;
	options.maxRecursionDepth = options.maxRecursionDepth / 4;
	options.maxNumVerticesPerHull = options.maxNumVerticesPerHull / 2;

	auto decomposer = ConvexDecomposer(element, options);
	return decomposer.decompose();
}
