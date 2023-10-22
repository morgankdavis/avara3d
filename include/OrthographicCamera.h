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
		OrthographicCamera(Extent extent);
		OrthographicCamera(std::optional<std::string> name, Extent extent);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		Extent 			extent() const;
		void	 			extent(const Extent& e);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 				constructProjectionMatrix() override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		Extent 			_extent;
	};
}

#endif /* OrthographicCamera_h */
