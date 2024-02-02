//
// Created by mkd on 10/22/23.
//

#ifndef OrthographicCamera_h
#define OrthographicCamera_h


//#include <glm/vec4.hpp>

#include "ae/Types.h"
#include "ae/rendering/camera/Camera.h"


namespace ae {

	class OrthographicCamera : public Camera {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		OrthographicCamera();
		OrthographicCamera(AABB extent);
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
