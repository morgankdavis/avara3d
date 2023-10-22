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
//		OrthographicCamera(glm::vec4 bounds); // left, right, top, botton
//		OrthographicCamera(float left, float right, float top, float botton);
		OrthographicCamera(ae::Bounds bounds);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		ae::Bounds 						bounds() const;
		void	 						bounds(ae::Bounds b);

//		glm::vec4 						bounds() const;
//		void	 						bounds(glm::vec4 b);

//		float 							left() const;
//		void	 						left(float l);
//
//		float 							right() const;
//		void	 						right(float r);
//
//		float 							top() const;
//		void	 						top(float t);
//
//		float 							bottom() const;
//		void	 						bottom(float b);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

//		glm::vec4 						_bounds;
		ae::Bounds 						_bounds;
	};
}

#endif /* OrthographicCamera_h */
