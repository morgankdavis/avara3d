//
// Created by mkd on 10/29/23.
//

#include "physics/bullet/BulletShapeModel.h"

#include <variant>

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "LinearMath/btIDebugDraw.h"
#include "magic_enum.hpp"

#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "geometry/primitives/Box.h"
#include "geometry/primitives/Capsule.h"
#include "geometry/primitives/Cone.h"
#include "geometry/primitives/Cylinder.h"
#include "geometry/primitives/Plane.h"
#include "geometry/primitives/Sphere.h"
#include "physics/ConvexDecomposer.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsBodyModel.h"
#include "physics/PhysicsShape.h"
#include "physics/PhysicalWorld.h"
#include "physics/bullet/BulletDebugDrawer.h"
#include "physics/bullet/BulletWorldModel.h"
#include "physics/shape_primitives/BoxPhysicsShape.h"
#include "physics/shape_primitives/CapsulePhysicsShape.h"
#include "physics/shape_primitives/ConePhysicsShape.h"
#include "physics/shape_primitives/CylinderPhysicsShape.h"
#include "physics/shape_primitives/PlanePhysicsShape.h"
#include "physics/shape_primitives/SpherePhysicsShape.h"
#include "scene/Node.h"
#include "Types.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static shared_ptr<btCollisionShape>
BTShapeFromSourceGeometry(Geometry* geometry,
						  PHYSICS_SHAPE_TYPE shapeType,
						  PHYSICS_BODY_TYPE bodyType,
						  vector<shared_ptr<btCollisionShape>>& btShapes,
						  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static shared_ptr<btCollisionShape>
BTShapeFromSourceNode(Node* node,
					  PHYSICS_SHAPE_TYPE shapeType,
					  PHYSICS_BODY_TYPE bodyType,
					  vector<shared_ptr<btCollisionShape>>& btShapes,
					  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static shared_ptr<btCollisionShape>
BTShapeFromPrimitiveShape(PhysicsShape& shape,
						  PHYSICS_BODY_TYPE bodyType);
static shared_ptr<btCollisionShape>
BTShapeFromGeometryElement(shared_ptr<GeometryElement> element,
						   Geometry* geometry,
						   PHYSICS_SHAPE_TYPE shapeType,
						   PHYSICS_BODY_TYPE bodyType,
						   vector<shared_ptr<btCollisionShape>>& btShapes,
						   shared_ptr<btTriangleIndexVertexArray>& btIndexVertexArray);

static shared_ptr<btCompoundShape>
BTShapeFromGeometry(Geometry* geometry,
					PHYSICS_SHAPE_TYPE shapeType,
					PHYSICS_BODY_TYPE bodyType,
					vector<shared_ptr<btCollisionShape>>& btShapes,
					vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static void
AddBTShapeFromNodeRec(shared_ptr<Node> node,
					  PHYSICS_SHAPE_TYPE shapeType,
					  PHYSICS_BODY_TYPE bodyType,
					  shared_ptr<btCompoundShape> compoundShape,
					  vector<shared_ptr<btCollisionShape>>& btShapes,
					  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);

static shared_ptr<btConvexHullShape>
BTConvexHullShapeFromGeometryElement(shared_ptr<GeometryElement> element);

static shared_ptr<btGImpactMeshShape>
BTGImpactMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
									  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray);

static shared_ptr<btBvhTriangleMeshShape>
BTBvhTriangleMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
										  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray);

static shared_ptr<btCompoundShape>
BTCompoundConvexHullHACDShapeFromGeometryElement(shared_ptr<GeometryElement> element,
												 vector<shared_ptr<btCollisionShape>>& btShapes);

static vector<shared_ptr<GeometryElement>>
HACDGeometryElementsFromGeometryElement(shared_ptr<GeometryElement> element);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletShapeModel::BulletShapeModel(PhysicsShape* shape):
		PhysicsShapeModel(shape),
		_btShapes(vector<shared_ptr<btCollisionShape>>()),
		_btIndexVertexArrays(vector<shared_ptr<btTriangleIndexVertexArray>>()) {

	AE_LOG_D("shape: {:p}", (void*)shape);

	auto bodyType = (*shape->bodies().begin())->type();

	shared_ptr<btCollisionShape> newShape = nullptr;

	auto btShapes = vector<shared_ptr<btCollisionShape>>();
	auto btIndexVertexArrays = vector<shared_ptr<btTriangleIndexVertexArray>>();

	auto sourceObject = shape->sourceObject();

	// souce GEOMETRY
	if (holds_alternative<Geometry*>(sourceObject)) {
		if (auto sourceGeometry = get<Geometry*>(sourceObject)) {

			newShape = BTShapeFromSourceGeometry(sourceGeometry,
												 shape->type(),
												 bodyType,
												 btShapes,
												 btIndexVertexArrays);
		}
	}

	// source NODE
	else if (holds_alternative<Node*>(sourceObject)) {
		if (auto sourceNode = get<Node*>(sourceObject)) {

			newShape = BTShapeFromSourceNode(sourceNode,
											 shape->type(),
											 bodyType,
											 btShapes,
											 btIndexVertexArrays);
		}
	}

	// primitive subclass
	else if (holds_alternative<monostate>(sourceObject)) {

		newShape = BTShapeFromPrimitiveShape(*shape, bodyType);
	}

	if (newShape) {

		newShape->setUserPointer((void*)shape);
		btShapes.insert(btShapes.begin(), newShape);

		_btShapes = btShapes;
		_btIndexVertexArrays = btIndexVertexArrays;

		shape->dirtyMask(PHYSICS_SHAPE_DIRTY_MASK_REMOVE(shape->dirtyMask(),
														PHYSICS_SHAPE_DIRTY_MASK::MODEL));
	}
	else {
		AE_LOG_E("PhysicsShape with no geometry or source node.");
	}
}

BulletShapeModel::~BulletShapeModel() {
	AE_LOG_D("Destroying BulletShapeModel {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

vector <shared_ptr<btCollisionShape>>& BulletShapeModel::btShapes() {
	return _btShapes;
}

//void BulletShapeModel::btShapes(vector<shared_ptr<btCollisionShape>> shapes) {
//	_btShapes = shapes;
//}
//
//vector <shared_ptr<btTriangleIndexVertexArray>>& BulletShapeModel::btIndexVertexArrays() {
//	return _btIndexVertexArrays;
//}
//
//void BulletShapeModel::btIndexVertexArrays(vector<shared_ptr<btTriangleIndexVertexArray>> indexVertexArrays) {
//	_btIndexVertexArrays = indexVertexArrays;
//}

/*********************************************************************************************
	Static
 *********************************************************************************************/

static shared_ptr<btCollisionShape>
BTShapeFromSourceGeometry(Geometry* geometry,
						  PHYSICS_SHAPE_TYPE shapeType,
						  PHYSICS_BODY_TYPE bodyType,
						  vector<shared_ptr<btCollisionShape>>& btShapes,
						  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	shared_ptr<btCollisionShape> newShape = nullptr;

	if (geometry->elements().size() == 1) {
		// make a single shape

		auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
		newShape = BTShapeFromGeometryElement(geometry->elements().front(),
											  geometry,
											  shapeType,
											  bodyType,
											  btShapes,
											  indexVertexArray);
		btShapes.push_back(newShape);
		btIndexVertexArrays.push_back(indexVertexArray);
	}
	else if (geometry->elements().size() > 1) {
		// make compound shape, loop BTShapeFromGeometryElement()

		newShape = BTShapeFromGeometry(geometry,
									   shapeType,
									   bodyType,
									   btShapes,
									   btIndexVertexArrays);
	}
	else {
		AE_LOG_E("Can't create physic shape for Geometry {:p}: has no elements.",
				 (void*)geometry);
	}

	return newShape;
}

static shared_ptr<btCollisionShape>
BTShapeFromSourceNode(Node* node,
					  PHYSICS_SHAPE_TYPE shapeType,
					  PHYSICS_BODY_TYPE bodyType,
					  vector<shared_ptr<btCollisionShape>>& btShapes,
					  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	// *** won't work for most static, kinematic? ***
	// "adding the following shapes to a btCompoundShape is not supported: btTriangleShape,
	// btBvhTriangleMeshShape, btGImpact*Shape, btStaticPlaneShape and a bunch more."
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=17718#p17718

	auto rootShape = make_shared<btCompoundShape>(true); // added to btShapes by caller

	// add the root geometry
	auto geometry = node->geometry().get();
	if (geometry) {

		auto nodeGeoShape = BTShapeFromGeometry(geometry,
												shapeType,
												bodyType,
												btShapes,
												btIndexVertexArrays);
		rootShape->addChildShape(BulletWorldModel::BTIdentityTransform(),
								 nodeGeoShape.get());
		btShapes.push_back(nodeGeoShape);
	}

	// add child geometries recursively
	for (auto& childNode : node->children(false)) {
		AddBTShapeFromNodeRec(childNode,
							  shapeType,
							  bodyType,
							  rootShape,
							  btShapes,
							  btIndexVertexArrays);
	}

	return rootShape;
}

static shared_ptr<btCollisionShape>
BTShapeFromPrimitiveShape(PhysicsShape& shape,
						  PHYSICS_BODY_TYPE bodyType) {

	if (auto boxShape = dynamic_cast<BoxPhysicsShape*>(&shape)) {
		AE_LOG_I("BoxPhysicsShape");

		return make_shared<btBoxShape>(btVector3((btScalar)boxShape->width()/2.0f,
												 (btScalar)boxShape->height()/2.0f,
												 (btScalar)boxShape->length()/2.0f));
	}
	else if (auto capsuleShape = dynamic_cast<CapsulePhysicsShape*>(&shape)) {
		AE_LOG_I("CapsulePhysicsShape");

		return make_shared<btCapsuleShape>((btScalar)capsuleShape->radius(),
										   (btScalar)capsuleShape->height());
	}
	else if (auto coneShape = dynamic_cast<ConePhysicsShape*>(&shape)) {
		AE_LOG_I("ConePhysicsShape");

		return make_shared<btConeShape>((btScalar)coneShape->radius(),
										(btScalar)coneShape->height());
	}
	else if (auto cylinderShape = dynamic_cast<CylinderPhysicsShape*>(&shape)) {
		AE_LOG_I("CylinderPhysicsShape");

		return make_shared<btCylinderShape>(btVector3((btScalar)cylinderShape->radius(),
													  (btScalar)cylinderShape->height()/2.0,
													  (btScalar)cylinderShape->radius()));
	}
	else if (auto planeShape = dynamic_cast<PlanePhysicsShape*>(&shape)) {
		AE_LOG_I("PlanePhysicsShape");

		return make_shared<btBoxShape>(btVector3((btScalar)planeShape->width()/2.0f,
												 (btScalar)planeShape->height()/2.0f,
												 (btScalar)0));
	}
	else if (auto sphereShape = dynamic_cast<SpherePhysicsShape*>(&shape)) {
		AE_LOG_I("SpherePhysicsShape");

		return make_shared<btSphereShape>((btScalar)sphereShape->radius());
	}
	else {
		AE_LOG_E("PhysicsShape {:p} is not a valid subclass.",
				 (void*)&shape);
	}
}

shared_ptr<btCollisionShape>
BTShapeFromGeometryElement(shared_ptr<GeometryElement> element,
						   Geometry* geometry,
						   PHYSICS_SHAPE_TYPE shapeType,
						   PHYSICS_BODY_TYPE bodyType,
						   vector<shared_ptr<btCollisionShape>>& btShapes,
						   shared_ptr<btTriangleIndexVertexArray>& indexVertexArray) {

	if (shapeType == PHYSICS_SHAPE_TYPE::BOUNDING_BOX) {
		AE_LOG_I("Creating box physics shape for GeometryElement {:p}...",
				 (void*)element.get());

		auto extent = element->extent();
		return make_shared<btBoxShape>(btVector3((btScalar)extent.x/2.0f,
												 (btScalar)extent.y/2.0f,
												 (btScalar)extent.z/2.0f));
	}
	else if (auto box = dynamic_cast<Box*>(geometry)) {
		AE_LOG_I("Creating box physics shape for GeometryElement {:p}... "
				 "(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btBoxShape>(btVector3((btScalar)box->length()/2.0f,
												 (btScalar)box->width()/2.0f,
												 (btScalar)box->height()/2.0f));
	}
	else if (auto capsule = dynamic_cast<Capsule*>(geometry)) {
		AE_LOG_I("Creating capsule physics shape for GeometryElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btCapsuleShape>((btScalar)capsule->radius(),
										   (btScalar)capsule->height());
	}
	else if (auto cone  = dynamic_cast<Cone*>(geometry)) {
		AE_LOG_I("Creating cone physics shape for GeometryElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btConeShape>((btScalar)cone->radius(),
										(btScalar)cone->height());
	}
	else if (auto cylinder = dynamic_cast<Cylinder*>(geometry)) {
		AE_LOG_I("Creating cylinder physics shape for GeometryElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btCylinderShape>(btVector3((btScalar)cylinder->radius(),
													  (btScalar)cylinder->height()/2.0,
													  (btScalar)cylinder->radius()));
	}
	else if (auto plane = dynamic_cast<Plane*>(geometry)) {
		// ae::Plane is not a true plane, it has a length and width, so we need to use a btBoxShape
		return make_shared<btBoxShape>(btVector3((btScalar)plane->width()/2.0f,
												 (btScalar)plane->height()/2.0f,
												 (btScalar)0));
	}
	else if (auto sphere = dynamic_cast<Sphere*>(geometry)) {
		AE_LOG_I("Creating sphere physics shape for GeometryElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btSphereShape>((btScalar)sphere->radius());
	}
		// * no Bullet primitives for Torus or Tube *
	else if (shapeType == PHYSICS_SHAPE_TYPE::CONVEX_HULL) {

		return BTConvexHullShapeFromGeometryElement(element);
	}
	else if (bodyType == PHYSICS_BODY_TYPE::DYNAMIC) {

		return BTCompoundConvexHullHACDShapeFromGeometryElement(element, btShapes);
	}
	else if (bodyType == PHYSICS_BODY_TYPE::KINEMATIC) {

		return BTGImpactMeshShapeFromGeometryElement(element, indexVertexArray);
	}
	else if (bodyType == PHYSICS_BODY_TYPE::STATIC) {

		return BTBvhTriangleMeshShapeFromGeometryElement(element, indexVertexArray);
	}

	return nullptr;
}

shared_ptr<btCompoundShape>
BTShapeFromGeometry(Geometry* geometry,
					PHYSICS_SHAPE_TYPE shapeType,
					PHYSICS_BODY_TYPE bodyType,
					vector<shared_ptr<btCollisionShape>>& btShapes,
					vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	auto newShape = make_shared<btCompoundShape>(true); // added to btShapes bt caller

	for (auto& element : geometry->elements()) {

		auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
		auto childShape = BTShapeFromGeometryElement(element,
													 geometry,
													 shapeType,
													 bodyType,
													 btShapes,
													 indexVertexArray);

		// the Geometry's transform is added to the btRigidBody's localInertia
		newShape->addChildShape(BulletWorldModel::BTIdentityTransform(),
								childShape.get());

		btShapes.push_back(childShape);
		btIndexVertexArrays.push_back(indexVertexArray);
	}

	return newShape;
}

void AddBTShapeFromNodeRec(shared_ptr<Node> node,
						   PHYSICS_SHAPE_TYPE shapeType,
						   PHYSICS_BODY_TYPE bodyType,
						   shared_ptr<btCompoundShape> btParentShape,
						   vector<shared_ptr<btCollisionShape>>& btShapes,
						   vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	auto newShape = make_shared<btCompoundShape>(true);

	if (node->name() != nullopt) {
		AE_LOG_I("name: {}", *node->name());
	}

	auto geometry = node->geometry().get();
	if (geometry) {
		auto nodeGeoShape = BTShapeFromGeometry(geometry,
												shapeType,
												bodyType,
												btShapes,
												btIndexVertexArrays);
		newShape->addChildShape(BulletWorldModel::BTIdentityTransform(),
								nodeGeoShape.get());
		btShapes.push_back(nodeGeoShape);
	}

	btParentShape->addChildShape(BulletWorldModel::BTTransformFromGLMMat4(node->transform()),
								 newShape.get());
	btShapes.push_back(newShape);

	// add child geometries recursively
	for (auto& childNode : node->children(false)) {
		AddBTShapeFromNodeRec(childNode,
							  shapeType,
							  bodyType,
							  newShape,
							  btShapes,
							  btIndexVertexArrays);
	}
}

shared_ptr<btConvexHullShape>
BTConvexHullShapeFromGeometryElement(shared_ptr<GeometryElement> element) {
	AE_LOG_I("Creating convex hull physics shape for GeometryElement {:p}...", (void*)element.get());

	// tips here: https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11385

	// https://pybullet.org/Bullet/BulletFull/classbtConvexHullShape.html#a069cf26ba277f9f5f141128fee345eaf
	auto originalShape = make_shared<btConvexHullShape>();
	for (const auto& vertex : element->vertices()) {
		originalShape->addPoint(BulletWorldModel::BTVector3FromGLMVec3(vertex.position), false);
	}
	originalShape->recalcLocalAabb();

	// reduce number of verticies
	// http://www.bulletphysics.org/mediawiki-1.5.8/index.php/BtShapeHull_vertex_reduction_utility
	auto hull = btShapeHull(originalShape.get());
	btScalar margin = originalShape->getMargin();
	hull.buildHull((btScalar)margin);

	auto reducedShape = make_shared<btConvexHullShape>((btScalar*)hull.getVertexPointer(),
													   hull.numVertices(),
													   sizeof(btVector3));

	reducedShape->optimizeConvexHull();

	// for debug drawing
	if (!reducedShape->initializePolyhedralFeatures()) {
		AE_LOG_W("Could not initialize polyhedral features for reduced btConvexHullShape.");
	}

	return reducedShape;
}

shared_ptr<btGImpactMeshShape>
BTGImpactMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
									  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray) {
	AE_LOG_I("Creating concave polyhedron physics shape for GeometryElement {:p}...", (void*)element.get());

	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997
	// "You can use btGImpactMeshShape (or btCompoundShapes plus HACD) for concave dynamic rigidbodies"
	// doesn't seem to want to collide with static shapes.
	// -> https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43020#p43020
	// "- BvhTriangleMeshShapes work well as static concave or convex shapes. But since they are meant to be static, there is no algorithm to make them collide with each other.
	// - ConvexTriangleMeshShapes are efficient as dynamic convex shapes.
	// - GImpact shapes are well optimized for when you need dynamic concave shapes.
	// - Convex decomposition can be used to decompose concave shapes into convex shapes. The resulting convex shapes can then be combined into a CompoundShape, which is also an efficient way to model dynamic concave shapes."
	// More: https://stackoverflow.com/questions/32668218/concave-collision-detection-in-bullet

	const auto& verts = element->vertices();
	const auto& faces = element->faces();

	auto indexedMesh = make_shared<btIndexedMesh>();

	indexedMesh->m_numTriangles = (int)faces.size();
	indexedMesh->m_triangleIndexBase = (const unsigned char *)faces.data();
	indexedMesh->m_triangleIndexStride = sizeof(Face);
	indexedMesh->m_numVertices = (int)verts.size();
	indexedMesh->m_vertexBase = (const unsigned char *)verts.data();
	indexedMesh->m_vertexStride = sizeof(Vertex);
	indexedMesh->m_vertexType = PHY_FLOAT;

	indexVertexArray->addIndexedMesh(*indexedMesh, PHY_INTEGER);

	auto gImpactMeshShape = make_shared<btGImpactMeshShape>(indexVertexArray.get());
	// https://pybullet.org/Bullet/BulletFull/classbtGImpactShapeInterface.html#a7d26525396fa957d10e36c099c58480f
	gImpactMeshShape->updateBound();

	return gImpactMeshShape;
}

shared_ptr<btBvhTriangleMeshShape>
BTBvhTriangleMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
										  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray) {
	AE_LOG_I("Creating concave polyhedron physics shape for GeometryElement {:p}...", (void*)element.get());

	// static objects ALWAYS use btBvhTriangleMeshShape
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997

	const auto& verts = element->vertices();
	const auto& faces = element->faces();

	// ^^ asked about on Bullet forum:
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=44462#p44462

	auto indexedMesh = make_shared<btIndexedMesh>();

	indexedMesh->m_numTriangles = (int)faces.size();
	indexedMesh->m_triangleIndexBase = (const unsigned char *)faces.data();
	indexedMesh->m_triangleIndexStride = sizeof(Face);
	indexedMesh->m_numVertices = (int)verts.size();
	indexedMesh->m_vertexBase = (const unsigned char *)verts.data();
	indexedMesh->m_vertexStride = sizeof(Vertex);
	indexedMesh->m_vertexType = PHY_FLOAT;

	indexVertexArray->addIndexedMesh(*indexedMesh, PHY_INTEGER);

	return make_shared<btBvhTriangleMeshShape>(indexVertexArray.get(), true);
}

