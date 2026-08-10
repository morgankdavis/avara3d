//
//  TurntableCameraController.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXTENSION_CAMERA_TURNTABLECAMERACONTROLLER_H
#define AVARA3D_EXTENSION_CAMERA_TURNTABLECAMERACONTROLLER_H

#include <limits>
#include <memory>
#include <variant>

#include "a3d/Math.h"

namespace a3d {

    class Node;

}

namespace a3d::ext {

    class TurntableCameraController {

    public:
        /// Public Types ///

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

        struct Config {

            float minPitch {-math::radians(85.0f)};
            float maxPitch {math::radians(85.0f)};

            float minDistance {0.01f};
            float maxDistance {std::numeric_limits<float>::max()};
        };

        /// Public Lifecycle Functions ///

        TurntableCameraController();
        explicit TurntableCameraController(const Config& config);

        /// Public Member Functions ///

        const Config& config() const;
        void          config(const Config& config);

        const View&   view() const;
        void          view(const View& view);

        void          target(const math::vec3& worldPosition);
        void          target(const std::shared_ptr<Node>& node);
        void          target(const std::shared_ptr<Node>& node, const math::vec3& localPosition);

        void          apply(Node& pov);

    private:
        /// Private Member Functions ///

        math::vec3 resolveTarget();

        /// Private Member Variables ///

        Config     _config;
        View       _view;
        math::vec3 _resolvedTarget;
    };

}

#endif // AVARA3D_EXTENSION_CAMERA_TURNTABLECAMERACONTROLLER_H
