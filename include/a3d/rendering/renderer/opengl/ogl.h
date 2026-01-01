//
// Created by mkd on 12/31/25.
//

#ifndef AVARA3D_OGL_H
#define AVARA3D_OGL_H

// include this ONLY from .cc/.mm files that make GL calls
// do NOT include this from public headers (OpenGLRenderer.h etc)
// prevents Qt/GLFW/GLAD include-order conflicts.

#ifdef A3D_GL_ES
	#include <EGL/egl.h>
	#include <GLES3/gl3.h>
#else
// GLAD must be included before anything that might include <GL/gl.h>
// (Qt's qopenglext, some platform headers, etc)
	#include <glad/glad.h>
#endif

#endif //AVARA3D_OGL_H
