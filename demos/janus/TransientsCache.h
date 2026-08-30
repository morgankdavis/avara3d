//
//  TransientsCache.h
//  janus
//
//  Created by Morgan Davis on 8/29/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DEMO_TRANSIENTSCACHE_H
#define AVARA3D_DEMO_TRANSIENTSCACHE_H

#include <memory>
#include <vector>

#include "a3d/Math.h"

namespace a3d {

    class Mesh;
    class PhysicsShape;

}

namespace demo::janus {

    class TransientsCache {

    public:
        // [Public Types]

        struct Entry {
            std::shared_ptr<a3d::Mesh>         mesh;
            std::shared_ptr<a3d::PhysicsShape> physicsShape;
        };

        // [Public Lifecycle Functions]

        TransientsCache() = default;

        // [Public Member Functions]

        void init();

        const std::vector<Entry>& rocks() const;
        const Entry&              coin() const;
        const Entry&              ball() const;
        const Entry&              hammer() const;
        const Entry&              hula() const;
        const Entry&              duck() const;

    private:
        // [Private Member Functions]

        void initRocks();
        void initCoin();
        void initBall();
        void initHammer();
        void initHula();
        void initDuck();

        // [Private Member Variables]

        std::vector<Entry> _rocks;
        Entry              _coin;
        Entry              _ball;
        Entry              _hammer;
        Entry              _hula;
        Entry              _duck;
    };

}

#endif // AVARA3D_DEMO_TRANSIENTSCACHE_H
