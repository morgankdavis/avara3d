//
//  PerspectiveCamera.h
//  avara3d
//
//  Created by Morgan Davis on 10/22/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PERSPECTIVECAMERA_H
#define AVARA3D_PERSPECTIVECAMERA_H


#include "a3d/rendering/camera/Camera.h"


namespace a3d {

	class PerspectiveCamera : public Camera {

	public:

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		PerspectiveCamera();
		PerspectiveCamera(float zNear, float zFar, float yFov);
		PerspectiveCamera(const std::string& name, float zNear, float zFar, float yFov);
		~PerspectiveCamera() override;

		float 			zNear() const;
		void 			zNear(float zNear);

		float 			zFar() const;
		void			zFar(float zFar);

		float 			yFov() const;
		void	 		yFov(float fov);

		float 			aspectRatio() const;
		void 			aspectRatio(float ratio);

	protected:

/*********************************************************************************************
	Camera Protected
 *********************************************************************************************/

		void 			constructProjectionMatrix() override;

	private:

/*********************************************************************************************
	Private
 *********************************************************************************************/

		float			_zNear;
		float			_zFar;
		float			_yFov;
		float			_aspectRatio;
	};
}

#endif /* AVARA3D_PERSPECTIVECAMERA_H */
