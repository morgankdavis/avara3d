//
//  Node.h
//  avara3d
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SCENE_NODE_H
#define AVARA3D_SCENE_NODE_H

#include <climits>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "a3d/mesh/AABB.h"
#include "a3d/util/Bitmask.h"

namespace a3d {

class Camera;
class Light;
class Mesh;
class Renderer;
class RenderContext;
class RenderItem;
class Scene;
class PhysicsWorld;
class PhysicsBody;
class VisualWorld;

/**
 * @brief A transformable object in a Scene hierarchy.
 *
 * A Node may contain a Mesh, Light, Camera, and PhysicsBody, and may have
 * child nodes. Transform properties are expressed relative to the parent;
 * world-space accessors include the transforms of all ancestors.
 *
 * Child nodes are retained with shared ownership. A node retains its parent
 * weakly, so retaining a child does not keep its parent hierarchy alive.
 */
class Node : public std::enable_shared_from_this<Node> {

public:
    // [Public Types]

    // TODO: probably move these
    /** @brief Debug visualization options associated with a node. */
    enum class DebugOptions : uint32_t {
        None              = 0,      ///< No node debug visualization.
        ShowHighlightBox  = 1 << 0, ///< Show the node highlight box.
        ShowHighlightTint = 1 << 1  ///< Tint the node for highlighting.
    };

    // [Public Static Member Functions]

    /** @brief Creates a new node with @p name. */
    static std::shared_ptr<Node> NamedNode(const std::string& name);

    /** @brief Creates a new node containing @p geometry. */
    static std::shared_ptr<Node> MeshNode(const std::shared_ptr<Mesh>& geometry);

    /** @brief Creates a new node containing @p light. */
    static std::shared_ptr<Node> LightNode(const std::shared_ptr<Light>& light);

    /** @brief Creates a new node containing @p camera. */
    static std::shared_ptr<Node> CameraNode(const std::shared_ptr<Camera>& camera);

    // [Public Lifecycle Functions]

    /** @brief Creates an empty node with an identity transform. */
    Node();

    /** @brief Creates a node with @p name and an identity transform. */
    explicit Node(const std::string& name);

    /** @brief Creates a node containing @p mesh and an identity transform. */
    explicit Node(const std::shared_ptr<Mesh>& mesh);

    /** @brief Creates a node containing @p light and an identity transform. */
    explicit Node(const std::shared_ptr<Light>& light);

    /** @brief Creates a node containing @p camera and an identity transform. */
    explicit Node(const std::shared_ptr<Camera>& camera);

    Node(const Node&)            = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&)            = delete;
    Node& operator=(Node&&) = delete;

    ~Node();

    // [Public Member Functions]

    /** @brief Returns the optional node name. */
    const std::optional<std::string>& name() const;

    /** @brief Sets the node name. */
    void                              name(const std::string& name);

    /** @brief Returns the mesh attached to this node, or nullptr if none is attached. */
    const std::shared_ptr<Mesh>&      mesh() const;

    /**
     * @brief Replaces the mesh attached to this node.
     *
     * Passing nullptr removes the current Mesh. If this node owns a PhysicsBody
     * whose PhysicsShape was automatically derived by A3D, the collision shape is
     * rebuilt from the updated Mesh or node hierarchy. An explicitly assigned
     * PhysicsShape is left unchanged.
     *
     * If an explicitly assigned PhysicsShape derives from the previous Mesh, the
     * caller must keep that Mesh alive as required by the PhysicsShape source-lifetime
     * contract.
     *
     * @throws std::invalid_argument if automatic collision-shape creation is required
     * and @p mesh contains no elements.
     */
    void                              mesh(const std::shared_ptr<Mesh>& mesh);

    /** @brief Returns the light attached to this node, or nullptr if none is attached. */
    const std::shared_ptr<Light>&     light() const;

    /** @brief Replaces the light attached to this node; nullptr removes it. */
    void                              light(const std::shared_ptr<Light>& light);

    /** @brief Returns the camera attached to this node, or nullptr if none is attached. */
    const std::shared_ptr<Camera>&    camera() const;

    /** @brief Replaces the camera attached to this node; nullptr removes it. */
    void                              camera(const std::shared_ptr<Camera>& camera);

    /** @brief Returns the physics body owned by this node, or nullptr if none is attached. */
    PhysicsBody*                      physicsBody() const;

    /**
     * @brief Replaces the physics body owned by this node; nullptr removes it.
     *
     * If @p body has no PhysicsShape, A3D may automatically derive one from this
     * node's Mesh or hierarchy.
     *
     * @throws std::invalid_argument if automatic collision-shape creation is required
     * from a Mesh with no elements.
     */
    void                              physicsBody(std::unique_ptr<PhysicsBody> body);

