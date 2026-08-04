//
//  Texture.h
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_MATERIAL_TEXTURE_H
#define AVARA3D_VISUAL_MATERIAL_TEXTURE_H

#include <atomic>
#include <climits>
#include <memory>
#include <variant>

#include "a3d/Id.h"
#include "a3d/util/Bitmask.h"
#include "a3d/visual/material/Sampler.h"

namespace a3d {

    class CubeImage;
    class Image;

    class Texture {

    public:
        /// Public Types ///

        using Contents = std::variant<std::monostate, std::shared_ptr<Image>, std::shared_ptr<CubeImage>>;

        /// Public Lifecycle Functions ///

        Texture();
        explicit Texture(const Contents&                 contents,
                         const std::shared_ptr<Sampler>& sampler        = std::make_shared<Sampler>(),
                         unsigned                        mappingChannel = 0);

        Texture(const Texture& other);
        Texture& operator=(const Texture& other);

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        ~Texture();

        /// Public Member Functions ///

        std::shared_ptr<Sampler> sampler() const;
        void                     sampler(const std::shared_ptr<Sampler>& sampler);

        const Contents&          contents() const;
        void                     contents(const Contents& contents);

        // TODO: move to TextureBinding
        unsigned                 mappingChannel() const;
        void                     mappingChannel(unsigned channel);

        /// Internal Types ///

        enum class DirtyMask : uint32_t {
            None     = 0,
            Contents = 1 << 0,
            Sampler  = 1 << 1, // TODO: move to MaterialBinding dirty mask?
            All      = UINT_MAX
        };

        /// Internal Member Functions ///

        TextureId id() const noexcept;

        DirtyMask dirtyMask() const;
        void      dirtyMask(DirtyMask mask);

    private:
        /// Private Member Variables ///

        TextureId                _id;
        std::shared_ptr<Sampler> _sampler;
        Contents                 _contents;
        unsigned                 _mappingChannel;
        DirtyMask                _dirtyMask;
    };

    namespace util::bitmask {

        template<>
        struct enable_ops<Texture::DirtyMask> : std::true_type {};

    }
}

#endif //AVARA3D_VISUAL_MATERIAL_TEXTURE_H
