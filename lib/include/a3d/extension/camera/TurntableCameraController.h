//
//  TurntableCameraController.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXTENSION_CAMERA_TURNTABLECAMERACONTROLLER_H
#define AVARA3D_EXTENSION_CAMERA_TURNTABLECAMERACONTROLLER_H

#include <memory>
#include <optional>
#include <variant>

#include "a3d/Math.h"
#include "a3d/input/DesktopInputContext.h"

namespace a3d {

class Node;

}

namespace a3d::ext {

/**
 * @brief Provides orbit, pan, and dolly control around a world-space or Node-relative target.
 *
 * update() consumes DesktopInputContext state and changes the controller's View;
 * apply() then positions and orients a supplied point-of-view Node from that View.
 * A Node-relative target is retained weakly and follows the selected local point
 * while the Node exists. If it later expires, the controller preserves the last
 * resolved world-space target position.
 */
class TurntableCameraController {

public:
    // [Public Types]

    /** @brief Target point expressed in the local coordinates of a weakly referenced Node. */
    struct NodeTarget {

        std::weak_ptr<Node> node; ///< Node whose transform supplies the target coordinate space.
        math::vec3          localPosition {0.0f}; ///< Target point in the Node's local coordinates.
    };

    /** @brief Orbit target represented by either a fixed world position or a Node-relative point. */
    using Target = std::variant<math::vec3, NodeTarget>;

    /** @brief Camera orbit state around a target. */
    struct View {

        Target target {math::vec3 {0.0f}}; ///< Point around which the camera orbits.
        float  yaw {0.0f}; ///< Horizontal orbit angle in radians.
        float  pitch {0.0f}; ///< Vertical orbit angle in radians.
        float  distance {10.0f}; ///< Distance from the target in scene units.
    };

    /** @brief Pointer buttons and modifier keys used for camera manipulation. */
    struct Controls {

        DesktopInputContext::MouseButton orbitButton {
            DesktopInputContext::MouseButton::One}; ///< Primary orbit/click button.
        DesktopInputContext::MouseButton panButton {
            DesktopInputContext::MouseButton::Two}; ///< Dedicated pan/click button.
        DesktopInputContext::Key panModifier {
            DesktopInputContext::Key::LeftShift}; ///< Makes orbit-button dragging pan.
        DesktopInputContext::Key dollyModifier {
            DesktopInputContext::Key::LeftControl}; ///< Makes orbit-button dragging dolly.
    };

    /** @brief Pointer click reported when a configured button is released without becoming a drag. */
    struct PointerClick {

        math::vec2 position; ///< Release position in logical viewport coordinates.
    };

    /** @brief Limits, sensitivities, and input bindings used by the controller. */
    struct Config {

        float minPitch {-math::radians(85.0f)}; ///< Minimum pitch in radians; must remain above -90 degrees.
        float maxPitch {math::radians(85.0f)}; ///< Maximum pitch in radians; must remain below 90 degrees.

        float minDistance {0.01f}; ///< Minimum positive target distance in scene units.
        float maxDistance {math::F32_MAX}; ///< Maximum target distance in scene units.

        float orbitSensitivity {0.002f}; ///< Orbit radians per logical pointer unit.
        float dollySensitivity {0.002f}; ///< Exponential dolly sensitivity for pointer dragging.
        float scrollDollySensitivity {0.05f}; ///< Exponential dolly sensitivity per scroll unit.
        float dragThreshold {5.0f}; ///< Pointer displacement required to convert a click candidate into a drag.
        bool  invertPitch {false}; ///< Reverses vertical orbit direction when true.

        Controls controls; ///< Pointer-button and modifier bindings.
    };

    /** @brief Results produced by one input update. */
    struct UpdateResult {

        bool cameraChanged {false}; ///< Whether update() changed the View.
        bool pointerDragging {false}; ///< Whether an orbit or pan button is currently dragging.
        std::optional<PointerClick>
            orbitButtonClick; ///< Orbit-button click released without crossing the drag threshold.
        std::optional<PointerClick>
            panButtonClick; ///< Pan-button click released without crossing the drag threshold.
    };

    // [Public Lifecycle Functions]

    /** @brief Creates a controller with the default configuration and View. */
    TurntableCameraController();

    /**
     * @brief Creates a controller with @p config and the default View.
     *
     * @throws std::invalid_argument if @p config is invalid.
     */
    explicit TurntableCameraController(const Config& config);

    // [Public Member Functions]

    /** @brief Returns the current controller configuration. */
    const Config& config() const;

    /**
     * @brief Replaces the controller configuration and clamps the current View to its new limits.
     *
     * @throws std::invalid_argument if @p config is invalid.
     */
    void          config(const Config& config);

    /** @brief Returns the current orbit View. */
    const View&   view() const;

    /**
     * @brief Replaces the orbit View, clamping pitch and distance to the configured limits.
     *
     * A NodeTarget must contain a live Node at the time the View is assigned.
     *
     * @throws std::invalid_argument if the View or its target is invalid.
     */
    void          view(const View& view);

    /**
     * @brief Sets a fixed world-space orbit target.
     */
    void          target(const math::vec3& worldPosition);

    /**
     * @brief Sets the orbit target to the local origin of @p node.
     *
     * The Node is retained weakly.
     *
     * @throws std::invalid_argument if @p node is null.
     */
    void          target(const std::shared_ptr<Node>& node);

    /**
     * @brief Sets the orbit target to @p localPosition in @p node coordinates.
     *
     * The Node is retained weakly and the target follows changes to its world transform.
     *
     * @throws std::invalid_argument if @p node is null.
     */
    void          target(const std::shared_ptr<Node>& node, const math::vec3& localPosition);

    /**
     * @brief Updates the View from pointer-button, pointer-motion, and scroll input.
     *
     * The orbit button normally orbits, or pans/dollies when the configured modifier
     * was held when the button was pressed. The dedicated pan button always pans.
     * A button released before crossing dragThreshold is reported as a click instead.
     * This function changes controller state only; call apply() to update a POV Node.
     *
     * @param input Current desktop-style input state.
     * @param vFov Vertical camera field of view in radians.
     * @param viewportHeight Logical viewport height used to scale panning.
     * @return Input-processing results for this update.
     *
     * @throws std::invalid_argument if @p vFov or @p viewportHeight is invalid.
     */
    UpdateResult  update(DesktopInputContext& input, float vFov, float viewportHeight);

    /**
     * @brief Applies the current View to @p pov so it looks at the resolved target.
     *
     * The resulting world transform is converted into parent coordinates when the POV
     * has a parent. Node-relative targets are resolved again each time apply() is called.
     */
    void          apply(Node& pov);

private:
    // [Private Types]

    enum class DragMode {
        Orbit,
        Pan,
        Dolly
    };

    // [Private Member Functions]

    math::vec3 resolveTarget();
    void       translateTarget(const math::vec3& worldTranslation);

    // [Private Member Variables]

    Config     _config;
    View       _view;
    math::vec3 _resolvedTarget;
    bool       _orbitButtonActive;
    bool       _orbitButtonDragging;
    DragMode   _orbitButtonDragMode;
    math::vec2 _orbitButtonPressPosition;
    bool       _panButtonActive;
    bool       _panButtonDragging;
    math::vec2 _panButtonPressPosition;
};

}

#endif // AVARA3D_EXTENSION_CAMERA_TURNTABLECAMERACONTROLLER_H
