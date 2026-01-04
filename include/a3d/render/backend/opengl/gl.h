//
//  gl.h
//  avara3d
//
//  Created by Morgan Davis on 12/31/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

// prevents Qt/GLFW/GLAD include-order conflicts

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_GL_H
#define AVARA3D_RENDER_BACKEND_OPENGL_GL_H

#ifdef A3D_GL_ES
	#include <EGL/egl.h>
	#include <GLES3/gl3.h>
#else
// GLAD must be included before anything that might include <GL/gl.h>
// (Qt's qopenglext, some platform headers, etc)
	#include <glad/glad.h>
#endif

#endif //AVARA3D_RENDER_BACKEND_OPENGL_GL_H
