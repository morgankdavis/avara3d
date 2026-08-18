//
//  OrthographicCamera.h
//  avara3d
//
//  Created by Morgan Davis on 10/22/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_CAMERA_ORTHOGRAPHICCAMERA_H
#define AVARA3D_VISUAL_CAMERA_ORTHOGRAPHICCAMERA_H

#include "a3d/Math.h"
#include "a3d/mesh/AABB.h"
#include "a3d/visual/camera/Camera.h"

namespace a3d {

    class OrthographicCamera : public Camera {

    public:
        /// Public Lifecycle Functions ///

        OrthographicCamera();
        explicit OrthographicCamera(const AABB& extent);
        OrthographicCamera(const std::string& name, const AABB& extent);

        OrthographicCamera(const OrthographicCamera&)            = default;
        OrthographicCamera& operator=(const OrthographicCamera&) = default;

        OrthographicCamera(OrthographicCamera&&) noexcept            = default;
        OrthographicCamera& operator=(OrthographicCamera&&) noexcept = default;

        ~OrthographicCamera() override;

        /// Public Member Functions ///

        const AABB& extent() const;
        void        extent(const AABB& e);

        /// Camera Internal Member Functions ///

        math::mat4  projection(const math::uvec2& viewportSize) const override;

    private:
        /// Private Member Variables ///

        AABB _extent;
    };

}

#endif /* AVARA3D_VISUAL_CAMERA_ORTHOGRAPHICCAMERA_H */
