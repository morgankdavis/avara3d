//
//  PhysicsWorldProxy.h
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PROXY_PHYSICSWORLDPROXY_H
#define AVARA3D_PHYSICS_PROXY_PHYSICSWORLDPROXY_H

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "a3d/physics/PhysicsContact.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/scene/HitTestResult.h"
#include "a3d/scene/Scene.h"

namespace a3d {
class Line;
class PhysicsWorld;
class PhysicsBody;
class Profiler;
class Renderer;
class RenderContext;

class PhysicsWorldProxy {

public:
    // [Internal Types]

    enum class ContactEventType : uint8_t {
        Begin,
        Continue,
        End
    };

    struct ContactEvent {
        ContactEventType type;
        PhysicsContact   contact;
    };

    using ContactEvents = std::vector<ContactEvent>;

    // [Internal Lifecycle Functions]

    explicit PhysicsWorldProxy(PhysicsWorld& world);

    PhysicsWorldProxy(const PhysicsWorldProxy&)            = delete;
    PhysicsWorldProxy& operator=(const PhysicsWorldProxy&) = delete;

    PhysicsWorldProxy(PhysicsWorldProxy&&)            = delete;
    PhysicsWorldProxy& operator=(PhysicsWorldProxy&&) = delete;

    virtual ~PhysicsWorldProxy();

    // [Internal Member Functions]

    virtual void                          add(PhysicsBody& body)    = 0;
    virtual void                          remove(PhysicsBody& body) = 0;

    virtual math::vec3                    gravity() const                    = 0;
    virtual void                          gravity(const math::vec3& gravity) = 0;

    virtual const ContactEvents&          step(double deltaTime, Profiler& profiler) = 0;

    virtual std::optional<PhysicsContact> contactTest(const PhysicsBody& bodyA,
                                                      const PhysicsBody& bodyB) const = 0;
    virtual std::vector<PhysicsContact>   contactTest(const PhysicsBody& body) const  = 0;
    virtual std::vector<HitTestResult>    rayTest(const math::vec3& from,
                                                  const math::vec3& to,
                                                  HitTestSearchMode searchMode) const = 0;
    // ! NOT IMPLEMENTED !
    virtual std::vector<PhysicsContact>   convexSweepTest(const PhysicsShape& shape,
                                                          const math::mat4&   fromMat,
                                                          const math::mat4&   toMat,
                                                          HitTestSearchMode   searchMode) const = 0;

    virtual PhysicsWorld::Inventory       inventory() const = 0;

    virtual void appendDebugLines(std::vector<Line>& out, Scene::DebugOptions debugOptions) = 0;
};
} // namespace a3d

#endif // AVARA3D_PHYSICS_PROXY_PHYSICSWORLDPROXY_H
