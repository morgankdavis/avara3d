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

//	class Sampleable;
	class Sampler;

	class Texture {

	public:
		/// Public Lifecycle Functions ///

		Texture();
		explicit Texture(const Sampleable& contents,
						 const std::shared_ptr<Sampler>& sampler = std::make_shared<Sampler>(),
						 unsigned mappingChannel = 0);
		~Texture();

		/// Public Member Functions ///

		std::shared_ptr<Sampler>		sampler() const;
		void							sampler(const std::shared_ptr<Sampler>& sampler);

		const Sampleable&				contents() const;
		void							contents(const Sampleable& contents);

		unsigned						mappingChannel() const;
		void							mappingChannel(unsigned channel);

		TextureDirtyMask 				dirtyMask() const;
		void 							dirtyMask(TextureDirtyMask mask);

	private:
		/// Private Member Variables ///

		std::shared_ptr<Sampler>		_sampler;
		Sampleable						_contents;
		unsigned						_mappingChannel;
		TextureDirtyMask				_dirtyMask;
	};
}

#endif //AVARA3D_TEXTURE_H
