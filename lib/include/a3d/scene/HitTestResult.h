//
//  HitTestResult.h
//  avara3d
//
//  Created by Morgan Davis on 8/6/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SCENE_HITTESTRESULT_H
#define AVARA3D_SCENE_HITTESTRESULT_H

#include <cstdint>
#include <memory>
#include <optional>

#include "a3d/Math.h"

namespace a3d {

    class Mesh;
    class MeshElement;
    class Node;

    enum class HitTestSearchMode : uint8_t {
        Any,
        Closest,
        All
    };

    class HitTestResult {

    public:
        /// Public Member Functions ///

        std::shared_ptr<Node>   node() const;

        const MeshElement*      meshElement() const;
        std::optional<uint32_t> faceIndex() const;

        const math::vec3&       localCoordinates() const;
        const math::vec3&       worldCoordinates() const;

        const math::vec3&       localNormal() const;
        const math::vec3&       worldNormal() const;

        const math::mat4&       modelTransform() const;

        /// Internal Lifecycle Functions ///

        HitTestResult(std::weak_ptr<Node>     node,
                      std::shared_ptr<Mesh>   mesh,
                      const MeshElement*      meshElement,
                      std::optional<uint32_t> faceIndex,
                      const math::vec3&       localCoordinates,
                      const math::vec3&       worldCoordinates,
                      const math::vec3&       localNormal,
                      const math::vec3&       worldNormal,
                      const math::mat4&       modelTransform);

    private:
        /// Private Member Variables ///

        std::weak_ptr<Node>     _node;

        // Keeps _meshElement alive for the lifetime of a visual hit result.
        std::shared_ptr<Mesh>   _mesh;
        const MeshElement*      _meshElement;
        std::optional<uint32_t> _faceIndex;

        math::vec3              _localCoordinates;
        math::vec3              _worldCoordinates;
        math::vec3              _localNormal;
        math::vec3              _worldNormal;
        math::mat4              _modelTransform;
    };

}

#endif //AVARA3D_SCENE_HITTESTRESULT_H
