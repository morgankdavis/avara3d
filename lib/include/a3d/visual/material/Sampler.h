//
//  Sampler.h
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2024-2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_MATERIAL_SAMPLER_H
#define AVARA3D_VISUAL_MATERIAL_SAMPLER_H

#include <climits>

#include "a3d/Id.h"
#include "a3d/util/Bitmask.h"

namespace a3d {

    class Color;
    class Image;

    class Sampler {

    public:
        /// Public Types ///

        // TODO: don't use GL constants
        enum class FilterMode : uint16_t {
            Nearest              = 0x2600,
            Linear               = 0x2601,
            NearestMipmapNearest = 0x2700,
            LinearMipmapNearest  = 0x2701,
            NearestMipmapLinear  = 0x2702,
            LinearMipmapLinear   = 0x2703
        };

        // TODO: don't use GL constants
        enum class WrapMode : uint16_t {
            Repeat         = 0x2901,
            MirroredRepeat = 0x8370,
            ClampToEdge    = 0x812F
        };

        /// Public Lifecycle Functions ///

        Sampler();

        Sampler(const Sampler& other);
        Sampler& operator=(const Sampler& other);

        Sampler(Sampler&& other);
        Sampler& operator=(Sampler&& other);

        ~Sampler();

        /// Public Member Functions ///

        FilterMode minificationFilter() const;
        void       minificationFilter(FilterMode mode);

        FilterMode magnificationFilter() const;
        void       magnificationFilter(FilterMode mode);

        float      maxAnisotropy() const;
        void       maxAnisotropy(float max);

        WrapMode   wrapS() const;
        void       wrapS(WrapMode mode);

        WrapMode   wrapT() const;
        void       wrapT(WrapMode mode);

        WrapMode   wrapR() const;
        void       wrapR(WrapMode mode);

        /// Internal Types ///

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

        /// Internal Member Functions ///

        SamplerId id() const noexcept;

        DirtyMask dirtyMask() const;
        void      dirtyMask(DirtyMask mask);

    private:
        /// Private Member Variables ///

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

    }
}

#endif //AVARA3D_VISUAL_MATERIAL_SAMPLER_H
