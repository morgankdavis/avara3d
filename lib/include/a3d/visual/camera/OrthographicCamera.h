//
//  OrthographicCamera.h
//  avara3d
//
//  Created by Morgan Davis on 10/22/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_CAMERA_ORTHOGRAPHICCAMERA_H
#define AVARA3D_VISUAL_CAMERA_ORTHOGRAPHICCAMERA_H

#include <string>

#include "a3d/Math.h"
#include "a3d/visual/camera/Camera.h"

namespace a3d {

/** @brief Camera using a centered orthographic projection with a fixed vertical size. */
class OrthographicCamera : public Camera {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates an unnamed camera with a 0.1 near plane, 1000 far plane, and vertical size of 2. */
    OrthographicCamera();

    /** @brief Creates an unnamed orthographic camera with the supplied clipping distances and vertical size. */
    OrthographicCamera(float zNear, float zFar, float ySize);

    /** @brief Creates a named orthographic camera with the supplied clipping distances and vertical size. */
    OrthographicCamera(const std::string& name, float zNear, float zFar, float ySize);

    OrthographicCamera(const OrthographicCamera&)            = default;
    OrthographicCamera& operator=(const OrthographicCamera&) = default;

    OrthographicCamera(OrthographicCamera&&) noexcept            = default;
    OrthographicCamera& operator=(OrthographicCamera&&) noexcept = default;

    ~OrthographicCamera() override;

    // [Public Member Functions]

    /** @brief Returns the near clipping distance. */
    float      zNear() const;

    /** @brief Sets the near clipping distance. */
    void       zNear(float zNear);

    /** @brief Returns the far clipping distance. */
    float      zFar() const;

    /** @brief Sets the far clipping distance. */
    void       zFar(float zFar);

    /** @brief Returns the vertical size of the projection in scene units. */
    float      ySize() const;

    /** @brief Sets the vertical size of the projection in scene units. */
    void       ySize(float ySize);

    // [Camera Internal Member Functions]

    math::mat4 projection(const math::uvec2& viewportSize) const override;

private:
    // [Private Member Variables]

    float _zNear;
    float _zFar;
    float _ySize;
};

}

#endif // AVARA3D_VISUAL_CAMERA_ORTHOGRAPHICCAMERA_H
