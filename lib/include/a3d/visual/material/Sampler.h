//
//  Sampler.h
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_MATERIAL_SAMPLER_H
#define AVARA3D_VISUAL_MATERIAL_SAMPLER_H

#include <climits>

#include "a3d/Id.h"
#include "a3d/util/Bitmask.h"

namespace a3d {

class Color;
class Image;

/** @brief Describes filtering, anisotropy, and coordinate wrapping for a Texture. */
class Sampler {

public:
    // [Public Types]

    // TODO: don't use GL constants
    /** @brief Selects texture filtering and optional mipmap interpolation. */
    enum class FilterMode : uint16_t {
        Nearest              = 0x2600, ///< Select the nearest texel.
        Linear               = 0x2601, ///< Linearly interpolate neighboring texels.
        NearestMipmapNearest = 0x2700, ///< Nearest texel from the nearest mipmap level.
        LinearMipmapNearest  = 0x2701, ///< Linear texel filtering from the nearest mipmap level.
        NearestMipmapLinear  = 0x2702, ///< Nearest-texel filtering interpolated between mipmap levels.
        LinearMipmapLinear   = 0x2703  ///< Linear texel filtering interpolated between mipmap levels.
    };

    // TODO: don't use GL constants
    /** @brief Selects how texture coordinates outside the image range are wrapped. */
    enum class WrapMode : uint16_t {
        Repeat         = 0x2901, ///< Repeat the texture periodically.
        MirroredRepeat = 0x8370, ///< Repeat while mirroring every other interval.
        ClampToEdge    = 0x812F  ///< Clamp coordinates to the texture edge.
    };

    // [Public Lifecycle Functions]

    /** @brief Creates a Sampler with default filtering, anisotropy, and wrapping state. */
    Sampler();

    Sampler(const Sampler& other);
    Sampler& operator=(const Sampler& other);

    Sampler(Sampler&& other);
    Sampler& operator=(Sampler&& other);

    ~Sampler();

    // [Public Member Functions]

    /** @brief Returns the texture minification filter. */
    FilterMode minificationFilter() const;

    /** @brief Sets the texture minification filter. */
    void       minificationFilter(FilterMode mode);

    /** @brief Returns the texture magnification filter. */
    FilterMode magnificationFilter() const;

    /** @brief Sets the magnification filter; magnification supports Nearest or Linear filtering. */
    void       magnificationFilter(FilterMode mode);

    /** @brief Returns the requested maximum anisotropy. */
    float      maxAnisotropy() const;

    /**
     * @brief Sets the requested maximum anisotropy.
     *
     * The renderer may clamp the request to the active rendering API's supported maximum.
     */
    void       maxAnisotropy(float max);

    /** @brief Returns the S-coordinate wrap mode. */
    WrapMode   wrapS() const;

    /** @brief Sets the S-coordinate wrap mode. */
    void       wrapS(WrapMode mode);

    /** @brief Returns the T-coordinate wrap mode. */
    WrapMode   wrapT() const;

    /** @brief Sets the T-coordinate wrap mode. */
    void       wrapT(WrapMode mode);

    /** @brief Returns the R-coordinate wrap mode used by cubemap sampling. */
    WrapMode   wrapR() const;

    /** @brief Sets the R-coordinate wrap mode used by cubemap sampling. */
    void       wrapR(WrapMode mode);

    // [Internal Types]

    enum class DirtyMask : uint32_t {
        None                = 0,
        MinificationFilter  = 1 << 0,
        MagnificationFilter = 1 << 1,
        MaxAnisotropy       = 1 << 2,
        WrapS               = 1 << 3,
        WrapT               = 1 << 4,
        WrapR               = 1 << 5,
        All                 = UINT_MAX
    };

    // [Internal Member Functions]

    SamplerId id() const noexcept;

    DirtyMask dirtyMask() const;
    void      dirtyMask(DirtyMask mask);

private:
    // [Private Member Variables]

    SamplerId  _id;
    FilterMode _minificationFilter;
    FilterMode _magnificationFilter;
    float      _maxAnisotropy;
    WrapMode   _wrapS;
    WrapMode   _wrapT;
    WrapMode   _wrapR;

    DirtyMask  _dirtyMask;
};

namespace util::bitmask {

    template<>
    struct enable_ops<Sampler::DirtyMask> : std::true_type {};

} // namespace util::bitmask
} // namespace a3d

#endif // AVARA3D_VISUAL_MATERIAL_SAMPLER_H
