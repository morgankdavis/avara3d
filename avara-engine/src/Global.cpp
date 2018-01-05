//
//  Global.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Global.h"

#include <iostream>
#include <time.h>

#include <assimp/version.h>
#include <GL/glew.h>
//#define GLFW_DLL
#include <GLFW/glfw3.h>


using namespace ae;
using namespace spdlog;
using namespace std;


/***************************************************************************************
     MARK:   Global vars
 **************************************************************************************/

std::shared_ptr<spdlog::logger>		ae::g_logger;
//std::shared_ptr<spdlog::logger>		APP_LOG;

int 	ae::g_glfwLastErrorCode;
char* 	ae::g_glfwLastErrorDescription;

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void glfwErrorCallback(int error, const char* description) {
	cout << "glfwErrorCallback(): error: " << error << ", description: "  << description << endl;
	
	g_glfwLastErrorCode = error;
	if (g_glfwLastErrorDescription) {
		free(g_glfwLastErrorDescription);
	}
	g_glfwLastErrorDescription = (char *)malloc(strlen(description));
	strcpy(g_glfwLastErrorDescription, description);
}

int ae::initLog() {
	
	static bool initialized = false;
	
	if (!initialized) {
		try {
			set_async_mode(pow(2, LOG_QUEUE_SIZE)); // queue size must be power of 2
			
			//set_level(spd::level::info); //Set global log level to info
			
			vector<sink_ptr> sinks;
			if (LOG_ENABLE_STDOUT) {
				sinks.push_back(make_shared<sinks::stdout_sink_st>());
			}
			sinks.push_back(make_shared<sinks::rotating_file_sink_mt>(LOG_FILE_NAME,
																	  LOG_FILE_SIZE,
																	  LOG_FILE_ROTATIONS));
			g_logger = make_shared<logger>("ae", begin(sinks), end(sinks));
			//APP_LOG = make_shared<logger>("app", begin(sinks), end(sinks));
			
			register_logger(g_logger);
			//register_logger(APP_LOG);
			
			// Under VisualStudio, this must be called before main finishes to workaround a known VS issue
			//drop_all();
			
			// https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
			set_pattern("[%Y-%d-%m %H:%M:%S.%e] [%n] [%l]\t%v");
			
			
			set_level(LOG_LEVEL);
			
			g_logger->flush_on(LOG_FLUSH_LEVEL);
			//APP_LOG->flush_on(LOG_FLESH_LEVEL);
			
			
			g_logger->info("Init.");
		}
		catch (const spdlog_ex& ex) {
			
			cout << "Log initialization failed: " << ex.what() << endl;
		}
		
		initialized = true;
	}
	
	return 0;
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
