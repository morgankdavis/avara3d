//
// Created by mkd on 2/7/24.
//

#ifndef AVARA3D_TEXTURE_H
#define AVARA3D_TEXTURE_H


#include <memory>

#include "a3d/Types.h"
#include "a3d/rendering/material/Sampler.h"


namespace a3d {


	class Sampleable;


	class Texture {

	public:

		Texture();
		Texture(std::shared_ptr<Sampleable> contents,
				std::shared_ptr<Sampler> sampler = std::make_shared<Sampler>(),
				unsigned mappingChannel = 0);
		~Texture();

		std::shared_ptr<Sampler>		sampler() const;
		void							sampler(std::shared_ptr<Sampler> sampler);

		std::shared_ptr<Sampleable>		contents() const;
		void							contents(std::shared_ptr<Sampleable> contents);

		unsigned						mappingChannel() const;
		void							mappingChannel(unsigned channel);

		TextureDirtyMask 				dirtyMask() const;
		void 							dirtyMask(TextureDirtyMask mask);

	private:

		std::shared_ptr<Sampler>		_sampler;
		std::shared_ptr<Sampleable>		_contents;
		unsigned						_mappingChannel;

		TextureDirtyMask				_dirtyMask;
	};
}


#endif //AVARA3D_TEXTURE_H
