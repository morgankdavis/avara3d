//
// Created by mkd on 10/22/23.
//

#ifndef PerspectiveCamera_h
#define PerspectiveCamera_h



#include "Camera.h"


namespace ae {

	class PerspectiveCamera : Camera {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PerspectiveCamera();
		PerspectiveCamera(float zNear, float zFar, float fov);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 							fov() const;
		void	 						fov(float fov);

		float 							aspectRatio() const;
		void 							aspectRatio(float ratio);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float							_fov;
		float							_aspectRatio;
	};
}

#endif /* PerspectiveCamera_h */
