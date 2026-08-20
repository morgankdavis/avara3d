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

    class FlyCameraController {

    public:
        // [Public Types]

        struct Controls {

            DesktopInputContext::Key forward {DesktopInputContext::Key::W};
            DesktopInputContext::Key back {DesktopInputContext::Key::S};
            DesktopInputContext::Key left {DesktopInputContext::Key::A};
            DesktopInputContext::Key right {DesktopInputContext::Key::D};
            DesktopInputContext::Key up {DesktopInputContext::Key::Space};
            DesktopInputContext::Key descendModifier {DesktopInputContext::Key::LeftShift};
            DesktopInputContext::Key fastModifier {DesktopInputContext::Key::LeftControl};
        };

        struct Config {

            float    lookSensitivity {0.001f};
            float    moveSpeed {1.0f};
            float    fastMoveMultiplier {2.0f};
            Controls controls;
        };

        // [Public Lifecycle Functions]

        FlyCameraController();
        explicit FlyCameraController(const Config& config);

        // [Public Member Functions]

        const Config& config() const;
        void          config(const Config& config);

        bool          update(Node& pov, DesktopInputContext& input, double deltaTime);

    private:
        // [Private Member Variables]

        Config _config;
    };

}

#endif // AVARA3D_EXTENSION_CAMERA_FLYCAMERACONTROLLER_H
