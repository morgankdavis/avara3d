//
//  Texture.h
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
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

/*********************************************************************************************
	Public
 *********************************************************************************************/

		Texture();
		explicit Texture(const std::shared_ptr<Sampleable>& contents,
						 const std::shared_ptr<Sampler>& sampler = std::make_shared<Sampler>(),
						 unsigned mappingChannel = 0);
		~Texture();

		std::shared_ptr<Sampler>		sampler() const;
		void							sampler(const std::shared_ptr<Sampler>& sampler);

		std::shared_ptr<Sampleable>		contents() const;
		void							contents(const std::shared_ptr<Sampleable>& contents);

		unsigned						mappingChannel() const;
		void							mappingChannel(unsigned channel);

		TextureDirtyMask 				dirtyMask() const;
		void 							dirtyMask(TextureDirtyMask mask);

	private:

/*********************************************************************************************
	Private
 *********************************************************************************************/

		std::shared_ptr<Sampler>		_sampler;
		std::shared_ptr<Sampleable>		_contents;
		unsigned						_mappingChannel;
		TextureDirtyMask				_dirtyMask;
	};
}


#endif //AVARA3D_TEXTURE_H
