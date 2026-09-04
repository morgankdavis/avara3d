//
//  TransientsBuilder.h
//  janus
//
//  Created by Morgan Davis on 8/30/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DEMO_JANUS_TRANSIENTSBUILDER_H
#define AVARA3D_DEMO_JANUS_TRANSIENTSBUILDER_H

#include <memory>
#include <vector>

#include "a3d/Math.h"

#include "TransientsCache.h"

namespace a3d {

class Node;

} // namespace a3d

namespace demo::janus {

class TransientsBuilder {

public:
    // [Public Member Functions]

    void init(const a3d::Node& assetsRoot);

    std::vector<std::shared_ptr<a3d::Node>> rocks(const a3d::math::vec3&   location,
                                                  const a3d::math::u8vec3& stackSize,
                                                  float                    gap);
    std::vector<std::shared_ptr<a3d::Node>> coins(const a3d::math::vec3&   location,
                                                  const a3d::math::u8vec3& stackSize,
                                                  float                    gap);
    std::vector<std::shared_ptr<a3d::Node>> balls(const a3d::math::vec3&   location,
                                                  const a3d::math::u8vec3& stackSize,
                                                  float                    gap);
    std::shared_ptr<a3d::Node> hammer(const a3d::math::vec3& location, const a3d::math::vec3& velocity);
    std::shared_ptr<a3d::Node> hula(const a3d::math::vec3& location, const a3d::math::vec3& velocity);
    std::shared_ptr<a3d::Node> duck(const a3d::math::vec3& location, const a3d::math::vec3& velocity);

private:
    // [Private Member Variables]

    TransientsCache _cache;
};

} // namespace demo::janus

#endif // AVARA3D_DEMO_JANUS_TRANSIENTSBUILDER_H
