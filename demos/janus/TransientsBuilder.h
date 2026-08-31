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

#include "TransientsCache.h"

#include "a3d/Math.h"

namespace a3d {

    class Node;

}

namespace demo::janus {

    class TransientsBuilder {

    public:
        // [Public Static Member Functions]

        static std::vector<std::shared_ptr<a3d::Node>> BuildRocks(const std::vector<TransientsCache::Entry>&
                                                                                           cacheEntries,
                                                                  const a3d::math::vec3&   location,
                                                                  const a3d::math::u8vec3& stackSize,
                                                                  float                    gap);
        static std::vector<std::shared_ptr<a3d::Node>> BuildCoins(const TransientsCache::Entry& cacheEntry,
                                                                  const a3d::math::vec3&        location,
                                                                  const a3d::math::u8vec3&      stackSize,
                                                                  float                         gap);
        static std::vector<std::shared_ptr<a3d::Node>> BuildBalls(const TransientsCache::Entry& cacheEntry,
                                                                  const a3d::math::vec3&        location,
                                                                  const a3d::math::u8vec3&      stackSize,
                                                                  float                         gap);
        static std::shared_ptr<a3d::Node>              BuildHammer(const TransientsCache::Entry& cacheEntry,
                                                                   const a3d::math::vec3&        location,
                                                                   const a3d::math::vec3&        velocity);
        static std::shared_ptr<a3d::Node>              BuildHula(const TransientsCache::Entry& cacheEntry,
                                                                 const a3d::math::vec3&        location,
                                                                 const a3d::math::vec3&        velocity);
        static std::shared_ptr<a3d::Node>              BuildDuck(const TransientsCache::Entry& cacheEntry,
                                                                 const a3d::math::vec3&        location,
                                                                 const a3d::math::vec3&        velocity);
    };

}

#endif // AVARA3D_DEMO_JANUS_TRANSIENTSBUILDER_H
