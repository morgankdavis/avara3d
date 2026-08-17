//
//  Node.h
//  avara3d
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
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

    class Node : public std::enable_shared_from_this<Node> {

    public:
        /// Public Types ///

        // TODO: probably move these
        enum class DebugOptions : uint32_t {
            None              = 0,
            ShowHighlightBox  = 1 << 0,
            ShowHighlightTint = 1 << 1
        };

        /// Public Static Member Functions ///

        static std::shared_ptr<Node> NamedNode(const std::string& name);
        static std::shared_ptr<Node> MeshNode(const std::shared_ptr<Mesh>& geometry);
        static std::shared_ptr<Node> LightNode(const std::shared_ptr<Light>& light);
        static std::shared_ptr<Node> CameraNode(const std::shared_ptr<Camera>& camera);

        /// Public Lifecycle Functions ///

        Node();
        explicit Node(const std::string& name);
        explicit Node(const std::shared_ptr<Mesh>& mesh);
        explicit Node(const std::shared_ptr<Light>& light);
        explicit Node(const std::shared_ptr<Camera>& camera);

        Node(const Node&)            = delete;
        Node& operator=(const Node&) = delete;

        Node(Node&&)            = delete;
        Node& operator=(Node&&) = delete;

        ~Node();

        /// Public Member Functions ///

        const std::optional<std::string>&  name() const;
        void                               name(const std::string& name);

        const std::shared_ptr<Light>&      light() const;
        void                               light(const std::shared_ptr<Light>& light);

        const std::shared_ptr<Camera>&     camera() const;
        void                               camera(const std::shared_ptr<Camera>& camera);

        const std::shared_ptr<Mesh>&       mesh() const;
        void                               mesh(const std::shared_ptr<Mesh>& mesh);

        const math::vec3&                  position() const;
        void                               position(const math::vec3& position);

        math::vec4                         rotation() const; // axis-angle
        void                               rotation(const math::vec3& axis, float angle);

        math::vec3                         eulerAngles() const; // pitch, yaw, roll
        void                               eulerAngles(const math::vec3& angles);

        const math::quat&                  orientation() const; // wxyz
        void                               orientation(const math::quat& orientation);

        const math::vec3&                  scale() const;
        void                               scale(const math::vec3& scale);

        math::vec3                         forward() const;
        math::vec3                         up() const;
        math::vec3                         right() const;

        math::mat4                         transform() const;
        void                               transform(const math::mat4& transform);

        math::vec3                         worldPosition() const;
        math::vec4                         worldRotation() const; // axis-angle
        math::vec3                         worldEulerAngles() const; // pitch, yaw, roll
        math::quat                         worldOrientation() const; // wxyz
        math::vec3                         worldScale() const;

        math::vec3                         worldForward() const;
        math::vec3                         worldUp() const;
        math::vec3                         worldRight() const;

        math::mat4                         worldTransform() const;

        math::vec3                         convertFrom(const math::vec3& pos, const Node& from);
        math::vec3                         convertTo(const math::vec3& pos, const Node& to);
        math::mat4                         convertFrom(const math::mat4& t, const Node& from);
        math::mat4                         convertTo(const math::mat4& t, const Node& to);

        void                               addChild(const std::shared_ptr<Node>& node);
        void                               addChildren(const std::vector<std::shared_ptr<Node>>& nodes);
        void                               removeFromParent();

        std::vector<std::shared_ptr<Node>> children(bool resursive = false) const;
        std::shared_ptr<Node>              childNamed(const std::string& name, bool resursive = false) const;

        PhysicsBody*                       physicsBody() const;
        void                               physicsBody(std::unique_ptr<PhysicsBody> body);

        bool                               hidden() const;
        void                               hidden(bool hidden);

        int                                renderOrder() const;
        void                               renderOrder(int order);

        DebugOptions                       debugOptions() const;
        void                               debugOptions(DebugOptions options);

        Scene*                             scene() const;

        std::weak_ptr<Node>                parent() const;

        /// Internal Types ///

        enum class DirtyMask : uint32_t {
            None           = 0,
            WorldTransform = 1 << 0,
            All            = UINT_MAX
        };

        /// Internal Member Functions ///

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
        /// Private Member Functions ///

        void                               getAABBRec(AABB& aabb);

        void                               setTransformComponents(const math::mat4& transform);
        void                               syncPhysicsTransforms();

        std::vector<std::shared_ptr<Node>> children(const Node& root) const;
        void childrenRec(const std::shared_ptr<Node>& node, std::vector<std::shared_ptr<Node>>& children) const;

        DirtyMask                          dirtyMask() const;
        void                               dirtyMask(DirtyMask mask);

        /// Private Member Variables ///

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

    }
}

#endif /* AVARA3D_SCENE_NODE_H */
