//
//  PhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PHYSICSSHAPE_H

#include <cstdint>
#include <memory>
#include <unordered_set>
#include <variant>

#include "a3d/physics/PhysicsBody.h"

namespace a3d {

    class Mesh;
    class Node;
    class PhysicsWorld;
    class PhysicsShapeProxy;
    class Scene;

    class PhysicsShape {

    public:
        /// Public Types ///

        enum class Type : uint8_t {
            Primitive, // eh, do something else
            BoundingBox,
            ConvexHull,
            ConcavePolyhedron
        };

        using Source = std::variant<std::monostate, std::weak_ptr<Mesh>, std::weak_ptr<Node>>;

        // TODO: static MeshShape(), NodeShape()

        /// Public Lifecycle Functions ///

        PhysicsShape(Type type, const std::shared_ptr<Mesh>& mesh);
        PhysicsShape(Type type, const std::shared_ptr<Node>& node);

        PhysicsShape(const PhysicsShape&)            = delete;
        PhysicsShape& operator=(const PhysicsShape&) = delete;

        PhysicsShape(PhysicsShape&&)            = delete;
        PhysicsShape& operator=(PhysicsShape&&) = delete;

        virtual ~PhysicsShape();

        /// Public Member Functions ///

        virtual Type                            type() const;
        virtual void                            type(Type type);

        Source                                  source() const;

        /// Internal Member Functions ///

        virtual bool                            supportsBodyType(PhysicsBody::Type type) const;

        void                                    attachedToBody(PhysicsBody& body);
        void                                    detachedFromBody(PhysicsBody& body);

        void                                    physicsWorldReachable(PhysicsWorld& world);
        void                                    physicsWorldUnreachable(PhysicsWorld& world);

        void                                    source(const Source& sourceObject);

        void                                    checkCreateProxy();

        const std::unordered_set<PhysicsBody*>& bodies() const;

        PhysicsShapeProxy*                      proxy() const;

    protected:
        /// Protected Lifecycle ///

        PhysicsShape();

        /// Protected Member Variables ///

        Type                               _type;
        std::unique_ptr<PhysicsShapeProxy> _proxy;

    private:
        /// Private Member Variables ///

        Source                           _source;
        std::unordered_set<PhysicsBody*> _bodies;
    };

}

#endif /* AVARA3D_PHYSICS_SHAPE_PHYSICSSHAPE_H */
