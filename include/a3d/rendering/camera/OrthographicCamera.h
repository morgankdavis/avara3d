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
	Public Lifecycle Functions
 *********************************************************************************************/

		OrthographicCamera();
		explicit OrthographicCamera(const AABB& extent);
		OrthographicCamera(const std::string& name, const AABB& extent);
		~OrthographicCamera() override;

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		AABB 		extent() const;
		void	 	extent(const AABB& e);

/*********************************************************************************************
	Camera Protected Member Functions
 *********************************************************************************************/

	protected:

		void 		constructProjectionMatrix() override;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		AABB 		_extent;
	};
}

#endif /* AVARA3D_ORTHOGRAPHICCAMERA_H */
