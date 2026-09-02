//
//  Texture.h
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
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

/**
 * @brief Image or cubemap data together with the Sampler used to read it.
 *
 * Texture contents and Sampler are held with shared ownership. Copying a Texture
 * therefore shares the referenced Image or CubeImage and Sampler.
 */
class Texture {

public:
    // [Public Types]

    /** @brief Texture contents: empty, a 2D Image, or a CubeImage. */
    using Contents = std::variant<std::monostate, std::shared_ptr<Image>, std::shared_ptr<CubeImage>>;

    // [Public Lifecycle Functions]

    /** @brief Creates an empty Texture with no contents or Sampler and mapping channel 0. */
    Texture();

    /**
     * @brief Creates a Texture with shared @p contents, @p sampler, and mapping channel.
     *
     * Omitting @p sampler creates a new default Sampler for the Texture.
     */
    explicit Texture(const Contents&                 contents,
                     const std::shared_ptr<Sampler>& sampler        = std::make_shared<Sampler>(),
                     unsigned                        mappingChannel = 0);

    Texture(const Texture& other);
    Texture& operator=(const Texture& other);

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    ~Texture();

    // [Public Member Functions]

    /** @brief Returns the shared Sampler, or nullptr if none is assigned. */
    std::shared_ptr<Sampler> sampler() const;

    /** @brief Sets the shared Sampler; nullptr removes the explicit Sampler. */
    void                     sampler(const std::shared_ptr<Sampler>& sampler);

    /** @brief Returns the shared image or cubemap contents. */
    const Contents&          contents() const;

    /** @brief Sets the shared image or cubemap contents. */
    void                     contents(const Contents& contents);

    // TODO: move to TextureBinding
    /** @brief Returns the stored texture-coordinate mapping channel. The current renderer does not consume this value. */
    unsigned                 mappingChannel() const;

    /** @brief Sets the stored texture-coordinate mapping channel. The current renderer does not consume this value. */
    void                     mappingChannel(unsigned channel);

    // [Internal Types]

    enum class DirtyMask : uint32_t {
        None     = 0,
        Contents = 1 << 0,
        Sampler  = 1 << 1, // TODO: move to MaterialBinding dirty mask?
        All      = UINT_MAX
    };

    // [Internal Member Functions]

    TextureId id() const noexcept;

    DirtyMask dirtyMask() const;
    void      dirtyMask(DirtyMask mask);

private:
    // [Private Member Variables]

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

#endif // AVARA3D_VISUAL_MATERIAL_TEXTURE_H
