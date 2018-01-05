//
//  Init.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Init.h"

#include <iostream>
#include <time.h>

#include <assimp/version.h>
#include <GL/glew.h>
//#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "Globals.h"




using namespace spdlog;
using namespace std;


/***************************************************************************************
     MARK:   Configuration
 **************************************************************************************/

#define LOG_FILE_NAME	    "ae.log"
#define LOG_QUEUE_SIZE	    12
#define LOG_FILE_SIZE 		1024 * 1024 * 5
#define LOG_FILE_ROTATIONS	3
#define LOG_ENABLE_STDOUT	true
#define LOG_LEVEL			level::trace
#define LOG_FLESH_LEVEL		level::warn


//typedef enum
//{
//	trace = 0,
//	debug = 1,
//	info = 2,
//	warn = 3,
//	err = 4,
//	critical = 5,
//	off = 6
//} level_enum;


/***************************************************************************************
     MARK:   Global vars
 **************************************************************************************/

std::shared_ptr<spdlog::logger>		g_logger;
//std::shared_ptr<spdlog::logger>		APP_LOG;

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
            set_pattern("[%Y-%d-%m %H:%M:%S.%e] [%n] [%l] %v");
			
			
			set_level(LOG_LEVEL);

			g_logger->flush_on(LOG_FLESH_LEVEL);
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



//auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logfile", 23, 59);
//// create synchronous  loggers
//auto net_logger = std::make_shared<spdlog::logger>("net", daily_sink);
//auto hw_logger  = std::make_shared<spdlog::logger>("hw",  daily_sink);
//auto db_logger  = std::make_shared<spdlog::logger>("db",  daily_sink);
//
//net_logger->set_level(spdlog::level::critical); // independent levels
//hw_logger->set_level(spdlog::level::debug);
//
//// globally register the loggers so so the can be accessed using spdlog::get(logger_name)
//spdlog::register_logger(net_logger);



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
