//
//  Wanderer.h
//  avara3d
//
//  Created by Morgan Davis on 8/12/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXTENSION_WANDERER_H
#define AVARA3D_EXTENSION_WANDERER_H

#include <cstdint>
#include <memory>
#include <random>

#include "a3d/Math.h"

namespace a3d {

class Node;

} // namespace a3d

namespace a3d::ext {

/**
 * @brief Moves a Node along a smooth deterministic path within a configurable box.
 *
 * The wander volume is centered on a position in the Node's parent coordinate space.
 */
class Wanderer {

public:
    // [Public Types]

    /** @brief Configures the wander volume, pace, and deterministic random sequence. */
    struct Config {

        math::vec3 halfExtents {
            1.0f, 1.0f,
            1.0f}; ///< Half-size of the wander box; components must be non-negative. Zero locks an axis.
        double segmentDuration {
            3.0}; ///< Seconds between successive spline control points; must be greater than zero.
        std::uint32_t seed {0}; ///< Random seed; the same seed produces the same path.
    };

    // [Public Lifecycle Functions]

    /**
     * @brief Creates a Wanderer centered on @p node's current position using the default Config.
     *
     * @throws std::invalid_argument if @p node is null.
     */
    explicit Wanderer(const std::shared_ptr<Node>& node);

    /**
     * @brief Creates a Wanderer centered on @p node's current position using @p config.
     *
     * @throws std::invalid_argument if @p node is null or @p config is invalid.
     */
    Wanderer(const std::shared_ptr<Node>& node, const Config& config);

    // [Public Member Functions]

    /** @brief Returns the current configuration. */
    const Config&     config() const;

    /**
     * @brief Replaces the configuration and restarts the deterministic path from the current center.
     *
     * @throws std::invalid_argument if @p config is invalid.
     */
    void              config(const Config& config);

    /** @brief Returns the center of the wander volume in the Node's parent coordinate space. */
    const math::vec3& center() const;

    /** @brief Advances the wander path by @p deltaTime seconds; non-positive values do nothing. */
    void              update(double deltaTime);

    /** @brief Returns the Node to the current center and restarts the same deterministic path. */
    void              reset();

    /** @brief Makes the Node's current position the new center and restarts the deterministic path. */
    void              recenter();

private:
    // [Private Member Functions]

    void                initializePath();
    void                advancePath();

    math::vec3          randomPoint();
    math::vec3          randomOffset(float scale);

    // [Private Member Variables]

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

} // namespace a3d::ext

#endif // AVARA3D_EXTENSION_WANDERER_H
