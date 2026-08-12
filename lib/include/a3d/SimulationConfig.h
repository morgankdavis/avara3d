//
//  SimulationConfig.h
//  avara3d
//
//  Created by Morgan Davis on 7/27/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SIMULATIONCONFIG_H
#define AVARA3D_SIMULATIONCONFIG_H

#include <cstdint>

namespace a3d {

    /**
     * @brief Initial simulation scheduling parameters supplied to a Runner.
     *
     * Runner copies this structure at construction. Runtime-configurable values
     * are exposed individually by Runner rather than by exposing the copied
     * configuration.
     *
     * @see Runner
     */
    struct SimulationConfig {

        /**
         * @brief Initial duration, in seconds, of each simulation step.
         *
         * The value must be finite and positive. It is used by both automatic
         * catch-up steps and explicitly requested paused steps.
         */
        double        timeStep {1.0 / 60.0};

        /**
         * @brief Initial maximum automatic simulation steps performed by one update.
         *
         * The value must be at least one. Any whole accumulated steps remaining
         * after this limit is reached are discarded while the fractional
         * remainder is retained.
         */
        std::uint32_t maxCatchUpSteps {8};

        /**
         * @brief Initial automatic simulation-time rate relative to host time.
         *
         * The value must be finite and positive. Requested paused steps ignore
         * this scale.
         */
        double        timeScale {1.0};
    };

}

#endif //AVARA3D_SIMULATIONCONFIG_H
