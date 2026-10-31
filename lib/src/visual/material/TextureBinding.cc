//
//  TextureBinding.cc
//  avara3d
//
//  Created by Morgan Davis on 10/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/material/TextureBinding.h"

#include <stdexcept>
#include <utility>

#include "a3d/visual/material/Texture.h"

using namespace std;

namespace a3d {

TextureBinding::TextureBinding(shared_ptr<Texture> texture, Sampler sampler, optional<UVMapping> uvMapping):
    _texture {std::move(texture)},
    _sampler {std::move(sampler)},
    _uvMapping {std::move(uvMapping)} {

    if (!_texture) {
        throw invalid_argument("TextureBinding texture cannot be null.");
    }
}

const shared_ptr<Texture>& TextureBinding::texture() const {
    return _texture;
}

void TextureBinding::texture(shared_ptr<Texture> texture) {

    if (!texture) {
        throw invalid_argument("TextureBinding texture cannot be null.");
    }

    _texture = std::move(texture);
}

Sampler& TextureBinding::sampler() {
    return _sampler;
}

const Sampler& TextureBinding::sampler() const {
    return _sampler;
}

void TextureBinding::sampler(Sampler sampler) {
    _sampler = std::move(sampler);
}

optional<UVMapping>& TextureBinding::uvMapping() {
    return _uvMapping;
}

const optional<UVMapping>& TextureBinding::uvMapping() const {
    return _uvMapping;
}

void TextureBinding::uvMapping(optional<UVMapping> mapping) {
    _uvMapping = std::move(mapping);
}

} // namespace a3d
