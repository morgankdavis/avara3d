//
//  PhysicsWorldProxy.h
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PROXY_PHYSICSWORLDPROXY_H
#define AVARA3D_PHYSICS_PROXY_PHYSICSWORLDPROXY_H

#include <cstdint>
#include <memory>
#include <vector>

#include "a3d/physics/PhysicsContact.h"
#include "a3d/physics/PhysicsInventory.h"
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
        /// Internal Types ///

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

        /// Internal Lifecycle Functions ///

        explicit PhysicsWorldProxy(PhysicsWorld& world);

        PhysicsWorldProxy(const PhysicsWorldProxy&)            = delete;
        PhysicsWorldProxy& operator=(const PhysicsWorldProxy&) = delete;

        PhysicsWorldProxy(PhysicsWorldProxy&&)            = delete;
        PhysicsWorldProxy& operator=(PhysicsWorldProxy&&) = delete;

        virtual ~PhysicsWorldProxy();

        /// Internal Member Functions ///

        virtual void                       add(PhysicsBody& body)    = 0;
        virtual void                       remove(PhysicsBody& body) = 0;

        virtual math::vec3                 gravity() const                    = 0;
        virtual void                       gravity(const math::vec3& gravity) = 0;

        virtual bool                       acceptsStepDelta(double deltaTime) const = 0;

        virtual const ContactEvents&       step(double deltaTime, Profiler& profiler) = 0;

        virtual std::vector<HitTestResult> rayTest(const math::vec3& from,
                                                   const math::vec3& to,
                                                   HitTestSearchMode searchMode) const = 0;

        virtual PhysicsInventory           inventory() const = 0;

        virtual void                       updateCollisionPairs() = 0;

        virtual void appendDebugLines(std::vector<Line>& out, Scene::DebugOptions debugOptions) = 0;
    };

}

#endif //AVARA3D_PHYSICS_PROXY_PHYSICSWORLDPROXY_H
