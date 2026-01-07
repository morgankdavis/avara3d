//
//  Texture.cc
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/material/Texture.h"

#include "a3d/IdGenerator.h"
#include "a3d/util/bitmask.h"

using namespace a3d;
using namespace std;

/// Public Lifecycle Functions ///

Texture::Texture():
		_id{IdGenerator<TextureId>::next()},
		_sampler{nullptr},
		_contents{monostate{}},
		_mappingChannel{0},
		_dirtyMask{TextureDirtyMask::All} {}

Texture::Texture(const Sampleable& contents,
				 const shared_ptr<Sampler>& sampler,
				 unsigned mappingChannel):
		Texture() {
	_sampler = sampler;
	_contents = contents;
	_mappingChannel = mappingChannel;
}

Texture::~Texture() {}

/// Public Member Functions ///

shared_ptr<Sampler> Texture::sampler() const {
	return _sampler;
}

void Texture::sampler(const shared_ptr<Sampler>& sampler) {
	_sampler = sampler;
	_dirtyMask = util::bitmask::add(_dirtyMask, TextureDirtyMask::Sampler);
}

const Sampleable& Texture::contents() const {
	return _contents;
}

void Texture::contents(const Sampleable& contents) {
	_contents = contents;
	_dirtyMask = util::bitmask::add(_dirtyMask, TextureDirtyMask::Contents);
}

unsigned Texture::mappingChannel() const {
	return _mappingChannel;
}

void Texture::mappingChannel(unsigned channel) {
	_mappingChannel = channel;
}

/// Internal Member Functions ///

TextureId Texture::id() const noexcept {
	return _id;
}

TextureDirtyMask Texture::dirtyMask() const {
	return _dirtyMask;
}

void Texture::dirtyMask(TextureDirtyMask mask) {
	_dirtyMask = mask;
}
