//
// Created by mkd on 10/22/23.
//

#ifndef OrthographicCamera_h
#define OrthographicCamera_h


#include "a3d/Types.h"
#include "a3d/rendering/camera/Camera.h"


namespace a3d {

	class OrthographicCamera : public Camera {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		OrthographicCamera();
		explicit OrthographicCamera(AABB extent);
		OrthographicCamera(std::string name, AABB extent);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		AABB 				extent() const;
		void	 			extent(const AABB& e);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 				constructProjectionMatrix() override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		AABB 			_extent;
	};
}

#endif /* OrthographicCamera_h */
