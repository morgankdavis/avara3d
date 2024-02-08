//
// Created by mkd on 2/7/24.
//

#ifndef AVARA_ENGINE_TEXTURE_H
#define AVARA_ENGINE_TEXTURE_H


#include <memory>

#include "ae/Types.h"


namespace ae {


	class Sampler;
	class Sampleable;


	class Texture {

	public:

		Texture();
		Texture(std::shared_ptr<Sampler> sampler,
				std::shared_ptr<Sampleable> sampleable,
				unsigned mappingChannel = 0);
		~Texture();

		std::shared_ptr<Sampler>		sampler() const;
		void							sampler(std::shared_ptr<Sampler> sampler);

		std::shared_ptr<Sampleable>		sampleable() const;
		void							sampleable(std::shared_ptr<Sampleable> sampleable);

		unsigned						mappingChannel() const;
		void							mappingChannel(unsigned channel);

	private:

		std::shared_ptr<Sampler>		_sampler;
		std::shared_ptr<Sampleable>		_sampleable;
		unsigned						_mappingChannel;
	};
}


#endif //AVARA_ENGINE_TEXTURE_H
