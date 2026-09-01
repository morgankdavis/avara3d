//
//  TransientsBuilder.cc
//  janus
//
//  Created by Morgan Davis on 8/30/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "TransientsBuilder.h"
#include "Constants.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace demo::janus;
using namespace std;

// [Private Static Non-Member Prototypes]

static pair<vec3, vec3> CalculateGridLayout(const vec3&   location,
                                            const vec3&   cellSize,
                                            const u8vec3& gridSize,
                                            float         gap);

// [Public Static Member Functions]

vector<shared_ptr<Node>> TransientsBuilder::BuildRocks(const vector<TransientsCache::Entry>& cacheEntries,
                                                       const vec3&                           location,
                                                       const u8vec3&                         stackSize,
                                                       float                                 gap) {

    float maxDim = 0.0f;
    for (const auto& entry : cacheEntries) {
        maxDim = std::max(maxDim, math::max(entry.mesh->localExtent()));
    }

    const vec3 cellSize {maxDim};

    const auto [startPosition, step] = CalculateGridLayout(location, cellSize, stackSize, gap);

    const unsigned sizeX = stackSize.x;
    const unsigned sizeY = stackSize.y;
    const unsigned sizeZ = stackSize.z;

    vector<shared_ptr<Node>> added;
    added.reserve(sizeX * sizeY * sizeZ);

    const vec3 positionVariance {gap / 2.0f};

    size_t rockIndex = 0;
    for (unsigned y = 0; y < sizeY; ++y) {
        for (unsigned z = 0; z < sizeZ; ++z) {
            for (unsigned x = 0; x < sizeX; ++x) {

                const auto& rockEntry = cacheEntries[rockIndex++ % cacheEntries.size()];

                auto node = Node::MeshNode(rockEntry.mesh);

                static int rockNum = 0;
                node->name(std::format("pock {}", ++rockNum));

                // node->position(startPosition
                //                + vec3 {static_cast<float>(x) * step.x, static_cast<float>(y) * step.y,
                //                        static_cast<float>(z) * step.z});
                const vec3 position = startPosition
                      + vec3 {static_cast<float>(x) * step.x, static_cast<float>(y) * step.y,
                              static_cast<float>(z) * step.z};

                node->position(position + uniform_linear(-positionVariance, positionVariance));

                auto body = PhysicsBody::DynamicBody(rockEntry.physicsShape);
                body->mass(3.5f);
                body->friction(STONE_FRICTION);
                body->restitution(STONE_RESTITUTION);
                body->rollingFriction(0.01f);
                body->spinningFriction(0.015f);
                // body->angularDamping(0.005f);
                body->angularSleepingThreshold(.25f);

                const float ANGULAR_VARIANCE = radians(30.0f);
                body->angularVelocity(uniform_linear(vec3 {-ANGULAR_VARIANCE}, vec3 {ANGULAR_VARIANCE}));

                node->physicsBody(std::move(body));

                added.push_back(node);
            }
        }
    }

    return added;
}

vector<shared_ptr<Node>> TransientsBuilder::BuildCoins(const TransientsCache::Entry& cacheEntry,
                                                       const vec3&                   location,
                                                       const u8vec3&                 stackSize,
                                                       float                         gap) {

    auto [mesh, shape] = cacheEntry;

    const vec3 cellSize {math::max(mesh->localExtent())};
    const auto [startPosition, step] = CalculateGridLayout(location, cellSize, stackSize, gap);

    const unsigned sizeX = stackSize.x;
    const unsigned sizeY = stackSize.y;
    const unsigned sizeZ = stackSize.z;

    vector<shared_ptr<Node>> added;
    added.reserve(sizeX * sizeY * sizeZ);

    const vec3 positionVariance {gap / 2.0f};

    for (unsigned y = 0; y < sizeY; ++y) {
        for (unsigned z = 0; z < sizeZ; ++z) {
            for (unsigned x = 0; x < sizeX; ++x) {

                auto node = Node::MeshNode(mesh);

                static int coinNum = 0;
                node->name(std::format("coin {}", ++coinNum));

                const vec3 position = startPosition
                                      + vec3 {static_cast<float>(x) * step.x, static_cast<float>(y) * step.y,
                                              static_cast<float>(z) * step.z};

                node->position(position + uniform_linear(-positionVariance, positionVariance));
                node->eulerAngles(uniform_linear(vec3 {0.0f}, vec3 {TWO_PI}));

                auto body = PhysicsBody::DynamicBody(shape);
                body->mass(20.0f);
                body->friction(0.35f);
                body->restitution(0.1);
                body->rollingFriction(0.15f);
                body->spinningFriction(0.1);
                body->angularDamping(0.4f);

                // const auto extent = mesh->localExtent();
                // const float minExtent = math::min(extent);
                // physicsBody->ccdMotionThreshold(minExtent * 0.25f);
                // physicsBody->ccdSweptSphereRadius(minExtent * 0.20f);
                // physicsBody->ccdEnabled(true);

                const float ANGULAR_VARIANCE = radians(180.0f);
                body->angularVelocity(uniform_linear(vec3 {-ANGULAR_VARIANCE}, vec3 {ANGULAR_VARIANCE}));

                node->physicsBody(std::move(body));

                added.push_back(node);
            }
        }
    }

    return added;
}

