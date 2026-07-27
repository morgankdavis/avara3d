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

#if defined(A3D_GL_WEB)
	#include <GLES3/gl3.h>
	#include <GLES2/gl2ext.h>
	#include <webgl/webgl1_ext.h>
#elif defined(A3D_GL_ES)
	#include <EGL/egl.h>
	#include <GLES3/gl3.h>
	#include <GLES2/gl2ext.h>
#elif defined(A3D_GL_DESKTOP)
	// GLAD must be included before anything that might include <GL/gl.h>
	// (Qt's qopenglext, some platform headers, etc)
	#include <glad/glad.h>
#else
	#error "No OpenGL target selected."
#endif

#endif //AVARA3D_RENDER_BACKEND_OPENGL_GL_H
