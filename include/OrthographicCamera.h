//
// Created by mkd on 10/22/23.
//

#ifndef OrthographicCamera_h
#define OrthographicCamera_h


//#include <glm/vec4.hpp>

#include "Camera.h"
#include "Types.h"


namespace ae {

	class OrthographicCamera : public Camera {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		OrthographicCamera();
		OrthographicCamera(std::optional<std::string> name,
						   float zNear, float zFar,
						   ae::Extent3D extent);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		ae::Extent3D 			extent() const;
		void	 				extent(const ae::Extent3D& e);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 					constructProjection() override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		ae::Extent3D 			_extent;
	};
}

#endif /* OrthographicCamera_h */
