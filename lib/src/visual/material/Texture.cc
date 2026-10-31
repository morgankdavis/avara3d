//
//  Texture.cc
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/material/Texture.h"

#include <utility>

#include "a3d/IdGenerator.h"
#include "a3d/util/Bitmask.h"

using namespace std;

namespace a3d {

// [Public Lifecycle Functions]

Texture::Texture():
    _id {IdGenerator<TextureId>::next()},
    _contents {monostate {}},
    _dirtyMask {DirtyMask::All} {}

Texture::Texture(const Contents& contents):
    Texture() {
    _contents = contents;
}

Texture::Texture(const Texture& other):
    Texture() {

    *this = other;
}

Texture& Texture::operator=(const Texture& other) {

    if (this == &other) {
        return *this;
    }

    _contents = other._contents;
    _dirtyMask = DirtyMask::All;

    return *this;
}

Texture::Texture(Texture&& other) noexcept:
    Texture() {

    *this = std::move(other);
}

Texture& Texture::operator=(Texture&& other) noexcept {

    if (this == &other) {
        return *this;
    }

    _contents = std::move(other._contents);
    _dirtyMask = DirtyMask::All;

    other._dirtyMask = DirtyMask::All;

    return *this;
}

Texture::~Texture() {}

// [Public Member Functions]

const Texture::Contents& Texture::contents() const {
    return _contents;
}

void Texture::contents(const Contents& contents) {
    _contents = contents;
    _dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::Contents);
}

// [Internal Member Functions]

TextureId Texture::id() const noexcept {
    return _id;
}

Texture::DirtyMask Texture::dirtyMask() const {
    return _dirtyMask;
}

void Texture::dirtyMask(DirtyMask mask) {
    _dirtyMask = mask;
}

} // namespace a3d
