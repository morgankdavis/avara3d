//
// Created by mkd on 12/14/25.
//

#ifndef AVARA3D_OPENGLDRAWITEM_H
#define AVARA3D_OPENGLDRAWITEM_H

#include "a3d/Math.h"

namespace a3d {

	class Program;

	struct OpenGLDrawItem {
		Program*	program;
		math::mat4	model;
		math::mat4 	view;
		math::mat4 	projection;
		unsigned 	vao;
		unsigned 	ebo;
		unsigned 	numElements;
	};
}

#endif //AVARA3D_OPENGLDRAWITEM_H