    /** @brief Returns the node position in parent coordinates. */
    const math::vec3&                 position() const;

    /** @brief Sets the node position in parent coordinates. */
    void                              position(const math::vec3& position);

    /**
     * @brief Returns the node rotation in parent coordinates.
     *
     * @return An axis-angle vector with xyz containing the axis and w containing
     *         the angle in radians.
     */
    math::vec4                        rotation() const;

    /** @brief Sets the node rotation from @p axis and @p angle in radians. */
    void                              rotation(const math::vec3& axis, float angle);

    /** @brief Returns the node Euler angles in parent coordinates as pitch, yaw, and roll in radians. */
    math::vec3                        eulerAngles() const;

    /** @brief Sets the node Euler angles as pitch, yaw, and roll in radians. */
    void                              eulerAngles(const math::vec3& angles);

    /** @brief Returns the node orientation in parent coordinates as a quaternion. */
    const math::quat&                 orientation() const;

    /** @brief Sets the node orientation in parent coordinates. */
    void                              orientation(const math::quat& orientation);

    /** @brief Returns the node scale relative to its parent. */
    const math::vec3&                 scale() const;

    /** @brief Sets the node scale relative to its parent. */
    void                              scale(const math::vec3& scale);

    /** @brief Returns the node's local -Z axis expressed in parent coordinates. */
    math::vec3                        forward() const;

    /** @brief Returns the node's local +Y axis expressed in parent coordinates. */
    math::vec3                        up() const;

    /** @brief Returns the node's local +X axis expressed in parent coordinates. */
    math::vec3                        right() const;

    /** @brief Returns the node transform from local coordinates to parent coordinates. */
    math::mat4                        transform() const;

    /** @brief Sets the node transform from local coordinates to parent coordinates. */
    void                              transform(const math::mat4& transform);

    /** @brief Returns the node position in world coordinates. */
    math::vec3                        worldPosition() const;

    /**
     * @brief Returns the node world rotation.
     *
     * @return An axis-angle vector with xyz containing the axis and w containing
     *         the angle in radians.
     */
    math::vec4                        worldRotation() const;

    /** @brief Returns the node world Euler angles as pitch, yaw, and roll in radians. */
    math::vec3                        worldEulerAngles() const;

    /** @brief Returns the node orientation in world coordinates. */
    math::quat                        worldOrientation() const;

    /** @brief Returns the effective node scale in world coordinates. */
    math::vec3                        worldScale() const;

    /** @brief Returns the node's local -Z axis expressed in world coordinates. */
    math::vec3                        worldForward() const;

    /** @brief Returns the node's local +Y axis expressed in world coordinates. */
    math::vec3                        worldUp() const;

    /** @brief Returns the node's local +X axis expressed in world coordinates. */
    math::vec3                        worldRight() const;

    /** @brief Returns the transform from this node's local coordinates to world coordinates. */
    math::mat4                        worldTransform() const;

    /** @brief Converts @p pos from @p from local coordinates into this node's local coordinates. */
    math::vec3                        convertFrom(const math::vec3& pos, const Node& from);

    /** @brief Converts @p pos from this node's local coordinates into @p to local coordinates. */
    math::vec3                        convertTo(const math::vec3& pos, const Node& to);

    /** @brief Converts @p t from @p from local coordinates into this node's local coordinates. */
    math::mat4                        convertFrom(const math::mat4& t, const Node& from);

    /** @brief Converts @p t from this node's local coordinates into @p to local coordinates. */
    math::mat4                        convertTo(const math::mat4& t, const Node& to);

    /**
     * @brief Adds @p node as a child of this node.
     *
     * If @p reparent is true and @p node already has a parent, it is detached from
     * that parent before being added. If @p reparent is false, an already-parented
     * node is rejected.
     *
     * @param node node to add.
     * @param reparent when true, automatically detaches @p node from its current parent.
     *
     * @throws std::invalid_argument if @p node is nullptr, is this node, is an
     * ancestor of this node, is already contained in this node's subtree without
     * reparenting, or is already attached to another parent when @p reparent is false.
     */
    void                              addChild(const std::shared_ptr<Node>& node, bool reparent = false);

    /**
     * @brief Adds each node in @p nodes as a child, in order.
     *
     * The same validation and reparenting behavior as addChild() applies to each
     * node. If adding a node throws, nodes added earlier in @p nodes remain attached.
     *
     * @param nodes nodes to add.
     * @param reparent when true, automatically detaches each node from its current parent.
     *
     * @throws std::invalid_argument if a node is nullptr, is this node, is an
     * ancestor of this node, is already contained in this node's subtree without
     * reparenting, or is already attached to another parent when @p reparent is false.
     */
    void addChildren(const std::vector<std::shared_ptr<Node>>& nodes, bool reparent = false);

