//
// Created by mkd on 10/22/23.
//

#ifndef PerspectiveCamera_h
#define PerspectiveCamera_h


#include "Camera.h"


namespace ae {

	class PerspectiveCamera : public Camera {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PerspectiveCamera();
		PerspectiveCamera(std::optional<std::string> name, float zNear, float zFar, float fov);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 					fov() const;
		void	 				fov(float fov);

		float 					aspectRatio() const;
		void 					aspectRatio(float ratio);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 					constructProjection() override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float					_fov;
		float					_aspectRatio;
	};
}

#endif /* PerspectiveCamera_h */