shared_ptr<btCompoundShape>
BTCompoundConvexHullHACDShapeFromGeometryElement(shared_ptr<GeometryElement> element,
												 vector<shared_ptr<btCollisionShape>>& btShapes) {
	AE_LOG_I("Creating convex hull compound physics shape for HACD GeometryElement {:p}...",
			 (void*)element.get());

	auto compoundShape = make_shared<btCompoundShape>(true);

	auto hacdElements = HACDGeometryElementsFromGeometryElement(element);
	for (auto& hacdElement : hacdElements) {
		auto convextHullShape = BTConvexHullShapeFromGeometryElement(hacdElement);
		compoundShape->addChildShape(BulletWorldModel::BTIdentityTransform(), convextHullShape.get());
		btShapes.push_back(convextHullShape);
	}

	return compoundShape;
}

vector<shared_ptr<GeometryElement>>
HACDGeometryElementsFromGeometryElement(shared_ptr<GeometryElement> element) {
	AE_LOG_I("Creating HACD GeometryElements for GeometryElement {:p}...", (void*)element.get());

	ConvexDecomposer::Options options;
	options.maxConvexHulls = options.maxConvexHulls / 8;
	options.resolution = options.resolution / 8;
	options.maxRecursionDepth = options.maxRecursionDepth / 4;
	options.maxNumVerticesPerHull = options.maxNumVerticesPerHull / 2;

	auto decomposer = ConvexDecomposer(element, options);
	return decomposer.decompose();
}
