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


using namespace spdlog;
using namespace std;


/***************************************************************************************
     MARK:   Configuration
 **************************************************************************************/

#define LOG_FILE_NAME	    "ae.log"
#define LOG_QUEUE_SIZE	    12
#define LOG_FILE_SIZE 		1024 * 1024 * 5
#define LOG_FILE_ROTATIONS	3

/***************************************************************************************
     MARK:   Global vars
 **************************************************************************************/

std::shared_ptr<spdlog::logger>		LOG;

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void glfwErrorCallback(int error, const char* description) {
	cout << "glfwErrorCallback(): error: " << error << ", description: "  << description << endl;
}

int ae::initLog() {
	
	static bool initialized = false;
	
	if (!initialized) {
		try {
			set_async_mode(pow(2, LOG_QUEUE_SIZE)); // queue size must be power of 2
            
            //set_level(spd::level::info); //Set global log level to info
            
//            struct my_type
//            {
//                int i;
//                template<typename OStream>
//                friend OStream& operator<<(OStream& os, const my_type &c)
//                {
//                    return os << "[my_type i="<<c.i << "]";
//                }
//            };
			
//			g_logger = rotating_logger_mt("ae-log", LOG_FILENAME, LOG_SIZE, LOG_ROTATIONS);
//			g_logger->info("Init.");

			vector<sink_ptr> sinks;
			sinks.push_back(make_shared<sinks::stdout_sink_st>());
			sinks.push_back(make_shared<sinks::rotating_file_sink_mt>(LOG_FILE_NAME, LOG_FILE_SIZE,
                                                                      LOG_FILE_ROTATIONS));
			LOG = make_shared<logger>("ae-log", begin(sinks), end(sinks));
			register_logger(LOG);
            
            // Under VisualStudio, this must be called before main finishes to workaround a known VS issue
            //drop_all(); 
            
            // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
            set_pattern("[%Y-%d-%m %H:%M:%S.%e] [%l] %v");

			LOG->info("Init.");
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