vector<shared_ptr<Node>> TransientsBuilder::BuildBalls(const TransientsCache::Entry& cacheEntry,
                                                       const vec3&                   location,
                                                       const u8vec3&                 stackSize,
                                                       float                         gap) {

    auto [mesh, shape] = cacheEntry;

    const vec3 cellSize {math::max(mesh->localExtent())};
    const auto [startPosition, step] = CalculateGridLayout(location, cellSize, stackSize, gap);

    const unsigned sizeX = stackSize.x;
    const unsigned sizeY = stackSize.y;
    const unsigned sizeZ = stackSize.z;

    vector<shared_ptr<Node>> added;
    added.reserve(sizeX * sizeY * sizeZ);

    const vec3 positionVariance {gap / 2.0f};

    for (unsigned y = 0; y < sizeY; ++y) {
        for (unsigned z = 0; z < sizeZ; ++z) {
            for (unsigned x = 0; x < sizeX; ++x) {

                auto node = Node::MeshNode(mesh);

                static int ballNum = 0;
                node->name(std::format("ball {}", ++ballNum));

                const vec3 position = startPosition
                                      + vec3 {static_cast<float>(x) * step.x, static_cast<float>(y) * step.y,
                                              static_cast<float>(z) * step.z};

                node->position(position + uniform_linear(-positionVariance, positionVariance));

                node->eulerAngles(uniform_linear(vec3 {0.0f}, vec3 {TWO_PI}));

                auto body = PhysicsBody::DynamicBody(shape);
                body->mass(0.1f);
                body->friction(0.1f);
                body->restitution(0.6f);
                body->rollingFriction(0.01f);
                body->linearDamping(0.2f);
                body->angularDamping(0.3f);

                // body->autocalculatesMomentOfInertia(false);
                // body->momentOfInertia(body->momentOfInertia() * (5.0f / 3.0f));

                const float ANGULAR_VARIANCE = radians(90.0f);
                body->angularVelocity(uniform_linear(vec3 {-ANGULAR_VARIANCE}, vec3 {ANGULAR_VARIANCE}));

                node->physicsBody(std::move(body));

                added.push_back(node);
            }
        }
    }

    return added;
}

shared_ptr<Node> TransientsBuilder::BuildHammer(const TransientsCache::Entry& cacheEntry,
                                                const vec3&                   location,
                                                const vec3&                   velocity) {

    auto [mesh, shape] = cacheEntry;

    auto       node = Node::MeshNode(mesh);
    static int hammerNum = 0;
    node->name(std::format("hammer {}", ++hammerNum));
    node->position(location);

    auto body = PhysicsBody::DynamicBody(shape);
    body->mass(5.0f);
    body->restitution(0.10);
    body->friction(0.65f);
    body->rollingFriction(0.003);
    // body->spinningFriction(0.005);

    static const auto extent = mesh->localExtent();
    body->centerOfMass(body->centerOfMass() + extent * vec3 {0.0f, 0.3f, 0.0f});

    const vec3 throwForward = math::normalize(vec3 {velocity.x, 0.0f, velocity.z});
    // node forward is -Z, so yaw -Z toward the horizontal throw direction.
    const float        yaw = math::atan2(-throwForward.x, -throwForward.z);
    static const float START_PITCH = radians(20.0f);
    static const float START_PITCH_VARIANCE = radians(10.0f);
    static const float SIDE_TILT_VARIANCE = radians(8.0f);
    node->eulerAngles({START_PITCH + uniform_linear(-START_PITCH_VARIANCE, START_PITCH_VARIANCE), yaw,
                       uniform_linear(-SIDE_TILT_VARIANCE, SIDE_TILT_VARIANCE)});
    static const float SPIN_RATE = radians(540.0f);
    static const float SPIN_RATE_VARIANCE = radians(90.0f);
    const float        spinRate = SPIN_RATE + uniform_linear(-SPIN_RATE_VARIANCE, SPIN_RATE_VARIANCE);
    body->angularVelocity(-node->right() * spinRate);
    body->linearVelocity(velocity);

    node->physicsBody(std::move(body));

    return node;
}

