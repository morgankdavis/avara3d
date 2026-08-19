//
//  BulletShapeProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 10/29/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletShapeProxy.h"

#include <utility>
#include <variant>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/BulletCollision/Gimpact/btGImpactShape.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

#include "a3d/log/Log.h"
#include "a3d/mesh/ConvexDecomposer.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/PrimitiveTopology.h"
#include "a3d/mesh/VertexAccess.h"
#include "a3d/mesh/VertexLayoutDesc.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/shape/PhysicsShape.h"
#include "a3d/physics/backend/bullet/BulletWorldProxy.h"
#include "a3d/physics/backend/bullet/BulletUtilities.h"
#include "a3d/physics/shape/primitive/BoxPhysicsShape.h"
#include "a3d/physics/shape/primitive/CapsulePhysicsShape.h"
#include "a3d/physics/shape/primitive/ConePhysicsShape.h"
#include "a3d/physics/shape/primitive/CylinderPhysicsShape.h"
#include "a3d/physics/shape/primitive/InfinitePlanePhysicsShape.h"
#include "a3d/physics/shape/primitive/PlanePhysicsShape.h"
#include "a3d/physics/shape/primitive/SpherePhysicsShape.h"
#include "a3d/scene/Node.h"
#include "a3d/util/Enum.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Private Static Non-Member Prototypes ///

static unique_ptr<btCollisionShape> BTShapeFromSourceMesh(Mesh&                                 mesh,
                                                          PhysicsShape::Type                    shapeType,
                                                          PhysicsBody::Type                     bodyType,
                                                          vector<unique_ptr<btCollisionShape>>& btShapes,
                                                          vector<unique_ptr<btTriangleIndexVertexArray>>&
                                                              btIndexVertexArrays);
static unique_ptr<btCollisionShape> BTShapeFromSourceNode(Node&                                 node,
                                                          PhysicsShape::Type                    shapeType,
                                                          PhysicsBody::Type                     bodyType,
                                                          vector<unique_ptr<btCollisionShape>>& btShapes,
                                                          vector<unique_ptr<btTriangleIndexVertexArray>>&
                                                              btIndexVertexArrays);
static unique_ptr<btCollisionShape> BTShapeFromPrimitiveShape(PhysicsShape& shape);
static unique_ptr<btCollisionShape> BTShapeFromMeshElement(MeshElement&                          element,
                                                           PhysicsShape::Type                    shapeType,
                                                           PhysicsBody::Type                     bodyType,
                                                           vector<unique_ptr<btCollisionShape>>& btShapes,
                                                           btTriangleIndexVertexArray& btIndexVertexArray);
static unique_ptr<btCompoundShape>  BTShapeFromMesh(Mesh&                                 mesh,
                                                    PhysicsShape::Type                    shapeType,
                                                    PhysicsBody::Type                     bodyType,
                                                    vector<unique_ptr<btCollisionShape>>& btShapes,
                                                    vector<unique_ptr<btTriangleIndexVertexArray>>&
                                                        btIndexVertexArrays);
