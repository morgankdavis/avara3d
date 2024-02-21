//
// Created by mkd on 2/7/24.
//

#include "ae/rendering/material/Sampler.h"


using namespace ae;
using namespace std;


Sampler::Sampler():
		_wrapS(WrapMode::Repeat),
		_wrapT(WrapMode::Repeat),
		_wrapR(WrapMode::Repeat),
		_minificationFilter(FilterMode::LinearMipmapLinear),
		_magnificationFilter(FilterMode::Linear),
		_maxAnisotropy(16),
		_dirtyMask(SamplerDirtyMask::All) {}

Sampler::~Sampler() {}

FilterMode Sampler::minificationFilter() const {
	return _minificationFilter;
}

void Sampler::minificationFilter(FilterMode mode) {
	_minificationFilter = mode;

	_dirtyMask = AE_MASK_ADD(_dirtyMask, SamplerDirtyMask::MinificationFilter);
}

FilterMode Sampler::magnificationFilter() const {
	return _magnificationFilter;
}

void Sampler::magnificationFilter(FilterMode mode) {
	_magnificationFilter = mode;

	_dirtyMask = AE_MASK_ADD(_dirtyMask, SamplerDirtyMask::MagnificationFilter);
}

float Sampler::maxAnisotropy() const {
#ifdef OPENGL_ES
	return 0;
#else
	return _maxAnisotropy;
#endif
}

void Sampler::maxAnisotropy(float max) {
#ifdef OPENGL_ES
	throw Exception("Anisotropy is not supported on this platform.");
#endif

	_maxAnisotropy = max;

	_dirtyMask = AE_MASK_ADD(_dirtyMask, SamplerDirtyMask::MaxAnisotropy);
}

WrapMode Sampler::wrapS() const {
	return _wrapS;
}

void Sampler::wrapS(WrapMode mode) {
	_wrapS = mode;

	_dirtyMask = AE_MASK_ADD(_dirtyMask, SamplerDirtyMask::WrapS);
}

WrapMode Sampler::wrapT() const {
	return _wrapT;
}

void Sampler::wrapT(WrapMode mode) {
	_wrapT = mode;

	_dirtyMask = AE_MASK_ADD(_dirtyMask, SamplerDirtyMask::WrapT);
}

WrapMode Sampler::wrapR() const {
	return _wrapR;
}

void Sampler::wrapR(WrapMode mode) {
	_wrapR = mode;

	_dirtyMask = AE_MASK_ADD(_dirtyMask, SamplerDirtyMask::WrapR);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

SamplerDirtyMask Sampler::dirtyMask() const {
	return _dirtyMask;
}

void Sampler::dirtyMask(SamplerDirtyMask mask) {
	_dirtyMask = mask;
}
