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
     * @brief Immutable fixed-step scheduling parameters supplied to a Runner.
     *
     * Runner copies this structure at construction, validates it when start() is
     * called, and owns the mutable runtime time-scale value thereafter.
     *
     * @see Runner
     */
    struct SimulationConfig {

        /**
         * @brief Constant duration, in seconds, of every simulation step.
         *
         * The value must be finite and positive. It is used by both automatic
         * catch-up steps and explicitly requested paused steps.
         */
        double        timeStep {1.0 / 60.0};

        /**
         * @brief Maximum automatic simulation steps performed by one update.
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
         * this scale. Runner owns the mutable runtime value after construction.
         */
        double        timeScale {1.0};
    };

}

#endif //AVARA3D_SIMULATIONCONFIG_H
