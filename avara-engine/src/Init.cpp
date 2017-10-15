//
//  Init.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Init.h"

#include <iostream>

//#if defined(EXPERIMENTAL)
//#if defined(NANOGUI_GLAD)
//#if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
//        #define GLAD_GLAPI_EXPORT
//    #endif
//
//    #include <glad/glad.h>
//#else
//#if defined(__APPLE__)
//#define GLFW_INCLUDE_GLCOREARB
//#else
//#define GL_GLEXT_PROTOTYPES
//#endif
//#endif
//#endif // EXPERIMENTAL


#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
//#if defined(EXPERIMENTAL)
//#include <nanogui/nanogui.h>
//#endif
#include <assimp/version.h>

#include "Globals.h"


using namespace std;


GLFWwindow				*g_glfwWindow;


#define DEFAULT_WIDTH		640
#define DEFAULT_HEIGHT		480

#define FULL_SCREEN			false
#define ENABLE_VSYNC		false


void glfwErrorCallback(int error, const char* description) {
	cout << "glfwErrorCallback(): error: " << error
	<< ", description: "  << description << endl;
}


int ae::init() {
	cout << "init()" << endl;
	
	int glfwMajVers, glfwMinVers, glfwRev;
	glfwGetVersion(&glfwMajVers, &glfwMinVers, &glfwRev);
	cout << "Starting GLFW version " << glfwMajVers << "." << glfwMinVers << "." << glfwRev << endl;
	
	glfwSetErrorCallback(glfwErrorCallback);
	
	if (glfwInit()) {
		cout << "GLFW Initialized." << endl;
	}
	else {
		cout << "Error initializing GLFW." << endl;
		return -1;
	}

//#ifdef MACOS
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16);
//#endif

	if (FULL_SCREEN) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *vmode = glfwGetVideoMode(monitor);
		g_glfwWindow = glfwCreateWindow(vmode->width, vmode->height, "avara-engine", monitor, NULL);
	}
	else {
		g_glfwWindow = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "avara-engine", NULL, NULL);
	}

	if (!g_glfwWindow) {
		cout << "Error creating glfwWindow." << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(g_glfwWindow);
	glewExperimental = GL_TRUE;
	glewInit();

	if (!ENABLE_VSYNC) glfwSwapInterval(0);

	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "Version: " << version << endl;

	cout << "Assimp version: "
		 << aiGetVersionMajor() << "."
		 << aiGetVersionMinor() << "."
		 << aiGetVersionRevision() << endl;


	return 0;
}
