//
//  HitTestResult.cc
//  avara3d
//
//  Created by Morgan Davis on 8/6/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/scene/HitTestResult.h"

using namespace std;

namespace a3d {
// [Public Member Functions]

shared_ptr<Node> HitTestResult::node() const {
    return _node.lock();
}

const MeshElement* HitTestResult::meshElement() const {
    return _meshElement;
}

optional<uint32_t> HitTestResult::faceIndex() const {
    return _faceIndex;
}

const math::vec3& HitTestResult::localCoordinates() const {
    return _localCoordinates;
}

const math::vec3& HitTestResult::worldCoordinates() const {
    return _worldCoordinates;
}

const math::vec3& HitTestResult::localNormal() const {
    return _localNormal;
}

const math::vec3& HitTestResult::worldNormal() const {
    return _worldNormal;
}

const math::mat4& HitTestResult::modelTransform() const {
    return _modelTransform;
}

// [Internal Lifecycle Functions]

HitTestResult::HitTestResult(weak_ptr<Node>     node,
                             shared_ptr<Mesh>   mesh,
                             const MeshElement* meshElement,
                             optional<uint32_t> faceIndex,
                             const math::vec3&  localCoordinates,
                             const math::vec3&  worldCoordinates,
                             const math::vec3&  localNormal,
                             const math::vec3&  worldNormal,
                             const math::mat4&  modelTransform):
    _node {std::move(node)},
    _mesh {std::move(mesh)},
    _meshElement {meshElement},
    _faceIndex {faceIndex},
    _localCoordinates {localCoordinates},
    _worldCoordinates {worldCoordinates},
    _localNormal {localNormal},
    _worldNormal {worldNormal},
    _modelTransform {modelTransform} {}
} // namespace a3d