shared_ptr<Node> TransientsBuilder::BuildHula(const TransientsCache::Entry& cacheEntry,
                                              const vec3&                   location,
                                              const vec3&                   velocity) {

    auto [mesh, shape] = cacheEntry;

    auto node = Node::MeshNode(mesh);

    static int hulaNum = 0;
    node->name(std::format("hula {}", ++hulaNum));
    node->position(location);

    auto body = PhysicsBody::DynamicBody(shape);

    body->mass(1.0f);
    body->friction(0.35f);
    body->restitution(0.3f);
    // body->rollingFriction(0.003);
    body->spinningFriction(0.05);
    // body->angularDamping(0.05f);

    static const auto extent = mesh->localExtent();
    body->centerOfMass(body->centerOfMass() + extent * vec3 {0.0f, .1f, 0.0f});

    // body->autocalculatesMomentOfInertia(false);
    // body->momentOfInertia(body->momentOfInertia() * 2.0f);

    static const float minExtent = math::min(extent);
    body->ccdMotionThreshold(minExtent * 0.25f);
    body->ccdSweptSphereRadius(minExtent * 0.20f);
    body->ccdEnabled(true);

    const vec3 up {0.0f, 1.0f, 0.0f};
    vec3       forward {0.0f, 0.0f, -1.0f};
    const vec3 horizontalVelocity {velocity.x, 0.0f, velocity.z};
    if (length(horizontalVelocity) > F32_COMPARE_EPSILON) {
        forward = normalize(horizontalVelocity);
    }
    const vec3  right = normalize(cross(forward, up));
    const float tilt = radians(uniform_linear(10.0f, 20.0f));
    const float bank = radians(uniform_linear(-5.0f, 5.0f));
    const auto  flatOrientation = quaternion({1.0f, 0.0f, 0.0f}, radians(-90.0f));
    const auto  tiltOrientation = quaternion(right, tilt);
    const auto  bankOrientation = quaternion(forward, bank);
    const auto  orientation = bankOrientation * tiltOrientation * flatOrientation;
    node->orientation(orientation);

    const vec3  spinAxis = -normalize(orientation * vec3 {0.0f, 0.0f, 1.0f});
    const float SPIN_RATE = radians(360.0f * 1.5f);
    body->angularVelocity(spinAxis * SPIN_RATE);

    body->linearVelocity(velocity);

    node->physicsBody(std::move(body));

    return node;
}

shared_ptr<Node> TransientsBuilder::BuildDuck(const TransientsCache::Entry& cacheEntry,
                                              const vec3&                   location,
                                              const vec3&                   velocity) {

    auto [mesh, shape] = cacheEntry;

    auto       node = Node::MeshNode(mesh);
    static int quackNum = 0;
    node->name(std::format("quack {}", ++quackNum));
    node->position(location);

    auto body = PhysicsBody::DynamicBody(shape);
    body->mass(1.0f);
    body->friction(0.6f);
    body->restitution(0.4f);
    body->rollingFriction(0.05);
    body->spinningFriction(0.05);
    // body->angularDamping(0.05f);

    static const auto extent = mesh->localExtent();
    body->centerOfMass(body->centerOfMass() + extent * vec3 {0.0f, -0.1f, 0.0f});

    // static const float minExtent = math::min(extent);
    // body->ccdMotionThreshold(minExtent * 0.25f);
    // body->ccdSweptSphereRadius(minExtent * 0.25f);
    // body->ccdEnabled(true);

    node->eulerAngles(uniform_linear(vec3 {0.0f}, vec3 {TWO_PI}));

    static const float ANGULAR_VARIANCE = radians(360.0f);
    body->angularVelocity(uniform_linear(vec3 {-ANGULAR_VARIANCE}, vec3 {ANGULAR_VARIANCE}));

    body->linearVelocity(velocity);

    node->physicsBody(std::move(body));

    return node;
}

// [Private Static Non-Member Functions]

pair<vec3, vec3> CalculateGridLayout(const vec3&   location,
                                     const vec3&   cellSize,
                                     const u8vec3& gridSize,
                                     float         gap) {

    const vec3 step = cellSize + vec3 {gap};

    const float totalX = cellSize.x * static_cast<float>(gridSize.x) + gap * static_cast<float>(gridSize.x - 1);
    const float totalZ = cellSize.z * static_cast<float>(gridSize.z) + gap * static_cast<float>(gridSize.z - 1);

    return {{location.x - totalX * 0.5f + cellSize.x * 0.5f, location.y + cellSize.y * 0.5f,
             location.z - totalZ * 0.5f + cellSize.z * 0.5f},
            step};
}
