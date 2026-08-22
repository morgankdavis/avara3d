//
//  PhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
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

    /**
     * @brief Describes collision geometry used by one or more PhysicsBody objects.
     *
     * A PhysicsShape may derive its collision geometry from a Mesh, from the mesh
     * hierarchy beneath a Node, or from a specialized primitive PhysicsShape
     * subclass. Mesh and Node sources are retained weakly, so constructing a shape
     * does not extend the lifetime of its source object.
     */
    class PhysicsShape {

    public:
        // [Public Types]

        /** @brief Collision-geometry representation requested for a PhysicsShape. */
        enum class Type : uint8_t {
            // TODO: change this
            Primitive, ///< Reserved for internal use; public callers should not supply this value. This API is expected to change.
            BoundingBox, ///< Bounding-box collision geometry derived from the source geometry.
            ConvexHull, ///< Convex-hull collision geometry derived from the source geometry.
            ConcavePolyhedron ///< Potentially concave polygonal collision geometry derived from the source geometry.
        };

        /** @brief Weak source object used to derive collision geometry, or no source for primitive shapes. */
        using Source = std::variant<std::monostate, std::weak_ptr<Mesh>, std::weak_ptr<Node>>;

        // [Public Static Member Functions]

        /** @brief Creates a bounding-box collision shape derived from @p mesh. */
        static std::shared_ptr<PhysicsShape> BoundingBoxShape(const std::shared_ptr<Mesh>& mesh);

        /** @brief Creates a bounding-box collision shape from mesh geometry in the @p node hierarchy. */
        static std::shared_ptr<PhysicsShape> BoundingBoxShape(const std::shared_ptr<Node>& node);

        /** @brief Creates a convex-hull collision shape derived from @p mesh. */
        static std::shared_ptr<PhysicsShape> ConvexHullShape(const std::shared_ptr<Mesh>& mesh);

        /** @brief Creates a convex-hull collision shape from mesh geometry in the @p node hierarchy. */
        static std::shared_ptr<PhysicsShape> ConvexHullShape(const std::shared_ptr<Node>& node);

        /** @brief Creates a concave-polyhedron collision shape derived from @p mesh. */
        static std::shared_ptr<PhysicsShape> ConcavePolyhedronShape(const std::shared_ptr<Mesh>& mesh);

        /** @brief Creates a concave-polyhedron collision shape from mesh geometry in the @p node hierarchy. */
        static std::shared_ptr<PhysicsShape> ConcavePolyhedronShape(const std::shared_ptr<Node>& node);

        // [Public Lifecycle Functions]

        /**
         * @brief Creates a PhysicsShape of @p type derived from @p mesh.
         *
         * The source Mesh is retained weakly and must be non-null.
         */
        PhysicsShape(Type type, const std::shared_ptr<Mesh>& mesh);

        /**
         * @brief Creates a PhysicsShape of @p type from mesh geometry in the @p node hierarchy.
         *
         * The source Node is retained weakly and must be non-null. Descendant mesh
         * geometry is included using the descendants' local transforms.
         */
        PhysicsShape(Type type, const std::shared_ptr<Node>& node);

        PhysicsShape(const PhysicsShape&)            = delete;
        PhysicsShape& operator=(const PhysicsShape&) = delete;

        PhysicsShape(PhysicsShape&&)            = delete;
        PhysicsShape& operator=(PhysicsShape&&) = delete;

        virtual ~PhysicsShape();

        // [Public Member Functions]

        /** @brief Returns the collision-geometry type. */
        virtual Type                            type() const;

        /**
         * @brief Validates the requested collision-geometry type.
         *
         * Changing a PhysicsShape type after construction is not currently supported.
         *
         * @throws std::logic_error if @p type differs from the current type.
         */
        virtual void                            type(Type type);

        /** @brief Returns the weak Mesh or Node source, or std::monostate for a source-less primitive shape. */
        Source                                  source() const;

        // [Internal Member Functions]

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
        // [Protected Lifecycle]

        PhysicsShape();

        // [Protected Member Variables]

        Type                               _type;
        std::unique_ptr<PhysicsShapeProxy> _proxy;

    private:
        // [Private Member Variables]

        Source                           _source;
        std::unordered_set<PhysicsBody*> _bodies;
    };

}

#endif // AVARA3D_PHYSICS_SHAPE_PHYSICSSHAPE_H
