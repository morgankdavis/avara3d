//
//  FlyCameraController.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXTENSION_CAMERA_FLYCAMERACONTROLLER_H
#define AVARA3D_EXTENSION_CAMERA_FLYCAMERACONTROLLER_H

#include "a3d/input/DesktopInputContext.h"

namespace a3d {

class Node;

}

namespace a3d::ext {

    /**
 * @brief Applies free-flight keyboard and pointer controls to a point-of-view Node.
 *
 * Pointer motion changes pitch and yaw while configured keys translate the Node
 * along its local axes expressed in parent coordinates. update() modifies the
 * supplied Node directly and returns whether its transform changed.
 */
class FlyCameraController {

public:
    // [Public Types]

        /** @brief Key bindings used for free-flight translation and speed control. */
    struct Controls {

        DesktopInputContext::Key forward {DesktopInputContext::Key::W}; ///< Move along the POV's forward axis.
        DesktopInputContext::Key back {DesktopInputContext::Key::S}; ///< Move opposite the POV's forward axis.
        DesktopInputContext::Key left {DesktopInputContext::Key::A}; ///< Move opposite the POV's right axis.
        DesktopInputContext::Key right {DesktopInputContext::Key::D}; ///< Move along the POV's right axis.
        DesktopInputContext::Key up {DesktopInputContext::Key::Space}; ///< Move along the POV's up axis.
        DesktopInputContext::Key descendModifier {
            DesktopInputContext::Key::LeftShift}; ///< Reverses the up control while held.
        DesktopInputContext::Key fastModifier {
            DesktopInputContext::Key::LeftControl}; ///< Applies the fast-movement multiplier while held.
    };

    /** @brief Free-flight sensitivity, speed, and control bindings. */
    struct Config {

        float lookSensitivity {0.001f}; ///< Pointer-look sensitivity applied to logical pointer displacement.
        float moveSpeed {1.0f}; ///< Translation speed in scene units per second.
        float fastMoveMultiplier {2.0f}; ///< Multiplier applied to moveSpeed while the fast modifier is held.
        Controls controls; ///< Keyboard bindings used for translation and speed control.
    };

    // [Public Lifecycle Functions]

    /** @brief Creates a FlyCameraController with the default configuration. */
    FlyCameraController();

    /** @brief Creates a FlyCameraController with @p config. */
    explicit FlyCameraController(const Config& config);

    // [Public Member Functions]

    /** @brief Returns the current controller configuration. */
    const Config& config() const;

    /** @brief Replaces the controller configuration. */
    void          config(const Config& config);

    /**
     * @brief Updates @p pov from the current input state.
     *
     * Pointer displacement changes pitch and yaw and clears roll when look input
     * is applied. Translation uses the configured movement keys and @p deltaTime.
     *
     * @param pov Node whose transform is controlled.
     * @param input Current desktop-style input state.
     * @param deltaTime Elapsed time represented by this update, in seconds.
     * @return true if the POV transform changed.
     */
    bool          update(Node& pov, DesktopInputContext& input, double deltaTime);

private:
    // [Private Member Variables]

    Config _config;
};

}

#endif // AVARA3D_EXTENSION_CAMERA_FLYCAMERACONTROLLER_H
