//
//  Sampler.cc
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/material/Sampler.h"

#include <utility>

using namespace std;

namespace a3d {

// [Public Lifecycle Functions]

Sampler::Sampler():
    // TODO: are these sensible?
    _minificationFilter {FilterMode::LinearMipmapLinear},
    _magnificationFilter {FilterMode::Linear},
    _maxAnisotropy {4},
    _wrapS {WrapMode::Repeat},
    _wrapT {WrapMode::Repeat},
    _wrapR {WrapMode::Repeat} {}

// [Public Member Functions]

Sampler::FilterMode Sampler::minificationFilter() const {
    return _minificationFilter;
}

void Sampler::minificationFilter(FilterMode mode) {
    _minificationFilter = mode;
}

Sampler::FilterMode Sampler::magnificationFilter() const {
    return _magnificationFilter;
}

void Sampler::magnificationFilter(FilterMode mode) {
    _magnificationFilter = mode;
}

float Sampler::maxAnisotropy() const {
#ifdef A3D_GL_ES
    return 0;
#else
    return _maxAnisotropy;
#endif
}

void Sampler::maxAnisotropy(float max) {
#ifdef A3D_GL_ES
    throw Exception("Anisotropy is not supported on this platform.");
#endif

    _maxAnisotropy = max;
}

Sampler::WrapMode Sampler::wrapS() const {
    return _wrapS;
}

void Sampler::wrapS(WrapMode mode) {
    _wrapS = mode;
}

Sampler::WrapMode Sampler::wrapT() const {
    return _wrapT;
}

void Sampler::wrapT(WrapMode mode) {
    _wrapT = mode;
}

Sampler::WrapMode Sampler::wrapR() const {
    return _wrapR;
}

void Sampler::wrapR(WrapMode mode) {
    _wrapR = mode;
}

} // namespace a3d