static void AddBTShapeFromNodeRec(Node&                                           node,
                                  PhysicsShape::Type                              shapeType,
                                  PhysicsBody::Type                               bodyType,
                                  btCompoundShape&                                parentShape,
                                  vector<unique_ptr<btCollisionShape>>&           btShapes,
                                  vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static unique_ptr<btConvexHullShape>      BTConvexHullShapeFromMeshElement(MeshElement& element);
static unique_ptr<btGImpactMeshShape>     BTGImpactMeshShapeFromMeshElement(MeshElement& element,
                                                                            btTriangleIndexVertexArray&
                                                                                indexVertexArray);
static unique_ptr<btBvhTriangleMeshShape> BTBvhTriangleMeshShapeFromMeshElement(MeshElement& element,
                                                                                btTriangleIndexVertexArray&
                                                                                    indexVertexArray);
static unique_ptr<btCompoundShape>        BTCompoundConvexHullHACDShapeFromMeshElement(MeshElement& element,
                                                                                       vector<unique_ptr<
                                                                                           btCollisionShape>>&
                                                                                           btShapes);
static vector<unique_ptr<MeshElement>>    HACDMeshElementsFromMeshElement(MeshElement& element);

/// Internal Lifecycle Functions ///

BulletShapeProxy::BulletShapeProxy(PhysicsShape& shape):
    PhysicsShapeProxy {shape},
    _btIndexVertexArrays {},
    _btShapes {} {

    log::d()("shape: {:p}", static_cast<void*>(&shape));

    auto bodyType = (*shape.bodies().begin())->type();

    auto btIndexVertexArrays = vector<unique_ptr<btTriangleIndexVertexArray>>();
    auto btShapes = vector<unique_ptr<btCollisionShape>>();

    auto sourceObject = shape.source();

    auto newShape = std::visit(
        [&shape, &bodyType, &btShapes, &btIndexVertexArrays](auto&& source) -> unique_ptr<btCollisionShape> {
            using T = std::decay_t<decltype(source)>;

            if constexpr (std::is_same_v<T, weak_ptr<Mesh>>) {

                if (auto sourceMesh = source.lock()) {
                    return BTShapeFromSourceMesh(*sourceMesh, shape.type(), bodyType, btShapes,
                                                 btIndexVertexArrays);
                }
                else {
                    log::w()("sourceMesh is null.");
                // TODO: throw?
                    return nullptr;
                }
            }
            else if constexpr (std::is_same_v<T, weak_ptr<Node>>) {

                if (auto sourceNode = source.lock()) {
                    return BTShapeFromSourceNode(*sourceNode, shape.type(), bodyType, btShapes,
                                                 btIndexVertexArrays);
                }
                else {
                    log::w()("sourceNode is null.");
                // TODO: throw?
                    return nullptr;
                }
            }

            else if constexpr (std::is_same_v<T, std::monostate>) {
                return BTShapeFromPrimitiveShape(shape);
            }
        },
        sourceObject);

    if (newShape) {

        newShape->setUserPointer(static_cast<void*>(&shape));
        btShapes.insert(btShapes.begin(), std::move(newShape));

        _btShapes.insert(_btShapes.begin(), std::make_move_iterator(btShapes.begin()),
                         std::make_move_iterator(btShapes.end()));

        _btIndexVertexArrays.insert(_btIndexVertexArrays.begin(),
                                    std::make_move_iterator(btIndexVertexArrays.begin()),
                                    std::make_move_iterator(btIndexVertexArrays.end()));

//		shape->dirtyMask(PHYSICS_SHAPE_DIRTY_MASK_REMOVE(shape->dirtyMask(),
//														PHYSICS_SHAPE_DIRTY_MASK::MODEL));
    }
    else {
        log::e()("PhysicsShape with no mesh or source node.");
    }
}

BulletShapeProxy::~BulletShapeProxy() {
    log::d()("Destroying BulletShapeProxy {:p}", static_cast<void*>(this));
}

/// Internal Member Functions ///

const vector<unique_ptr<btCollisionShape>>& BulletShapeProxy::btShapes() {
    return _btShapes;
}

/// Static Non-Member Functions ///

static unique_ptr<btCollisionShape> BTShapeFromSourceMesh(Mesh&                                 mesh,
                                                          PhysicsShape::Type                    shapeType,
                                                          PhysicsBody::Type                     bodyType,
                                                          vector<unique_ptr<btCollisionShape>>& btShapes,
                                                          vector<unique_ptr<btTriangleIndexVertexArray>>&
                                                              btIndexVertexArrays) {

    unique_ptr<btCollisionShape> newShape = nullptr;

    if (mesh.elements().size() == 1) {
        // make a single shape

        auto indexVertexArray = make_unique<btTriangleIndexVertexArray>();
        newShape = BTShapeFromMeshElement(*(mesh.elements().front()), shapeType, bodyType, btShapes,
                                          *indexVertexArray);

        //btShapes.push_back(newShape); // handled in caller
        btIndexVertexArrays.push_back(std::move(indexVertexArray));
    }
    else if (mesh.elements().size() > 1) {
        // make compound shape, loop BTShapeFromMeshElement()

        newShape = BTShapeFromMesh(mesh, shapeType, bodyType, btShapes, btIndexVertexArrays);
    }
    else {
        log::e()("Can't create physic shape for Mesh {:p}: has no elements.", static_cast<void*>(&mesh));
    }

    return newShape;
}

static unique_ptr<btCollisionShape> BTShapeFromSourceNode(Node&                                 node,
                                                          PhysicsShape::Type                    shapeType,
                                                          PhysicsBody::Type                     bodyType,
                                                          vector<unique_ptr<btCollisionShape>>& btShapes,
                                                          vector<unique_ptr<btTriangleIndexVertexArray>>&
                                                              btIndexVertexArrays) {

    // *** won't work for most static, kinematic? ***
    // "adding the following shapes to a btCompoundShape is not supported: btTriangleShape,
    // btBvhTriangleMeshShape, btGImpact*Shape, btStaticPlaneShape and a bunch more."
    // https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=17718#p17718

    auto rootShape = make_unique<btCompoundShape>(true); // added to btShapes by caller

    // add the root mesh
    auto& mesh = node.mesh();
    if (mesh) {

        auto nodeGeoShape = BTShapeFromMesh(*mesh, shapeType, bodyType, btShapes, btIndexVertexArrays);
        rootShape->addChildShape(btTransform::getIdentity(), nodeGeoShape.get());
        btShapes.push_back(std::move(nodeGeoShape));
    }

    // add child geometries recursively
    for (auto& childNode : node.children(false)) {
        AddBTShapeFromNodeRec(*childNode, shapeType, bodyType, *rootShape, btShapes, btIndexVertexArrays);
    }

    return rootShape;
}

static unique_ptr<btCollisionShape> BTShapeFromPrimitiveShape(PhysicsShape& shape) {

    if (auto boxShape = dynamic_cast<BoxPhysicsShape*>(&shape)) {
        return make_unique<btBoxShape>(btVector3((btScalar) boxShape->width() / 2.0f,
                                                 (btScalar) boxShape->height() / 2.0f,
                                                 (btScalar) boxShape->length() / 2.0f));
    }
    else if (auto capsuleShape = dynamic_cast<CapsulePhysicsShape*>(&shape)) {
        return make_unique<btCapsuleShape>((btScalar) capsuleShape->radius(),
                                           (btScalar) capsuleShape->height());
    }
    else if (auto coneShape = dynamic_cast<ConePhysicsShape*>(&shape)) {
        return make_unique<btConeShape>((btScalar) coneShape->radius(), (btScalar) coneShape->height());
    }
    else if (auto cylinderShape = dynamic_cast<CylinderPhysicsShape*>(&shape)) {
        return make_unique<btCylinderShape>(btVector3((btScalar) cylinderShape->radius(),
                                                      (btScalar) cylinderShape->height() / 2.0f,
                                                      (btScalar) cylinderShape->radius()));
    }
    else if (dynamic_cast<InfinitePlanePhysicsShape*>(&shape)) {
        return make_unique<btStaticPlaneShape>(btVector3(0.0f, 0.0f, 1.0f), (btScalar) 0);
    }
    else if (auto planeShape = dynamic_cast<PlanePhysicsShape*>(&shape)) {
        return make_unique<btBoxShape>(btVector3((btScalar) planeShape->width() / 2.0f,
                                                 (btScalar) planeShape->height() / 2.0f, (btScalar) 0));
    }
    else if (auto sphereShape = dynamic_cast<SpherePhysicsShape*>(&shape)) {
        return make_unique<btSphereShape>((btScalar) sphereShape->radius());
    }
    else {
        log::e()("PhysicsShape {:p} is not a valid subclass.", static_cast<void*>(&shape));
    }

    return nullptr;
}

unique_ptr<btCollisionShape> BTShapeFromMeshElement(MeshElement&                          element,
                                                    PhysicsShape::Type                    shapeType,
                                                    PhysicsBody::Type                     bodyType,
                                                    vector<unique_ptr<btCollisionShape>>& btShapes,
                                                    btTriangleIndexVertexArray&           btIndexVertexArray) {

    if (shapeType == PhysicsShape::Type::BoundingBox) {
        log::i()("Creating box physics shape for MeshElement {:p}...", static_cast<void*>(&element));

        const auto aabb = element.localAABB();
        const auto extent = aabb.max - aabb.min;
        const auto center = AABB::Center(aabb);

        auto boxShape =
            make_unique<btBoxShape>(btVector3((btScalar) extent.x / 2.0f, (btScalar) extent.y / 2.0f,
                                              (btScalar) extent.z / 2.0f));

        auto compoundShape = make_unique<btCompoundShape>(true);

        btTransform childTransform;
        childTransform.setIdentity();
        childTransform.setOrigin(BTVector3FromA3DVec3(center));

        compoundShape->addChildShape(childTransform, boxShape.get());

        btShapes.push_back(std::move(boxShape));

        return compoundShape;
    }
    else if (shapeType == PhysicsShape::Type::Primitive) {

        // gross
        log::e()("Primitive PhysicsShape cannot be generated from a generic MeshElement.");
        return nullptr;
    }
    else if (shapeType == PhysicsShape::Type::ConvexHull) {

        return BTConvexHullShapeFromMeshElement(element);
    }
    else if (bodyType == PhysicsBody::Type::Dynamic) {

        return BTCompoundConvexHullHACDShapeFromMeshElement(element, btShapes);
    }
    else if (bodyType == PhysicsBody::Type::Kinematic) {

        return BTGImpactMeshShapeFromMeshElement(element, btIndexVertexArray);
    }
    else if (bodyType == PhysicsBody::Type::Static) {

        return BTBvhTriangleMeshShapeFromMeshElement(element, btIndexVertexArray);
    }

    return nullptr;
}

unique_ptr<btCompoundShape> BTShapeFromMesh(Mesh&                                 mesh,
                                            PhysicsShape::Type                    shapeType,
                                            PhysicsBody::Type                     bodyType,
                                            vector<unique_ptr<btCollisionShape>>& btShapes,
                                            vector<unique_ptr<btTriangleIndexVertexArray>>&
                                                btIndexVertexArrays) {

    auto newShape = make_unique<btCompoundShape>(true); // added to btShapes bt caller

    for (auto& element : mesh.elements()) {

        auto indexVertexArray = make_unique<btTriangleIndexVertexArray>();
        auto childShape = BTShapeFromMeshElement(*element, shapeType, bodyType, btShapes, *indexVertexArray);

        // the Mesh's transform is added to the btRigidBody's localInertia
        newShape->addChildShape(btTransform::getIdentity(), childShape.get());

        btShapes.push_back(std::move(childShape));
        btIndexVertexArrays.push_back(std::move(indexVertexArray));
    }

    return newShape;
}

void AddBTShapeFromNodeRec(Node&                                           node,
                           PhysicsShape::Type                              shapeType,
                           PhysicsBody::Type                               bodyType,
                           btCompoundShape&                                btParentShape,
                           vector<unique_ptr<btCollisionShape>>&           btShapes,
                           vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

    auto newShape = make_unique<btCompoundShape>(true);

    if (node.name() != nullopt) {
        log::i()("name: {}", *node.name());
    }

    auto& mesh = node.mesh();
    if (mesh) {
        auto nodeGeoShape = BTShapeFromMesh(*mesh, shapeType, bodyType, btShapes, btIndexVertexArrays);
        newShape->addChildShape(btTransform::getIdentity(), nodeGeoShape.get());
        btShapes.push_back(std::move(nodeGeoShape));
    }

    // add child geometries recursively
    for (auto& childNode : node.children(false)) {
        AddBTShapeFromNodeRec(*childNode, shapeType, bodyType, *newShape, btShapes, btIndexVertexArrays);
    }

    btParentShape.addChildShape(BTTransformFromA3DMat4(node.transform()), newShape.get());
    btShapes.push_back(std::move(newShape));
}

unique_ptr<btConvexHullShape> BTConvexHullShapeFromMeshElement(MeshElement& element) {
    log::i()("Creating convex hull physics shape for MeshElement {:p}...", static_cast<void*>(&element));

    // tips here: https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11385

    const auto     vb = element.vertexBytes();
    const uint16_t st = element.vertexStride();
    const uint32_t n = element.vertexCount();
    if (n == 0) {
        return make_unique<btConvexHullShape>(); // empty
    }

    const VertexAttribDesc* posA = VertexAccess::GetPositionAttribF32x3(element.vertexLayout());

    // https://pybullet.org/Bullet/BulletFull/classbtConvexHullShape.html#a069cf26ba277f9f5f141128fee345eaf
    btConvexHullShape originalShape {};
    for (uint32_t i = 0; i < n; ++i) {
        const std::byte* base = vb.data() + size_t(i) * size_t(st);
        const vec3       p = VertexAccess::ReadVec3(base, posA->offset);
        originalShape.addPoint(BTVector3FromA3DVec3(p), false);
    }
    originalShape.recalcLocalAabb();

    // reduce number of verticies
    // http://www.bulletphysics.org/mediawiki-1.5.8/index.php/BtShapeHull_vertex_reduction_utility
    btShapeHull hull(&originalShape);
    btScalar    margin = originalShape.getMargin();
    hull.buildHull(margin);

    auto reducedShape = make_unique<btConvexHullShape>((btScalar*) hull.getVertexPointer(), hull.numVertices(),
                                                       sizeof(btVector3));

    reducedShape->optimizeConvexHull();

    if (!reducedShape->initializePolyhedralFeatures()) {
        log::w()("Could not initialize polyhedral features for reduced btConvexHullShape.");
    }

    return reducedShape;
}

unique_ptr<btGImpactMeshShape> BTGImpactMeshShapeFromMeshElement(MeshElement&                element,
                                                                 btTriangleIndexVertexArray& indexVertexArray) {
    log::i()("Creating concave polyhedron physics shape for MeshElement {:p}...", static_cast<void*>(&element));

    // https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997
    // "You can use btGImpactMeshShape (or btCompoundShapes plus HACD) for concave dynamic rigidbodies"
    // doesn't seem to want to collide with static shapes.
    // -> https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43020#p43020
    // "- BvhTriangleMeshShapes work well as static concave or convex shapes. But since they are meant to be static, there is no algorithm to make them collide with each other.
    // - ConvexTriangleMeshShapes are efficient as dynamic convex shapes.
    // - GImpact shapes are well optimized for when you need dynamic concave shapes.
    // - Convex decomposition can be used to decompose concave shapes into convex shapes. The resulting convex shapes can then be combined into a CompoundShape, which is also an efficient way to model dynamic concave shapes."
    // More: https://stackoverflow.com/questions/32668218/concave-collision-detection-in-bullet

    const uint32_t vcount = element.vertexCount();
    if (vcount == 0) {
        return make_unique<btGImpactMeshShape>(&indexVertexArray);
    }

    // requires indexed triangles
    A3D_ASSERT(element.topology() == PrimitiveTopology::Triangles);

    const IndexFormat ifmt = element.indexFormat();
    const uint32_t    icount = element.indexCount(); // number of indices (not triangles!)

    if (ifmt == IndexFormat::None || icount == 0) {
        // no indices -> nothing we can feed btTriangleIndexVertexArray
        return make_unique<btGImpactMeshShape>(&indexVertexArray);
    }

    const uint16_t indexStride = IndexStride(ifmt);
    A3D_ASSERT(indexStride == 2 || indexStride == 4);
    A3D_ASSERT((icount % 3u) == 0u);

    const auto     vb = element.vertexBytes();
    const auto     ib = element.indexBytes();
    const uint16_t st = element.vertexStride();

    const VertexAttribDesc* posA = VertexAccess::GetPositionAttribF32x3(element.vertexLayout());
    A3D_ASSERT(posA);

    const uint32_t triCount = icount / 3u;
    if (triCount == 0) {
        return make_unique<btGImpactMeshShape>(&indexVertexArray);
    }

    // choose Bullet index scalar type based on our index format
    const PHY_ScalarType bulletIndexType = (ifmt == IndexFormat::U16) ? PHY_SHORT
                                           : (ifmt == IndexFormat::U32)
                                               ? PHY_INTEGER
                                               : PHY_INTEGER; // shouldn't happen due to checks above

    btIndexedMesh indexedMesh {};
    indexedMesh.m_numTriangles = static_cast<int>(triCount);
    indexedMesh.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(ib.data());
    indexedMesh.m_triangleIndexStride = static_cast<int>(3u * uint32_t(indexStride));

    indexedMesh.m_numVertices = static_cast<int>(vcount);
    indexedMesh.m_vertexBase = reinterpret_cast<const unsigned char*>(vb.data() + posA->offset);
    indexedMesh.m_vertexStride = static_cast<int>(st);
    indexedMesh.m_vertexType = PHY_FLOAT;

    indexVertexArray.addIndexedMesh(indexedMesh, bulletIndexType);

    auto gImpactMeshShape = make_unique<btGImpactMeshShape>(&indexVertexArray);
    // https://pybullet.org/Bullet/BulletFull/classbtGImpactShapeInterface.html#a7d26525396fa957d10e36c099c58480f
    gImpactMeshShape->updateBound();
    return gImpactMeshShape;
}

unique_ptr<btBvhTriangleMeshShape> BTBvhTriangleMeshShapeFromMeshElement(MeshElement& element,
                                                                         btTriangleIndexVertexArray&
                                                                             indexVertexArray) {
    log::i()("Creating concave polyhedron physics shape for MeshElement {:p}...", static_cast<void*>(&element));

    // static objects ALWAYS use btBvhTriangleMeshShape
    // https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997

    const uint32_t vcount = element.vertexCount();
    if (vcount == 0) {
        return make_unique<btBvhTriangleMeshShape>(&indexVertexArray, true);
    }

    // requires indexed triangles
    A3D_ASSERT(element.topology() == PrimitiveTopology::Triangles);

    const IndexFormat ifmt = element.indexFormat();
    const uint32_t    icount = element.indexCount(); // number of indices (NOT triangles)

    if (ifmt == IndexFormat::None || icount == 0) {
        // no indices -> nothing we can feed btTriangleIndexVertexArray.
        return make_unique<btBvhTriangleMeshShape>(&indexVertexArray, true);
    }

    const uint16_t indexStride = IndexStride(ifmt);
    A3D_ASSERT(indexStride == 2 || indexStride == 4);
    A3D_ASSERT((icount % 3u) == 0u);

    const auto     vb = element.vertexBytes();
    const auto     ib = element.indexBytes();
    const uint16_t st = element.vertexStride();

    const VertexAttribDesc* posA = VertexAccess::GetPositionAttribF32x3(element.vertexLayout());
    A3D_ASSERT(posA);

    const uint32_t triCount = icount / 3u;
    if (triCount == 0) {
        return make_unique<btBvhTriangleMeshShape>(&indexVertexArray, true);
    }

    // choose Bullet index scalar type based on our index format
    const PHY_ScalarType bulletIndexType = (ifmt == IndexFormat::U16) ? PHY_SHORT
                                           : (ifmt == IndexFormat::U32)
                                               ? PHY_INTEGER
                                               : PHY_INTEGER; // shouldn't happen due to checks above

    btIndexedMesh indexedMesh {};
    indexedMesh.m_numTriangles = static_cast<int>(triCount);
    indexedMesh.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(ib.data());
    indexedMesh.m_triangleIndexStride = static_cast<int>(3u * uint32_t(indexStride));
    indexedMesh.m_numVertices = static_cast<int>(vcount);
    indexedMesh.m_vertexBase = reinterpret_cast<const unsigned char*>(vb.data() + posA->offset);
    indexedMesh.m_vertexStride = static_cast<int>(st);
    indexedMesh.m_vertexType = PHY_FLOAT;

    indexVertexArray.addIndexedMesh(indexedMesh, bulletIndexType);

    return make_unique<btBvhTriangleMeshShape>(&indexVertexArray, true);
}

unique_ptr<btCompoundShape> BTCompoundConvexHullHACDShapeFromMeshElement(MeshElement& element,
                                                                         vector<unique_ptr<btCollisionShape>>&
                                                                             btShapes) {
    log::i()("Creating convex hull compound physics shape for HACD MeshElement {:p}...",
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

vector<unique_ptr<MeshElement>> HACDMeshElementsFromMeshElement(MeshElement& element) {
    log::i()("Creating HACD MeshElement for MeshElement {:p}...", static_cast<void*>(&element));

    ConvexDecomposer::Options options;
    options.maxConvexHulls = options.maxConvexHulls / 8;
    options.resolution = options.resolution / 8;
    options.maxRecursionDepth = options.maxRecursionDepth / 4;
    options.maxNumVerticesPerHull = options.maxNumVerticesPerHull / 2;

    auto decomposer = ConvexDecomposer(element, options);
    return decomposer.decompose();
}
