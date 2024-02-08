//
// Created by mkd on 2/7/24.
//

#include "ae/rendering/materials/Texture.h"


using namespace ae;
using namespace std;


Texture::Texture():
		_sampler{nullptr},
		_sampleable{nullptr},
		_mappingChannel{0} {}

Texture::Texture(shared_ptr<Sampler> sampler,
				 shared_ptr<Sampleable> sampleable,
				 unsigned mappingChannel):
		_sampler{sampler},
		_sampleable{sampleable},
		_mappingChannel{mappingChannel} { }

Texture::~Texture() {}

shared_ptr<Sampler> Texture::sampler() const {
	return _sampler;
}

void Texture::sampler(shared_ptr<Sampler> sampler) {
	_sampler = sampler;
}

shared_ptr<Sampleable> Texture::sampleable() const {
	return _sampleable;
}

void Texture::sampleable(shared_ptr<Sampleable> sampleable) {
	_sampleable = sampleable
}

unsigned Texture::mappingChannel() const {
	return _mappingChannel;
}

void Texture::mappingChannel(unsigned channel) {
	_mappingChannel = channel;
}
