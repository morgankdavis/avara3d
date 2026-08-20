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

    /**
     * @brief Camera using an orthographic projection defined by an AABB.
     *
     * The extent's X range supplies the left and right projection limits, Y supplies
     * bottom and top, and Z supplies the near and far clipping limits. The projection
     * does not change with viewport aspect ratio.
     */
    class OrthographicCamera : public Camera {

    public:
        // [Public Lifecycle Functions]

        /** @brief Creates an unnamed OrthographicCamera with extent [-1, 1] on all three axes. */
        OrthographicCamera();

        /** @brief Creates an unnamed OrthographicCamera using @p extent. */
        explicit OrthographicCamera(const AABB& extent);

        /** @brief Creates a named OrthographicCamera using @p extent. */
        OrthographicCamera(const std::string& name, const AABB& extent);

        OrthographicCamera(const OrthographicCamera&)            = default;
        OrthographicCamera& operator=(const OrthographicCamera&) = default;

        OrthographicCamera(OrthographicCamera&&) noexcept            = default;
        OrthographicCamera& operator=(OrthographicCamera&&) noexcept = default;

        ~OrthographicCamera() override;

        // [Public Member Functions]

        /** @brief Returns the orthographic projection extent. */
        const AABB& extent() const;

        /** @brief Sets the orthographic projection extent. */
        void        extent(const AABB& e);

        // [Camera Internal Member Functions]

        math::mat4  projection(const math::uvec2& viewportSize) const override;

    private:
        // [Private Member Variables]

        AABB _extent;
    };

}

#endif /* AVARA3D_VISUAL_CAMERA_ORTHOGRAPHICCAMERA_H */
