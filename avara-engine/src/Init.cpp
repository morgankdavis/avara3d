//
//  Init.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Init.h"

#include <iostream>

#include <GL/glew.h>
//#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <assimp/version.h>

#include "Globals.h"

#include <time.h>


using namespace std;


/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void glfwErrorCallback(int error, const char* description) {
	cout << "glfwErrorCallback(): error: " << error
	<< ", description: "  << description << endl;
}

int ae::initGLFW() {
	static bool initialized = false;
	
	if (!initialized) {
		cout << "initGLFW()" << endl;
		
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
	
		cout << "Assimp version: "
		<< aiGetVersionMajor() << "."
		<< aiGetVersionMinor() << "."
		<< aiGetVersionRevision() << endl;
		
		srand(time(NULL));
		
		initialized = true;
	}

	return 0;
}

int ae::initGLEW() {
	cout << "initGLEW()" << endl;
	
	// must set OpenGL context first
	
	static bool initialized = false;
	if (!initialized) {
		glewExperimental = GL_TRUE;
		glewInit();
		
		const GLubyte *renderer = glGetString(GL_RENDERER);
		const GLubyte *version = glGetString(GL_VERSION);
		cout << "Renderer: " << renderer << endl;
		cout << "Version: " << version << endl;
		
		initialized = true;
	}
	
	return 0;
}
