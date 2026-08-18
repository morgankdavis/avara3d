#include <array>
#include <memory>
#include <span>

#include <bullet/btBulletCollisionCommon.h>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/backend/bullet/BulletShapeProxy.h"
#include "a3d/physics/backend/bullet/BulletUtilities.h"
#include "a3d/physics/shape/PhysicsShape.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

TEST_CASE("BoundingBox physics shape preserves the source AABB center") {

    const array<VertexPNT, 2> vertices {{
        {{2.0f, 4.0f, 6.0f}, {}, {}},
        {{4.0f, 8.0f, 10.0f}, {}, {}},
    }};

    const auto vertexBytes = as_bytes(span {vertices});

    auto element = make_unique<MeshElement>(VertexLayout::PNT,
                                            vertexBytes,
                                            static_cast<uint32_t>(vertices.size()),
                                            static_cast<uint16_t>(sizeof(VertexPNT)),
                                            PrimitiveTopology::Points,
                                            IndexFormat::None,
                                            span<const byte> {},
                                            0);

    auto mesh = make_shared<Mesh>(std::move(element), shared_ptr<Material> {});

    const vec3 expectedCenter = AABB::Center(mesh->localAABB());

    auto shape = make_shared<PhysicsShape>(PhysicsShape::Type::BoundingBox, mesh);
    auto body = make_unique<PhysicsBody>(PhysicsBody::Type::Dynamic, shape);

    auto* shapeProxy = static_cast<BulletShapeProxy*>(shape->proxy());

    REQUIRE(shapeProxy);
    REQUIRE_FALSE(shapeProxy->btShapes().empty());

    auto* btShape = shapeProxy->btShapes().front().get();

    REQUIRE(btShape);

    btTransform identity;
    identity.setIdentity();

    btVector3 aabbMin;
    btVector3 aabbMax;
    btShape->getAabb(identity, aabbMin, aabbMax);

    const vec3 actualCenter =
        A3DVec3FromBTVector3((aabbMin + aabbMax) * btScalar(0.5));

    CHECK(actualCenter.x == Catch::Approx(expectedCenter.x));
    CHECK(actualCenter.y == Catch::Approx(expectedCenter.y));
    CHECK(actualCenter.z == Catch::Approx(expectedCenter.z));
}
