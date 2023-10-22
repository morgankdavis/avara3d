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
						   ae::Bounds bounds);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		ae::Bounds 				bounds() const;
		void	 				bounds(const ae::Bounds& b);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 					constructProjection() override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		ae::Bounds 				_bounds;
	};
}

#endif /* OrthographicCamera_h */
