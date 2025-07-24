//
//  Sampler.h
//  avara3d
//
//  Created by Morgan Davis on 2/7/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SAMPLER_H
#define AVARA3D_SAMPLER_H


#include "a3d/Types.h"


namespace a3d {


	class Color;
	class Image;
//	class Sampleable;


	class Sampler {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		Sampler();
		~Sampler();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		FilterMode 							minificationFilter() const;
		void 								minificationFilter(FilterMode mode);

		FilterMode 							magnificationFilter() const;
		void 								magnificationFilter(FilterMode mode);

		float 								maxAnisotropy() const;
		void 								maxAnisotropy(float max);

		WrapMode 							wrapS() const;
		void 								wrapS(WrapMode mode);

		WrapMode 							wrapT() const;
		void 								wrapT(WrapMode mode);

		WrapMode 							wrapR() const;
		void 								wrapR(WrapMode mode);

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		SamplerDirtyMask 					dirtyMask() const;
		void 								dirtyMask(SamplerDirtyMask mask);

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		FilterMode							_minificationFilter;
		FilterMode							_magnificationFilter;
		float								_maxAnisotropy;
		WrapMode							_wrapS;
		WrapMode							_wrapT;
		WrapMode							_wrapR;

		SamplerDirtyMask					_dirtyMask;
	};
}


#endif //AVARA3D_SAMPLER_H
