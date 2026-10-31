//
//  TextureBinding.h
//  avara3d
//
//  Created by Morgan Davis on 10/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_MATERIAL_TEXTUREBINDING_H
#define AVARA3D_VISUAL_MATERIAL_TEXTUREBINDING_H

#include <cstdint>
#include <memory>
#include <optional>

#include "a3d/Math.h"
#include "a3d/visual/material/Sampler.h"

namespace a3d {

class Texture;

class TextureBinding {

public:
    // [Public Types]

    struct UVMapping {
        std::uint8_t channel {0};
        math::mat3   transform {1.0f};
    };

    // [Public Lifecycle Functions]

    explicit TextureBinding(std::shared_ptr<Texture> texture,
                            Sampler                  sampler   = {},
                            std::optional<UVMapping> uvMapping = UVMapping {});

    // [Public Member Functions]

    const std::shared_ptr<Texture>& texture() const;
    void                            texture(std::shared_ptr<Texture> texture);

    Sampler&                        sampler();
    const Sampler&                  sampler() const;
    void                            sampler(Sampler sampler);

    std::optional<UVMapping>&       uvMapping();
    const std::optional<UVMapping>& uvMapping() const;
    void                            uvMapping(std::optional<UVMapping> mapping);

private:
    std::shared_ptr<Texture> _texture;
    Sampler                  _sampler;
    std::optional<UVMapping> _uvMapping;
};

} // namespace a3d

#endif // AVARA3D_VISUAL_MATERIAL_TEXTUREBINDING_H
