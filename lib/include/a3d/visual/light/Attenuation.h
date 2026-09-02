//
//  Attenuation.h
//  avara3d
//
//  Created by Morgan Davis on 1/7/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_ATTENUATION_H
#define AVARA3D_VISUAL_LIGHT_ATTENUATION_H

namespace a3d {

/**
 * @brief Distance-attenuation coefficients for point and spot lights.
 *
 * Light intensity is scaled by 1 / (constant + linear * d + quadratic * d^2),
 * where d is the light-to-surface distance.
 */
struct Attenuation {

    // [Public Static Member Functions]

    /**
     * @brief Returns quadratic attenuation that falls to the fraction @p p at
     * distance @p range.
     *
     * The returned coefficients use constant = 1 and linear = 0. Values of @p p
     * are clamped to the open interval (0, 1). If @p range is less than or equal
     * to zero, the returned quadratic coefficient is zero.
     */
    static Attenuation FromRange(float range, float p = 0.01f);

    // [Public Member Variables]

    float              constant  = 1.0f; ///< Constant denominator coefficient.
    float              linear    = 0.0f; ///< Linear-distance denominator coefficient.
    float              quadratic = 0.1f; ///< Squared-distance denominator coefficient.
};

} // namespace a3d

#endif // AVARA3D_VISUAL_LIGHT_ATTENUATION_H
