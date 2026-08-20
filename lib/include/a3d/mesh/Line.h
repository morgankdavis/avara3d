//
//  Line.h
//  avara3d
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_LINE_H
#define AVARA3D_MESH_LINE_H

#include <memory>

#include "a3d/Color.h"
#include "a3d/Math.h"

namespace a3d {

    class Line {

// [Internal Lifecycle Functions]

    public:
        Line(const math::vec3& fromLocation, const math::vec3& toLocation);
        Line(const math::vec3& fromLocation, const math::vec3& toLocation, const Color& color);
        Line(const math::vec3& fromLocation,
             const math::vec3& toLocation,
             const math::vec3& fromColor,
             const math::vec3& toColor);
        Line(const math::vec3& fromLocation,
             const math::vec3& toLocation,
             const Color&      fromColor,
             const Color&      toColor);

        // [Internal Member Functions]

        const math::vec3& fromLocation() const;
        void              fromLocation(const math::vec3& location);

        const math::vec3& toLocation() const;
        void              toLocation(const math::vec3& location);

        const Color&      fromColor() const;
        void              fromColor(const Color& color);

        const Color&      toColor() const;
        void              toColor(const Color& color);

    private:
        // [Private Member Variables]

        math::vec3 _fromLocation;
        math::vec3 _toLocation;
        Color      _fromColor;
        Color      _toColor;
    };

}

#endif /* AVARA3D_MESH_LINE_H */
