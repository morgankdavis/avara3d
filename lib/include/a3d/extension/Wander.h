//
//  Wander.h
//  avara3d
//
//  Created by Morgan Davis on 8/12/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXTENSION_WANDER_H
#define AVARA3D_EXTENSION_WANDER_H

#include <cstdint>
#include <memory>
#include <random>

#include "a3d/Math.h"

namespace a3d {

    class Node;

}

namespace a3d::ext {

    class Wander {

    public:
        struct Config {

            // Half-size of the local-space box around the node's starting
            // position. A zero component locks movement on that axis.
            math::vec3    halfExtents {1.0f, 1.0f, 1.0f};

            // Time between generation of successive spline control points.
            // Smaller values produce faster, more restless movement.
            double        segmentDuration {3.0};

            // Same seed produces the same path.
            std::uint32_t seed {0};
        };

        explicit Wander(const std::shared_ptr<Node>& node);
        Wander(const std::shared_ptr<Node>& node, const Config& config);

        const Config&     config() const;
        void              config(const Config& config);

        const math::vec3& center() const;

        void              update(double deltaTime);

        // Returns the node to its original center and restarts the same
        // deterministic path.
        void              reset();

        // Makes the node's current position the new center and restarts.
        void              recenter();

    private:
        void                validateConfig(const Config& config) const;

        void                initializePath();
        void                advancePath();

        math::vec3          randomPoint();
        math::vec3          randomOffset(float scale);

        math::vec3          evaluate(float t) const;

        std::weak_ptr<Node> _node;

        Config              _config;
        std::mt19937        _random;

        math::vec3          _center;

        math::vec3          _p0;
        math::vec3          _p1;
        math::vec3          _p2;
        math::vec3          _p3;

        double              _phase {0.0};
    };

}

#endif // AVARA3D_EXTENSION_WANDER_H
