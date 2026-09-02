//
//  BulletShapeProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 10/29/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletShapeProxy.h"

#include <format>
#include <stdexcept>
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
#include "a3d/physics/shape/primitive/FinitePlanePhysicsShape.h"
#include "a3d/physics/shape/primitive/SpherePhysicsShape.h"
#include "a3d/scene/Node.h"

using namespace a3d::math;
using namespace std;

namespace a3d {

namespace {

    // [Private Constants]

    const bool USE_HIGHRES_CONVEX_HULL {false};

    // [Private Non-Member Prototypes]

    unique_ptr<btCollisionShape> BTShapeFromSourceMesh(Mesh&                                 mesh,
                                                       PhysicsShape::Type                    shapeType,
                                                       PhysicsBody::Type                     bodyType,
                                                       vector<unique_ptr<btCollisionShape>>& btShapes,
                                                       vector<unique_ptr<btTriangleIndexVertexArray>>&
                                                           btIndexVertexArrays);
    unique_ptr<btCollisionShape> BTShapeFromSourceNode(Node&                                 node,
                                                       PhysicsShape::Type                    shapeType,
                                                       PhysicsBody::Type                     bodyType,
                                                       vector<unique_ptr<btCollisionShape>>& btShapes,
                                                       vector<unique_ptr<btTriangleIndexVertexArray>>&
                                                           btIndexVertexArrays);
    unique_ptr<btCollisionShape> BTShapeFromPrimitiveShape(PhysicsShape& shape);
    unique_ptr<btCollisionShape> BTShapeFromMeshElement(MeshElement&                          element,
                                                        PhysicsShape::Type                    shapeType,
                                                        PhysicsBody::Type                     bodyType,
                                                        vector<unique_ptr<btCollisionShape>>& btShapes,
                                                        btTriangleIndexVertexArray& btIndexVertexArray);
    unique_ptr<btCompoundShape>  BTShapeFromMesh(Mesh&                                 mesh,
                                                 PhysicsShape::Type                    shapeType,
                                                 PhysicsBody::Type                     bodyType,
                                                 vector<unique_ptr<btCollisionShape>>& btShapes,
                                                 vector<unique_ptr<btTriangleIndexVertexArray>>&
                                                     btIndexVertexArrays);
    void AddBTShapeFromNodeRec(Node&                                           node,
                               PhysicsShape::Type                              shapeType,
                               PhysicsBody::Type                               bodyType,
                               btCompoundShape&                                parentShape,
                               vector<unique_ptr<btCollisionShape>>&           btShapes,
                               vector<unique_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
    unique_ptr<btConvexHullShape>      BTConvexHullShapeFromMeshElement(MeshElement& element);
    unique_ptr<btGImpactMeshShape>     BTGImpactMeshShapeFromMeshElement(MeshElement& element,
                                                                         btTriangleIndexVertexArray&
                                                                             indexVertexArray);
    unique_ptr<btBvhTriangleMeshShape> BTBvhTriangleMeshShapeFromMeshElement(MeshElement& element,
                                                                             btTriangleIndexVertexArray&
                                                                                 indexVertexArray);
    unique_ptr<btCompoundShape> BTCompoundConvexHullHACDShapeFromMeshElement(MeshElement& element,
                                                                             vector<
                                                                                 unique_ptr<btCollisionShape>>&
                                                                                 btShapes);
    vector<unique_ptr<MeshElement>> HACDMeshElementsFromMeshElement(MeshElement& element);
    void                            ValidateBTShapeMarginRec(const btCollisionShape& shape);
    void                            SetBTShapeMarginRec(btCollisionShape& shape, btScalar margin);
    void AccumulateBTShapeMarginRec(const btCollisionShape& shape, btScalar& margin, bool& foundMargin);

} // namespace

// [Internal Lifecycle Functions]

BulletShapeProxy::BulletShapeProxy(PhysicsShape& shape):
    PhysicsShapeProxy {shape},
    _btIndexVertexArrays {},
    _btShapes {} {

    log::d()("shape: {:p}", static_cast<void*>(&shape));

    if (shape.bodies().empty()) {
        throw logic_error("Cannot create BulletShapeProxy for an unattached PhysicsShape.");
    }

    const auto bodyType = (*shape.bodies().begin())->type();

    auto btIndexVertexArrays = vector<unique_ptr<btTriangleIndexVertexArray>>();
    auto btShapes = vector<unique_ptr<btCollisionShape>>();

    const auto sourceObject = shape.source();

    auto newShape = std::visit(
        [&shape, bodyType, &btShapes, &btIndexVertexArrays](auto&& source) -> unique_ptr<btCollisionShape> {
            using T = std::decay_t<decltype(source)>;

            if constexpr (std::is_same_v<T, weak_ptr<Mesh>>) {

                if (auto sourceMesh = source.lock()) {
                    return BTShapeFromSourceMesh(*sourceMesh, shape.type(), bodyType, btShapes,
                                                 btIndexVertexArrays);
                }

                throw logic_error("PhysicsShape source Mesh has expired.");
            }
            else if constexpr (std::is_same_v<T, weak_ptr<Node>>) {

                if (auto sourceNode = source.lock()) {
                    return BTShapeFromSourceNode(*sourceNode, shape.type(), bodyType, btShapes,
                                                 btIndexVertexArrays);
                }

                throw logic_error("PhysicsShape source Node has expired.");
            }
            else if constexpr (std::is_same_v<T, std::monostate>) {
                return BTShapeFromPrimitiveShape(shape);
            }
        },
        sourceObject);

    if (!newShape) {
        throw logic_error("Failed to create a Bullet collision shape for PhysicsShape.");
    }

    newShape->setUserPointer(static_cast<void*>(&shape));
    btShapes.insert(btShapes.begin(), std::move(newShape));

    _btShapes.insert(_btShapes.begin(), std::make_move_iterator(btShapes.begin()),
                     std::make_move_iterator(btShapes.end()));

    _btIndexVertexArrays.insert(_btIndexVertexArrays.begin(),
                                std::make_move_iterator(btIndexVertexArrays.begin()),
                                std::make_move_iterator(btIndexVertexArrays.end()));
}

BulletShapeProxy::~BulletShapeProxy() {
    log::d()("Destroying BulletShapeProxy {:p}", static_cast<void*>(this));
}

// [Internal Member Functions]

float BulletShapeProxy::margin() const {

    if (_btShapes.empty() || !_btShapes.front()) {
        throw logic_error("BulletShapeProxy has no root collision shape.");
    }

    btScalar margin {};
    bool     foundMargin {false};

    AccumulateBTShapeMarginRec(*_btShapes.front(), margin, foundMargin);

    if (!foundMargin) {
        throw logic_error("BulletShapeProxy contains no collision shape with a configurable margin.");
    }

    return static_cast<float>(margin);
}

void BulletShapeProxy::margin(float margin) {

    if (_btShapes.empty() || !_btShapes.front()) {
        throw logic_error("BulletShapeProxy has no root collision shape.");
    }

    auto& rootShape = *_btShapes.front();

    ValidateBTShapeMarginRec(rootShape);
    SetBTShapeMarginRec(rootShape, static_cast<btScalar>(margin));
}

const vector<unique_ptr<btCollisionShape>>& BulletShapeProxy::btShapes() {
    return _btShapes;
}

namespace {

