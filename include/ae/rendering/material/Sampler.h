//
// Created by mkd on 2/7/24.
//

#ifndef AVARA_ENGINE_SAMPLER_H
#define AVARA_ENGINE_SAMPLER_H


#include "ae/Types.h"


namespace ae {


	class Color;
	class Image;
	class Sampleable;

	class Sampler {

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

//	public:
//
//		static std::unique_ptr<Sampler> DefaultSampler();

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Sampler();
		~Sampler();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		FilterMode 									minificationFilter() const;
		void 										minificationFilter(FilterMode mode);

		FilterMode 									magnificationFilter() const;
		void 										magnificationFilter(FilterMode mode);

		float 										maxAnisotropy() const;
		void 										maxAnisotropy(float max);

		WrapMode 									wrapS() const;
		void 										wrapS(WrapMode mode);

		WrapMode 									wrapT() const;
		void 										wrapT(WrapMode mode);

		WrapMode 									wrapR() const;
		void 										wrapR(WrapMode mode);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		SamplerDirtyMask 							dirtyMask() const;
		void 										dirtyMask(SamplerDirtyMask mask);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		FilterMode									_minificationFilter;
		FilterMode									_magnificationFilter;
		float										_maxAnisotropy;
		WrapMode									_wrapS;
		WrapMode									_wrapT;
		WrapMode									_wrapR;

		SamplerDirtyMask							_dirtyMask;
	};
}


#endif //AVARA_ENGINE_SAMPLER_H
