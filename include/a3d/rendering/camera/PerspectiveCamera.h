//
// Created by mkd on 10/22/23.
//

#ifndef PerspectiveCamera_h
#define PerspectiveCamera_h


#include "a3d/rendering/camera/Camera.h"


namespace a3d {

	class PerspectiveCamera : public Camera {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PerspectiveCamera();
		PerspectiveCamera(float zNear, float zFar, float yFov);
		PerspectiveCamera(const std::string& name, float zNear, float zFar, float yFov);
		~PerspectiveCamera() override;

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 					zNear() const;
		void 					zNear(float zNear);

		float 					zFar() const;
		void				 	zFar(float zFar);

		float 					yFov() const; // radians
		void	 				yFov(float fov);

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
		float					_yFov;
		float					_aspectRatio;
	};
}

#endif /* PerspectiveCamera_h */