    /** @brief Detaches this node from its current parent, if it has one. */
    void removeFromParent();

    /**
     * @brief Returns this node's children.
     *
     * @param recursive when true, returns all descendants; otherwise returns direct children only.
     * @return Child nodes in hierarchy traversal order.
     */
    std::vector<std::shared_ptr<Node>> children(bool recursive = false) const;

    /**
     * @brief Finds a child with @p name.
     *
     * @param name name to match.
     * @param recursive when true, searches all descendants; otherwise searches direct children only.
     * @return The first matching child, or nullptr if no match is found.
     */
    std::shared_ptr<Node>              childNamed(const std::string& name, bool recursive = false) const;

    /** @brief Returns true when this node is hidden from rendering. */
    bool                               hidden() const;

    /** @brief Sets whether this node is hidden from rendering. */
    void                               hidden(bool hidden);

    /** @brief Returns the node render-order value. */
    int                                renderOrder() const;

    /** @brief Sets the node render-order value. */
    void                               renderOrder(int order);

    /** @brief Returns the enabled node debug visualization options. */
    DebugOptions                       debugOptions() const;

    /** @brief Sets the enabled node debug visualization options. */
    void                               debugOptions(DebugOptions options);

    /** @brief Returns the Scene containing this node, or nullptr if the node is not attached to a Scene. */
    Scene*                             scene() const;

    /** @brief Returns the current parent, or an empty weak pointer for a root or detached node. */
    std::weak_ptr<Node>                parent() const;

    // [Internal Types]

    enum class DirtyMask : uint32_t {
        None           = 0,
        WorldTransform = 1 << 0,
        All            = UINT_MAX
    };

    // [Internal Member Functions]

    void          attachedToParent(Node& parent);
    void          detachedFromParent(Node& parent);

    void          attachedToScene(Scene& scene);
    void          detachedFromScene(Scene& scene);

    void          ancestorAttachedToParent(Node& ancestor, Node& parent);
    void          ancestorDetachedFromParent(Node& ancestor, Node& parent);

    void          ancestorAttachedToScene(Node& ancestor, Scene& scene);
    void          ancestorDetachedFromScene(Node& ancestor, Scene& scene);

    void          visualWorldAttachedToScene(VisualWorld& world, Scene& scene);
    void          visualWorldDetachedFromScene(VisualWorld& world, Scene& scene);

    void          physicsWorldAttachedToScene(PhysicsWorld& world, Scene& scene);
    void          physicsWorldDetachedFromScene(PhysicsWorld& world, Scene& scene);

    VisualWorld*  visualWorld() const;
    PhysicsWorld* physicsWorld() const;

    void          checkNotifyPhysicsBodyOfReachablePhysicsWorld() const;
    void          checkNotifyPhysicsBodyOfUnreachablePhysicsWorld() const;

    bool          containsChild(const std::shared_ptr<Node>& node);

    AABB          aabb(bool vertfit = false) const; // recursive world AABB
    math::vec3    extent(bool vertfit = false) const; // recursive world extent

    void          applyPhysicsTransform(const math::mat4& transform);

    void          _debugPrint();
    void          _debugPrintRec(Node& node, unsigned level);

private:
    // [Private Member Functions]

    void                               getAABBRec(AABB& aabb);

    void                               setTransformComponents(const math::mat4& transform);
    void                               syncPhysicsTransforms();

    std::vector<std::shared_ptr<Node>> children(const Node& root) const;
    void childrenRec(const std::shared_ptr<Node>& node, std::vector<std::shared_ptr<Node>>& children) const;

    DirtyMask                          dirtyMask() const;
    void                               dirtyMask(DirtyMask mask);

    // [Private Member Variables]

    std::optional<std::string>         _name;
    std::shared_ptr<Light>             _light;
    std::shared_ptr<Camera>            _camera;
    std::shared_ptr<Mesh>              _mesh;
    std::vector<std::shared_ptr<Node>> _children;
    math::vec3                         _position;
    math::quat                         _orientation;
    math::vec3                         _scale;
    mutable std::optional<math::vec3>  _eulerAngles;
    std::unique_ptr<PhysicsBody>       _physicsBody;
    bool                               _hidden;
    int                                _renderOrder;
    DebugOptions                       _debugOptions;
    Scene*                             _scene;
    std::weak_ptr<Node>                _parent;
    DirtyMask                          _dirtyMask;
};

namespace util::bitmask {

    template<>
    struct enable_ops<Node::DebugOptions> : std::true_type {};

    template<>
    struct enable_ops<Node::DirtyMask> : std::true_type {};

} // namespace util::bitmask
} // namespace a3d

#endif // AVARA3D_SCENE_NODE_H
