//
//  PerspectiveCamera.h
//  avara3d
//
//  Created by Morgan Davis on 10/22/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_CAMERA_PERSPECTIVECAMERA_H
#define AVARA3D_VISUAL_CAMERA_PERSPECTIVECAMERA_H

#include "a3d/Math.h"
#include "a3d/visual/camera/Camera.h"

namespace a3d {

    /** @brief Camera using a perspective projection with a vertical field of view. */
class PerspectiveCamera : public Camera {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates an unnamed camera with a 0.1 near plane, 1000 far plane, and 45-degree vertical FOV. */
    PerspectiveCamera();

    /** @brief Creates an unnamed perspective camera with the supplied clipping distances and vertical FOV. */
    PerspectiveCamera(float zNear, float zFar, float yFov);

    /** @brief Creates a named perspective camera with the supplied clipping distances and vertical FOV. */
    PerspectiveCamera(const std::string& name, float zNear, float zFar, float yFov);

    PerspectiveCamera(const PerspectiveCamera&)            = default;
    PerspectiveCamera& operator=(const PerspectiveCamera&) = default;

    PerspectiveCamera(PerspectiveCamera&&) noexcept            = default;
    PerspectiveCamera& operator=(PerspectiveCamera&&) noexcept = default;

    ~PerspectiveCamera() override;

    // [Public Member Functions]

    /** @brief Returns the near clipping distance. */
    float      zNear() const;

    /** @brief Sets the near clipping distance. */
    void       zNear(float zNear);

    /** @brief Returns the far clipping distance. */
    float      zFar() const;

    /** @brief Sets the far clipping distance. */
    void       zFar(float zFar);

    /** @brief Returns the vertical field of view in radians. */
    float      yFov() const;

    /** @brief Sets the vertical field of view in radians. */
    void       yFov(float fov);

    // [Camera Internal Member Functions]

    math::mat4 projection(const math::uvec2& framebufferSize) const override;

private:
    // [Private Member Variables]

    float _zNear;
    float _zFar;
    float _yFov;
};

}

#endif // AVARA3D_VISUAL_CAMERA_PERSPECTIVECAMERA_H
