//
//  Sampler.cc
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/material/Sampler.h"

#include <utility>

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
		_dirtyMask{DirtyMask::All} {}

Sampler::~Sampler() {}

Sampler::Sampler(const Sampler& other):
		Sampler() {

	*this = other;
}

Sampler& Sampler::operator=(const Sampler& other) {

	if (this == &other) {
		return *this;
	}

	_minificationFilter = other._minificationFilter;
	_magnificationFilter = other._magnificationFilter;
	_maxAnisotropy = other._maxAnisotropy;
	_wrapS = other._wrapS;
	_wrapT = other._wrapT;
	_wrapR = other._wrapR;
	_dirtyMask = DirtyMask::All;

	return *this;
}

Sampler::Sampler(Sampler&& other):
		Sampler() {

	*this = std::move(other);
}

Sampler& Sampler::operator=(Sampler&& other) {

	if (this == &other) {
		return *this;
	}

	_minificationFilter = other._minificationFilter;
	_magnificationFilter = other._magnificationFilter;
	_maxAnisotropy = other._maxAnisotropy;
	_wrapS = other._wrapS;
	_wrapT = other._wrapT;
	_wrapR = other._wrapR;
	_dirtyMask = DirtyMask::All;

	return *this;
}

/// Public Member Functions ///

Sampler::FilterMode Sampler::minificationFilter() const {
	return _minificationFilter;
}

void Sampler::minificationFilter(FilterMode mode) {
	_minificationFilter = mode;

	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::MinificationFilter);
}

Sampler::FilterMode Sampler::magnificationFilter() const {
	return _magnificationFilter;
}

void Sampler::magnificationFilter(FilterMode mode) {
	_magnificationFilter = mode;

	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::MagnificationFilter);
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

	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::MaxAnisotropy);
}

Sampler::WrapMode Sampler::wrapS() const {
	return _wrapS;
}

void Sampler::wrapS(WrapMode mode) {
	_wrapS = mode;

	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::WrapS);
}

Sampler::WrapMode Sampler::wrapT() const {
	return _wrapT;
}

void Sampler::wrapT(WrapMode mode) {
	_wrapT = mode;

	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::WrapT);
}

Sampler::WrapMode Sampler::wrapR() const {
	return _wrapR;
}

void Sampler::wrapR(WrapMode mode) {
	_wrapR = mode;

	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::WrapR);
}

/// Internal Member Functions ///

SamplerId Sampler::id() const noexcept {
	return _id;
}

Sampler::DirtyMask Sampler::dirtyMask() const {
	return _dirtyMask;
}

void Sampler::dirtyMask(DirtyMask mask) {
	_dirtyMask = mask;
}
