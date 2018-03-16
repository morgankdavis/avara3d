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

#define FONTSTASH_IMPLEMENTATION
#include "fontstash.h"

#include <assimp/version.h>
#include <GL/glew.h>
//#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <LinearMath/btScalar.h> // for bullet version (!)

#define GLFONTSTASH_IMPLEMENTATION
#include "gl3fontstash.h"

#include "Logger.h"
#include "LoggerManager.h"
#include "Types.h"


using namespace ae;
using namespace spdlog;
using namespace std;


/***************************************************************************************
     MARK:   Globals
 **************************************************************************************/

shared_ptr<Logger>		ae::g_aeLogger;
int 					ae::g_glfwLastErrorCode;
char* 					ae::g_glfwLastErrorDescription;

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
		
		
//		g_spdlogMainFileSink = make_shared<sinks::rotating_file_sink_mt>(string(LOG_MAIN_FILE_NAME) + ".log",
//																		 LOG_FILE_SIZE,
//																		 LOG_FILE_ROTATIONS);
//		g_spdlogSTDOUTSink = make_shared<sinks::stdout_sink_st>();
		
		
		

//		g_loggerManager = make_shared<LoggerManager>();
//
//
//		// if (LOG_ENABLE_STDOUT)
//
//
		LOGGER_SINKS sinks = LOGGER_SINKS::NONE;
		if (LOG_ENABLE_STDOUT) sinks = LOGGER_SINKS_ADD(sinks, LOGGER_SINKS::STDOUT);
		sinks = LOGGER_SINKS_ADD(sinks, LOGGER_SINKS::MAIN_FILE);
		g_aeLogger = make_shared<Logger>("ae", sinks);
		g_aeLogger->info("Init.");
		
//		try {
//			set_async_mode(pow(2, LOG_QUEUE_SIZE)); // queue size must be power of 2
//
//			//set_level(spd::level::info); //Set global log level to info
//
//			vector<sink_ptr> sinks;
//			if (LOG_ENABLE_STDOUT) {
//				sinks.push_back(make_shared<sinks::stdout_sink_st>());
//			}
//			sinks.push_back(make_shared<sinks::rotating_file_sink_mt>((LOG_MAIN_FILE_NAME + ".log"),
//																	  LOG_FILE_SIZE,
//																	  LOG_FILE_ROTATIONS));
//			g_logger = make_shared<logger>("ae", begin(sinks), end(sinks));
//			//APP_LOG = make_shared<logger>("app", begin(sinks), end(sinks));
//
//			register_logger(g_logger);
//			//register_logger(APP_LOG);
//
//			// Under VisualStudio, this must be called before main finishes to workaround a known VS issue
//			//drop_all();
//
//			// https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
//			set_pattern("[%Y-%d-%m %H:%M:%S.%e] [%n] [%l]\t%v");
//
//
//			set_level(LOG_LEVEL);
//
//			g_logger->flush_on(LOG_FLUSH_LEVEL);
//			//APP_LOG->flush_on(LOG_FLESH_LEVEL);
//
//
//			g_logger->info("Init.");
//		}
//		catch (const spdlog_ex& ex) {
//
//			cout << "Log initialization failed: " << ex.what() << endl;
//		}
		
		initialized = true;
	}
	
	return 0;
}

int ae::initGLFW() {
	static bool initialized = false;
	
	if (!initialized) {
		AE_LOG->trace("initGLFW()");
		
		int glfwMajVers, glfwMinVers, glfwRev;
		glfwGetVersion(&glfwMajVers, &glfwMinVers, &glfwRev);
		//cout << "Starting GLFW version " << glfwMajVers << "." << glfwMinVers << "." << glfwRev << endl;
		AE_LOG->info("Starting GLFW version {}.{}.{}", glfwMajVers, glfwMinVers, glfwRev);
		
		glfwSetErrorCallback(glfwErrorCallback);
		
		if (glfwInit()) {
			AE_LOG->info("GLFW Initialized.");
		}
		else {
			AE_LOG->critical("Error initializing GLFW.");
			return -1;
		}

		AE_LOG->info("Bullet version: {}",  btGetVersion());
		
		AE_LOG->info("Assimp version: {}.{}.{}",
					aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
		
		srand(time(NULL));
		
		initialized = true;
	}
	
	return 0;
}

int ae::initGLEW() {
	AE_LOG->trace("initGLEW()");
	
	// must set OpenGL context first
	
	static bool initialized = false;
	if (!initialized) {
		glewExperimental = GL_TRUE;
		glewInit();
		
		const GLubyte *renderer = glGetString(GL_RENDERER);
		const GLubyte *version = glGetString(GL_VERSION);
		AE_LOG->info("Renderer: {}", renderer);
		AE_LOG->info("Version: {}", version);
		
		initialized = true;
	}
	
	return 0;
}
