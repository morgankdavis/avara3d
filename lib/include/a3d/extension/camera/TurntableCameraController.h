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

    class TurntableCameraController {

    public:
        // [Public Types]

        struct NodeTarget {

            std::weak_ptr<Node> node;
            math::vec3          localPosition {0.0f};
        };

        using Target = std::variant<math::vec3, NodeTarget>;

        struct View {

            Target target {math::vec3 {0.0f}};
            float  yaw {0.0f};
            float  pitch {0.0f};
            float  distance {10.0f};
        };

        struct Controls {

            DesktopInputContext::MouseButton orbitButton {DesktopInputContext::MouseButton::One};
            DesktopInputContext::MouseButton panButton {DesktopInputContext::MouseButton::Two};
            DesktopInputContext::Key         panModifier {DesktopInputContext::Key::LeftShift};
            DesktopInputContext::Key         dollyModifier {DesktopInputContext::Key::LeftControl};
        };

        struct PointerClick {

            math::vec2 position;
        };

        struct Config {

            float    minPitch {-math::radians(85.0f)};
            float    maxPitch {math::radians(85.0f)};

            float    minDistance {0.01f};
            float    maxDistance {math::F32_MAX};

            float    orbitSensitivity {0.004f};
            float    dollySensitivity {0.01f};
            float    scrollDollySensitivity {0.15f};
            float    dragThreshold {4.0f};
            bool     invertPitch {false};

            Controls controls;
        };

        struct UpdateResult {

            bool                        cameraChanged {false};
            bool                        pointerDragging {false};
            std::optional<PointerClick> orbitButtonClick;
            std::optional<PointerClick> panButtonClick;
        };

        // [Public Lifecycle Functions]

        TurntableCameraController();
        explicit TurntableCameraController(const Config& config);

        // [Public Member Functions]

        const Config& config() const;
        void          config(const Config& config);

        const View&   view() const;
        void          view(const View& view);

        void          target(const math::vec3& worldPosition);
        void          target(const std::shared_ptr<Node>& node);
        void          target(const std::shared_ptr<Node>& node, const math::vec3& localPosition);

        UpdateResult  update(DesktopInputContext& input, float vFov, float viewportHeight);
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
