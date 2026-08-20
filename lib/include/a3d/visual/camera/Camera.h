//
//  Camera.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_CAMERA_CAMERA_H
#define AVARA3D_VISUAL_CAMERA_CAMERA_H

#include <memory>
#include <optional>
#include <string>

#include "a3d/Math.h"

namespace a3d {

    /**
     * @brief Base class for camera projection models used by VisualWorld.
     *
     * A Camera is attached to a Node; the Node's world transform supplies the
     * camera pose while the concrete Camera supplies the projection.
     */
    class Camera {

    public:
        // [Public Lifecycle Functions]

        /** @brief Creates an unnamed Camera. */
        Camera();

        /** @brief Creates a Camera with @p name. */
        explicit Camera(const std::string& name);

        virtual ~Camera() = 0;

        // [Public Member Functions]

        /** @brief Returns the optional camera name. */
        const std::optional<std::string>& name() const;

        /** @brief Sets the camera name. */
        void                              name(const std::string& name);

        // [Internal Member Functions]

        // virtual math::mat4                projection() const = 0;
        virtual math::mat4                projection(const math::uvec2& viewportSize) const = 0;

    protected:
        // [Protected Lifecycle Functions]

        Camera(const Camera&)            = default;
        Camera& operator=(const Camera&) = default;

        Camera(Camera&&) noexcept                               = default;
        Camera&                    operator=(Camera&&) noexcept = default;

        // [Protected Member Variables]

        std::optional<std::string> _name;
    };

}

#endif /* AVARA3D_VISUAL_CAMERA_CAMERA_H */