    // [Private Non-Member Functions]

    unique_ptr<btCollisionShape> BTShapeFromSourceMesh(Mesh&                                 mesh,
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
            throw std::invalid_argument(std::format("Can't create physic shape for Mesh {:p} with no elements.",
                                                    static_cast<void*>(&mesh)));
        }

        return newShape;
    }

    unique_ptr<btCollisionShape> BTShapeFromSourceNode(Node&                                 node,
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

    unique_ptr<btCollisionShape> BTShapeFromPrimitiveShape(PhysicsShape& shape) {

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
        else if (auto planeShape = dynamic_cast<FinitePlanePhysicsShape*>(&shape)) {
            return make_unique<btBoxShape>(btVector3((btScalar) planeShape->width() / 2.0f,
                                                     (btScalar) planeShape->height() / 2.0f, (btScalar) 0));
        }
        else if (auto sphereShape = dynamic_cast<SpherePhysicsShape*>(&shape)) {
            return make_unique<btSphereShape>((btScalar) sphereShape->radius());
        }
        else {
            throw logic_error(std::format("PhysicsShape {:p} is not a valid subclass.",
                                          static_cast<void*>(&shape)));
        }

        return nullptr;
    }

    unique_ptr<btCollisionShape> BTShapeFromMeshElement(MeshElement&                          element,
                                                        PhysicsShape::Type                    shapeType,
                                                        PhysicsBody::Type                     bodyType,
                                                        vector<unique_ptr<btCollisionShape>>& btShapes,
                                                        btTriangleIndexVertexArray& btIndexVertexArray) {

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
            throw logic_error("Primitive PhysicsShape cannot be generated from a generic MeshElement.");
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
            auto childShape =
                BTShapeFromMeshElement(*element, shapeType, bodyType, btShapes, *indexVertexArray);

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

        // btShapeHull samples localGetSupportingVertex(), which includes the shape's
        // collision margin. disable it here so reduction operates on the geometric hull
        originalShape.setMargin(0.0f);
        originalShape.recalcLocalAabb();

        btShapeHull hull(&originalShape);
        hull.buildHull(0.0f, static_cast<int>(USE_HIGHRES_CONVEX_HULL));

        auto reducedShape = make_unique<btConvexHullShape>((btScalar*) hull.getVertexPointer(),
                                                           hull.numVertices(), sizeof(btVector3));

        reducedShape->optimizeConvexHull();

        if (!reducedShape->initializePolyhedralFeatures()) {
            log::w()("Could not initialize polyhedral features for reduced btConvexHullShape.");
        }

        return reducedShape;
    }

    unique_ptr<btGImpactMeshShape> BTGImpactMeshShapeFromMeshElement(MeshElement& element,
                                                                     btTriangleIndexVertexArray&
                                                                         indexVertexArray) {
        log::i()("Creating concave polyhedron physics shape for MeshElement {:p}...",
                 static_cast<void*>(&element));

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
        log::i()("Creating concave polyhedron physics shape for MeshElement {:p}...",
                 static_cast<void*>(&element));

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
                                                                             vector<
                                                                                 unique_ptr<btCollisionShape>>&
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

    void ValidateBTShapeMarginRec(const btCollisionShape& shape) {

        if (auto compound = dynamic_cast<const btCompoundShape*>(&shape)) {

            for (int i = 0; i < compound->getNumChildShapes(); ++i) {

                const auto* child = compound->getChildShape(i);

                if (!child) {
                    throw logic_error("btCompoundShape contains a null child shape.");
                }

                ValidateBTShapeMarginRec(*child);
            }

            return;
        }

        if (dynamic_cast<const btConvexHullShape*>(&shape) || dynamic_cast<const btBoxShape*>(&shape)
            || dynamic_cast<const btCylinderShape*>(&shape) || dynamic_cast<const btConeShape*>(&shape)
            || dynamic_cast<const btBvhTriangleMeshShape*>(&shape)
            || dynamic_cast<const btGImpactShapeInterface*>(&shape)) {

            return;
        }

        if (dynamic_cast<const btSphereShape*>(&shape) || dynamic_cast<const btCapsuleShape*>(&shape)
            || dynamic_cast<const btStaticPlaneShape*>(&shape)) {

            throw logic_error("Bullet collision shape does not support a configurable A3D collision margin.");
        }

        throw logic_error("Unsupported Bullet collision shape for collision-margin update.");
    }

    void SetBTShapeMarginRec(btCollisionShape& shape, btScalar margin) {

        if (auto compound = dynamic_cast<btCompoundShape*>(&shape)) {

            for (int i = 0; i < compound->getNumChildShapes(); ++i) {

                auto* child = compound->getChildShape(i);

                if (!child) {
                    throw logic_error("btCompoundShape contains a null child shape.");
                }

                SetBTShapeMarginRec(*child, margin);

                compound->updateChildTransform(i, compound->getChildTransform(i), false);
            }

            compound->recalculateLocalAabb();

            return;
        }

        if (auto hull = dynamic_cast<btConvexHullShape*>(&shape)) {

            hull->setMargin(margin);
            hull->recalcLocalAabb();

            return;
        }

        if (auto box = dynamic_cast<btBoxShape*>(&shape)) {

            box->setMargin(margin);
            return;
        }

        if (auto cylinder = dynamic_cast<btCylinderShape*>(&shape)) {

            cylinder->setMargin(margin);
            return;
        }

        if (auto cone = dynamic_cast<btConeShape*>(&shape)) {

            cone->setMargin(margin);
            return;
        }

        if (auto mesh = dynamic_cast<btBvhTriangleMeshShape*>(&shape)) {

            mesh->setMargin(margin);
            return;
        }

        if (auto gImpact = dynamic_cast<btGImpactShapeInterface*>(&shape)) {

            gImpact->setMargin(margin);
            gImpact->updateBound();

            return;
        }

        throw logic_error("Unsupported Bullet collision shape for collision-margin update.");
    }

    void AccumulateBTShapeMarginRec(const btCollisionShape& shape, btScalar& margin, bool& foundMargin) {

        if (auto compound = dynamic_cast<const btCompoundShape*>(&shape)) {

            for (int i = 0; i < compound->getNumChildShapes(); ++i) {

                const auto* child = compound->getChildShape(i);

                if (!child) {
                    throw logic_error("btCompoundShape contains a null child shape.");
                }

                AccumulateBTShapeMarginRec(*child, margin, foundMargin);
            }

            return;
        }

        btScalar shapeMargin;

        if (auto hull = dynamic_cast<const btConvexHullShape*>(&shape)) {
            shapeMargin = hull->getMargin();
        }
        else if (auto box = dynamic_cast<const btBoxShape*>(&shape)) {
            shapeMargin = box->getMargin();
        }
        else if (auto cylinder = dynamic_cast<const btCylinderShape*>(&shape)) {
            shapeMargin = cylinder->getMargin();
        }
        else if (auto cone = dynamic_cast<const btConeShape*>(&shape)) {
            shapeMargin = cone->getMargin();
        }
        else if (auto mesh = dynamic_cast<const btBvhTriangleMeshShape*>(&shape)) {
            shapeMargin = mesh->getMargin();
        }
        else if (auto gImpact = dynamic_cast<const btGImpactShapeInterface*>(&shape)) {
            shapeMargin = gImpact->getMargin();
        }
        else {
            throw logic_error("Unsupported Bullet collision shape for collision-margin query.");
        }

        if (!foundMargin) {
            margin = shapeMargin;
            foundMargin = true;
            return;
        }

        if (shapeMargin != margin) {
            throw logic_error("PhysicsShape collision components do not use a uniform margin.");
        }
    }

} // namespace

} // namespace a3d
