//
//  Sampler.cc
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/material/Sampler.h"

#include "a3d/IdGenerator.h"

using namespace a3d;
using namespace std;

/// Public Lifecycle Functions ///

Sampler::Sampler():
// TODO: are these sensible?
		_id{IdGenerator<SamplerId>::next()},
		_minificationFilter{FilterMode::LinearMipmapLinear},
		_magnificationFilter{FilterMode::Linear},
		_maxAnisotropy{16},
		_wrapS{WrapMode::Repeat},
		_wrapT{WrapMode::Repeat},
		_wrapR{WrapMode::Repeat},
		_dirtyMask{SamplerDirtyMask::All} {}

Sampler::~Sampler() {}

/// Public Member Functions ///

FilterMode Sampler::minificationFilter() const {
	return _minificationFilter;
}

void Sampler::minificationFilter(FilterMode mode) {
	_minificationFilter = mode;

	_dirtyMask = A3D_MASK_ADD(_dirtyMask, SamplerDirtyMask::MinificationFilter);
}

FilterMode Sampler::magnificationFilter() const {
	return _magnificationFilter;
}

void Sampler::magnificationFilter(FilterMode mode) {
	_magnificationFilter = mode;

	_dirtyMask = A3D_MASK_ADD(_dirtyMask, SamplerDirtyMask::MagnificationFilter);
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

	_dirtyMask = A3D_MASK_ADD(_dirtyMask, SamplerDirtyMask::MaxAnisotropy);
}

WrapMode Sampler::wrapS() const {
	return _wrapS;
}

void Sampler::wrapS(WrapMode mode) {
	_wrapS = mode;

	_dirtyMask = A3D_MASK_ADD(_dirtyMask, SamplerDirtyMask::WrapS);
}

WrapMode Sampler::wrapT() const {
	return _wrapT;
}

void Sampler::wrapT(WrapMode mode) {
	_wrapT = mode;

	_dirtyMask = A3D_MASK_ADD(_dirtyMask, SamplerDirtyMask::WrapT);
}

WrapMode Sampler::wrapR() const {
	return _wrapR;
}

void Sampler::wrapR(WrapMode mode) {
	_wrapR = mode;

	_dirtyMask = A3D_MASK_ADD(_dirtyMask, SamplerDirtyMask::WrapR);
}

/// Internal Member Functions ///

SamplerId Sampler::id() const noexcept {
	return _id;
}

SamplerDirtyMask Sampler::dirtyMask() const {
	return _dirtyMask;
}

void Sampler::dirtyMask(SamplerDirtyMask mask) {
	_dirtyMask = mask;
}
