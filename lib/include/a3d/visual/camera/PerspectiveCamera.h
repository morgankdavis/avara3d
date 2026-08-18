//
//  PerspectiveCamera.h
//  avara3d
//
//  Created by Morgan Davis on 10/22/23.
//  Copyright © 2023-2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_CAMERA_PERSPECTIVECAMERA_H
#define AVARA3D_VISUAL_CAMERA_PERSPECTIVECAMERA_H

#include "a3d/Math.h"
#include "a3d/visual/camera/Camera.h"

namespace a3d {

    class PerspectiveCamera : public Camera {

    public:
        /// Public Lifecycle Functions ///

        PerspectiveCamera();
        PerspectiveCamera(float zNear, float zFar, float yFov);
        PerspectiveCamera(const std::string& name, float zNear, float zFar, float yFov);

        PerspectiveCamera(const PerspectiveCamera&)            = default;
        PerspectiveCamera& operator=(const PerspectiveCamera&) = default;

        PerspectiveCamera(PerspectiveCamera&&) noexcept            = default;
        PerspectiveCamera& operator=(PerspectiveCamera&&) noexcept = default;

        ~PerspectiveCamera() override;

        /// Public Member Functions ///

        float                       zNear() const;
        void                        zNear(float zNear);

        float                       zFar() const;
        void                        zFar(float zFar);

        float                       yFov() const;
        void                        yFov(float fov);

        /// Camera Internal Member Functions ///

        math::mat4                  projection(const math::uvec2& framebufferSize) const override;

    protected:
        /// Camera Protected Member Functions ///

//		void 			constructProjectionMatrix() override;

    private:
        /// Private Member Variables ///

        float                _zNear;
        float                _zFar;
        float                _yFov;
    };

}

#endif /* AVARA3D_VISUAL_CAMERA_PERSPECTIVECAMERA_H */
