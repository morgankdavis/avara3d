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
#include <optional>
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
 * does not extend the lifetime of its source object. The source and any mesh
 * geometry used to build the collision shape must remain alive for as long as
 * the PhysicsShape is in use.
 *
 * Supported collision shapes expose a configurable collision margin used for
 * collision detection and contact generation. The effective default margin is
 * shape-dependent. Changing the margin updates existing PhysicsBody objects
 * without regenerating the source geometry.
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

    /**
     * @brief Returns the collision margin used by this shape.
     *
     * If no explicit margin has been set, the effective margin is obtained from
     * the existing collision geometry. Compound collision geometry must use a
     * common margin for this function to return a value.
     *
     * @throws std::logic_error if this shape does not support a configurable
     * collision margin, if collision geometry has not yet been created and no
     * explicit margin has been set, or if the collision components do not use a
     * common margin.
     */
    float                                   margin() const;

    /**
     * @brief Sets a uniform collision margin for this shape.
     *
     * The exact geometric effect of the margin depends on the collision-shape
     * representation. Existing PhysicsBody objects sharing this shape are updated
     * immediately without regenerating the source geometry.
     *
     * InfinitePlanePhysicsShape, SpherePhysicsShape, and CapsulePhysicsShape do
     * not support a configurable collision margin.
     *
     * @throws std::logic_error if this shape does not support a configurable
     * collision margin.
     * @throws std::invalid_argument if @p margin is negative or non-finite.
     */
    void                                    margin(float margin);

    /** @brief Returns the weak Mesh or Node source, or std::monostate for a source-less primitive shape. */
    Source                                  source() const;

    // [Internal Member Functions]

    virtual bool                            supportsBodyType(PhysicsBody::Type type) const;
    virtual bool                            supportsMargin() const;

    void                                    attachedToBody(PhysicsBody& body);
    void                                    detachedFromBody(PhysicsBody& body);

    void                                    physicsWorldReachable(PhysicsWorld& world);
    void                                    physicsWorldUnreachable(PhysicsWorld& world);

    void                                    source(const Source& sourceObject);

    void                                    checkCreateProxy();

    const std::unordered_set<PhysicsBody*>& bodies() const;

    const std::optional<float>&             marginOverride() const;

    PhysicsShapeProxy*                      proxy() const;

protected:
    // [Protected Lifecycle]

    PhysicsShape();

    // [Protected Member Variables]

    Type                               _type;
    std::unique_ptr<PhysicsShapeProxy> _proxy;

private:
    // [Private Member Variables]

    std::optional<float>             _margin;
    Source                           _source;
    std::unordered_set<PhysicsBody*> _bodies;
};

}

#endif // AVARA3D_PHYSICS_SHAPE_PHYSICSSHAPE_H
