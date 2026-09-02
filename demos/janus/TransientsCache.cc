//
//  TransientsCache.cc
//  janus
//
//  Created by Morgan Davis on 8/29/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "TransientsCache.h"

#include <utility>

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

namespace demo::janus {
namespace {

    // [Private Constants]

    const vec3  ROCK_FIT_SIZE {0.35f, 0.35f, 0.35f};
    const float COIN_MAX_DIM {0.35f};
    const float BEACHBALL_MAX_DIM {0.45f};
    const float HAMMER_LENGTH {1.0f};
    const float HULA_DIAMETER {1.1f};
    const float DUCK_HEIGHT {0.5f};

} // namespace

// [Public Member Functions]

void TransientsCache::init(const Node& transientAssetsRoot) {

    initRocks(*transientAssetsRoot.childNamed("rocks"));
    initCoin(*transientAssetsRoot.childNamed("coin"));
    initBall(*transientAssetsRoot.childNamed("ball"));
    initHammer(*transientAssetsRoot.childNamed("hammer"));
    initHula(*transientAssetsRoot.childNamed("hula"));
    initDuck(*transientAssetsRoot.childNamed("quack"));
}

const vector<TransientsCache::Entry>& TransientsCache::rocks() const {
    return _rocks;
}

const TransientsCache::Entry& TransientsCache::coin() const {
    return _coin;
}

const TransientsCache::Entry& TransientsCache::ball() const {
    return _ball;
}

const TransientsCache::Entry& TransientsCache::hammer() const {
    return _hammer;
}

const TransientsCache::Entry& TransientsCache::hula() const {
    return _hula;
}

const TransientsCache::Entry& TransientsCache::duck() const {
    return _duck;
}

// [Private Member Functions]

void TransientsCache::initRocks(const Node& rocksRoot) {

    const auto nodes = rocksRoot.children();

    _rocks.clear();
    _rocks.reserve(nodes.size());

    for (const auto& node : nodes) {

        auto mesh = node->mesh();
        if (!mesh) {
            continue;
        }

        mesh->burnTransform(util::geom::fit_inside(mesh->localAABB(), ROCK_FIT_SIZE), true);

        auto physicsShape = PhysicsShape::ConvexHullShape(mesh);

        { // force shape backend creation now
            auto body = PhysicsBody::DynamicBody(physicsShape);
        }

        _rocks.push_back(Entry {.mesh = std::move(mesh), .physicsShape = std::move(physicsShape)});
    }
}

void TransientsCache::initCoin(const Node& node) {

    auto mesh = node.mesh();

    const float scaleFactor = COIN_MAX_DIM / math::max(mesh->localExtent());

    auto transform = math::rotate(mat4(1.0f), radians(90.0f), vec3 {1.0f, 0.0f, 0.0f});
    transform = math::scale(transform, vec3(scaleFactor));

    mesh->burnTransform(transform, true);

    // after rotation:
    // X = diameter
    // Y = thickness
    // Z = diameter
    const auto  extent = mesh->localExtent();
    const float radius = math::max(extent.x, extent.z) * 0.5f;
    const float height = extent.y;
    auto        physicsShape = make_shared<CylinderPhysicsShape>(radius, height);

    { // force shape backend creation now
        auto body = PhysicsBody::DynamicBody(physicsShape);
    }

    mesh->firstMaterial()->specular(Color::LightGray());
    mesh->firstMaterial()->specularExponent(16.0f);

    _coin = Entry {.mesh = std::move(mesh), .physicsShape = std::move(physicsShape)};
}

void TransientsCache::initBall(const Node& node) {

    auto mesh = node.mesh();

    const float scaleFactor = BEACHBALL_MAX_DIM / math::max(mesh->localExtent());
    const auto  transform = math::scale(mat4(1.0f), vec3(scaleFactor));
    mesh->burnTransform(transform, true);

    auto physicsShape = make_shared<SpherePhysicsShape>(BEACHBALL_MAX_DIM / 2.0f);

    { // force shape backend creation now
        auto body = PhysicsBody::DynamicBody(physicsShape);
    }

    mesh->firstMaterial()->specular(Color::LightGray());
    mesh->firstMaterial()->specularExponent(16.0f);

    _ball = Entry {.mesh = std::move(mesh), .physicsShape = std::move(physicsShape)};
}

void TransientsCache::initHammer(const Node& node) {

    auto mesh = node.mesh();

    const float scaleFactor = HAMMER_LENGTH / math::max(mesh->localExtent());
    auto        transform = math::rotate(mat4(1.0f), radians(90.0f), vec3 {1.0f, 0.0f, 0.0f});
    transform = math::scale(transform, scaleFactor);
    mesh->burnTransform(transform, true);

    auto physicsShape = PhysicsShape::ConvexHullShape(mesh);

    { // force shape backend creation now
        auto body = PhysicsBody::DynamicBody(physicsShape);
    }

    _hammer = Entry {.mesh = std::move(mesh), .physicsShape = std::move(physicsShape)};
}

void TransientsCache::initHula(const Node& node) {

    auto mesh = node.mesh();

    const float scaleFactor = HULA_DIAMETER / mesh->localExtent().y;
    mesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);

    auto physicsShape = PhysicsShape::ConcavePolyhedronShape(mesh);

    { // force shape backend creation now
        auto body = PhysicsBody::DynamicBody(physicsShape);
    }

    _hula = Entry {.mesh = std::move(mesh), .physicsShape = std::move(physicsShape)};
}

void TransientsCache::initDuck(const Node& node) {

    auto mesh = node.mesh();

    const float scaleFactor = DUCK_HEIGHT / mesh->localExtent().y;
    mesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);

    auto physicsShape = PhysicsShape::ConvexHullShape(mesh);

    { // force shape backend creation now
        auto body = PhysicsBody::DynamicBody(physicsShape);
    }

    _duck = Entry {.mesh = std::move(mesh), .physicsShape = std::move(physicsShape)};
}

} // namespace demo::janus
