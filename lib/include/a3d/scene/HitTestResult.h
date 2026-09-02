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

// [Public Types]

/** @brief Controls how hit-test results are selected. */
enum class HitTestSearchMode : uint8_t {
    Any,     ///< Returns the first hit found, which is not necessarily the closest.
    Closest, ///< Returns only the closest hit.
    All      ///< Returns all hits ordered from closest to farthest.
};

/**
 * @brief Describes an intersection with scene geometry or a physics body.
 *
 * Coordinates, normals, and the model transform are snapshots from the time
 * of the hit test. Visual geometry hits may identify a MeshElement and triangle
 * face; physics hits do not necessarily have corresponding visual geometry.
 *
 * The result retains any visual Mesh needed to keep meshElement() alive, but
 * retains the hit Node weakly.
 */
class HitTestResult {

public:
    // [Public Member Functions]

    /** @brief Returns the hit node, or nullptr if it has since been destroyed. */
    std::shared_ptr<Node>   node() const;

    /** @brief Returns the hit visual mesh element, or nullptr when no visual mesh element is identified. */
    const MeshElement*      meshElement() const;

    /**
     * @brief Returns the zero-based triangle index within meshElement().
     *
     * @return Empty when the hit does not identify a specific triangle, such as
     *         a bounding-box-only visual hit or a physics hit.
     */
    std::optional<uint32_t> faceIndex() const;

    /** @brief Returns the hit position in the node's local coordinate system. */
    const math::vec3&       localCoordinates() const;

    /** @brief Returns the hit position in world coordinates. */
    const math::vec3&       worldCoordinates() const;

    /** @brief Returns the hit surface normal in the node's local coordinate system. */
    const math::vec3&       localNormal() const;

    /** @brief Returns the hit surface normal in world coordinates. */
    const math::vec3&       worldNormal() const;

    /** @brief Returns the local-to-world model transform used for this hit. */
    const math::mat4&       modelTransform() const;

    // [Internal Lifecycle Functions]

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
    // [Private Member Variables]

    std::weak_ptr<Node>     _node;

    // keeps _meshElement alive for the lifetime of a visual hit result
    std::shared_ptr<Mesh>   _mesh;
    const MeshElement*      _meshElement;
    std::optional<uint32_t> _faceIndex;

    math::vec3              _localCoordinates;
    math::vec3              _worldCoordinates;
    math::vec3              _localNormal;
    math::vec3              _worldNormal;
    math::mat4              _modelTransform;
};
} // namespace a3d

#endif // AVARA3D_SCENE_HITTESTRESULT_H
