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

using namespace a3d;
using namespace std;

// [Public Lifecycle Functions]

Texture::Texture():
    _id {IdGenerator<TextureId>::next()},
    _sampler {nullptr},
    _contents {monostate {}},
    _mappingChannel {0},
    _dirtyMask {DirtyMask::All} {}

Texture::Texture(const Contents& contents, const shared_ptr<Sampler>& sampler, unsigned mappingChannel):
    Texture() {
    _sampler = sampler;
    _contents = contents;
    _mappingChannel = mappingChannel;
}

Texture::Texture(const Texture& other):
    Texture() {

    *this = other;
}

Texture& Texture::operator=(const Texture& other) {

    if (this == &other) {
        return *this;
    }

    _sampler = other._sampler;
    _contents = other._contents;
    _mappingChannel = other._mappingChannel;
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

    _sampler = std::move(other._sampler);
    _contents = std::move(other._contents);
    _mappingChannel = other._mappingChannel;
    _dirtyMask = DirtyMask::All;

    other._dirtyMask = DirtyMask::All;

    return *this;
}

Texture::~Texture() {}

// [Public Member Functions]

shared_ptr<Sampler> Texture::sampler() const {
    return _sampler;
}

void Texture::sampler(const shared_ptr<Sampler>& sampler) {
    _sampler = sampler;
    _dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::Sampler);
}

const Texture::Contents& Texture::contents() const {
    return _contents;
}

void Texture::contents(const Contents& contents) {
    _contents = contents;
    _dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::Contents);
}

unsigned Texture::mappingChannel() const {
    return _mappingChannel;
}

void Texture::mappingChannel(unsigned channel) {
    _mappingChannel = channel;
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
