//
// Created by mkd on 2/7/24.
//

#ifndef AVARA3D_TEXTURE_H
#define AVARA3D_TEXTURE_H


#include <memory>

#include "a3d/Types.h"


namespace a3d {


	class Sampleable;
	class Sampler;


	class Texture {

	public:

		Texture();
		explicit Texture(const std::shared_ptr<Sampleable>& contents,
				const std::shared_ptr<Sampler>& sampler = std::make_shared<Sampler>(),
				int mappingChannel = 0);
		~Texture();

		std::shared_ptr<Sampler>		sampler() const;
		void							sampler(const std::shared_ptr<Sampler>& sampler);

		std::shared_ptr<Sampleable>		contents() const;
		void							contents(const std::shared_ptr<Sampleable>& contents);

		int								mappingChannel() const;
		void							mappingChannel(int channel);

		TextureDirtyMask 				dirtyMask() const;
		void 							dirtyMask(TextureDirtyMask mask);

	private:

		std::shared_ptr<Sampler>		_sampler;
		std::shared_ptr<Sampleable>		_contents;
		int								_mappingChannel;
		TextureDirtyMask				_dirtyMask;
	};
}


#endif //AVARA3D_TEXTURE_H
