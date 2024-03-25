//
// Created by mkd on 2/7/24.
//

#include "a3d/rendering/material/Texture.h"


using namespace a3d;
using namespace std;


Texture::Texture():
		_sampler{nullptr},
		_contents{nullptr},
		_mappingChannel{0},
		_dirtyMask{TextureDirtyMask::All} {}

Texture::Texture(const shared_ptr<Sampleable>& contents,
				 const shared_ptr<Sampler>& sampler,
				 unsigned mappingChannel):
		_sampler{sampler},
		_contents{contents},
		_mappingChannel{mappingChannel},
		_dirtyMask{TextureDirtyMask::All} { }

Texture::~Texture() {}

shared_ptr<Sampler> Texture::sampler() const {
	return _sampler;
}

void Texture::sampler(const shared_ptr<Sampler>& sampler) {
	_sampler = sampler;
}

shared_ptr<Sampleable> Texture::contents() const {
	return _contents;
}

void Texture::contents(const shared_ptr<Sampleable>& contents) {
	_contents = contents;
}

unsigned Texture::mappingChannel() const {
	return _mappingChannel;
}

void Texture::mappingChannel(unsigned channel) {
	_mappingChannel = channel;
}

TextureDirtyMask Texture::dirtyMask() const {
	return _dirtyMask;
}

void Texture::dirtyMask(TextureDirtyMask mask) {
	_dirtyMask = mask;
}
