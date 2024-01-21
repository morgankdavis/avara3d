//
// Created by mkd on 10/22/23.
//

#ifndef PerspectiveCamera_h
#define PerspectiveCamera_h


#include "ae/rendering/camera/Camera.h"


namespace ae {

	class PerspectiveCamera : public Camera {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PerspectiveCamera();
		PerspectiveCamera(float zNear, float zFar, float fov);
		PerspectiveCamera(std::optional<std::string> name, float zNear, float zFar, float fov);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 					zNear() const;
		void 					zNear(float zNear);

		float 					zFar() const;
		void				 	zFar(float zFar);

		float 					fov() const;
		void	 				fov(float fov);

		float 					aspectRatio() const;
		void 					aspectRatio(float ratio);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

	protected:

		void 					constructProjectionMatrix() override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float					_zNear;
		float					_zFar;
		float					_fov;
		float					_aspectRatio;
	};
}

#endif /* PerspectiveCamera_h */
