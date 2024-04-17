//
//  OrthographicCamera.h
//  avara3d
//
//  Created by Morgan Davis on 10/22/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_ORTHOGRAPHICCAMERA_H
#define AVARA3D_ORTHOGRAPHICCAMERA_H


#include "a3d/Types.h"
#include "a3d/rendering/camera/Camera.h"


namespace a3d {

	class OrthographicCamera : public Camera {

	public:

/*********************************************************************************************
	Public
 *********************************************************************************************/

		OrthographicCamera();
		explicit OrthographicCamera(AABB extent);
		OrthographicCamera(const std::string& name, AABB extent);
		~OrthographicCamera() override;

		AABB 		extent() const;
		void	 	extent(const AABB& e);

	protected:

/*********************************************************************************************
	Camera Protected
 *********************************************************************************************/

		void 		constructProjectionMatrix() override;

	private:

/*********************************************************************************************
	Private
 *********************************************************************************************/

		AABB 		_extent;
	};
}

#endif /* AVARA3D_ORTHOGRAPHICCAMERA_H */
