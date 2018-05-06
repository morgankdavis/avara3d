
//  Activity.cpp
//	avara-engine
//
//  Created by Morgan Davis on 5/3/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

//#if 0

#ifdef ANDROID


#include "Activity.h"

//#include <iostream>


#include <jni.h>
#include <errno.h>
#include <iostream>

#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>

#include "NDKHelper.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <sys/time.h>




//#include <GLFW/glfw3.h>
//#ifdef MACOS
//#define GLFW_EXPOSE_NATIVE_COCOA
//#endif
//#include <GLFW/glfw3native.h>
//#include <glm/gtc/matrix_transform.hpp>
//#include "stb_image_resize.h"
//
#include "Camera.h"
#include "Global.h"
//#include "InputManager.h"
#include "Logger.h"
#include "Node.h"
//#include "PhysicsWorld.h"
#include "Renderer.h"
#include "Scene.h"


// FROM ANDROID SAMPLE
#include <string.h>
#include <unistd.h>




using namespace std;
using namespace ae;


/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

//AndroidActivity::AndroidActivity(shared_ptr<Renderer> renderer):
//	RenderContext(renderer) {

	
//	int viewportWidth = width;
//	int viewportHeight = height;
//
//	float scaleFactor = 1.0;
//
//	enableVSync(false);
//
//	m_width = viewportWidth;
//	m_height = viewportHeight;
//
//	m_framebufferScale = (useHighDPI ? scaleFactor : 1.0);
//	m_framebufferWidth = m_width * m_framebufferScale;
//	m_framebufferHeight = m_height * m_framebufferScale;
//
//	m_antialiasingMode = antialiasingMode;
//	m_inputManager = nullptr;
//
//	m_recordingGIF = false;
//	m_cursorCaptured = false;
//	m_recordingGIF = false;
//	m_gifRecordingWidth = 0;
//	m_gifRecordingHeight = 0;
//	m_gifRecordingMaxFramerate = 0;
//	m_gifRecordedFrames = 0;
//}

//AndroidActivity::~AndroidActivity() {
//	
//}

Activity::Activity(shared_ptr<Renderer> renderer):
		RenderContext(renderer),
		m_window(nullptr),
		m_display(EGL_NO_DISPLAY),
		m_surface(EGL_NO_SURFACE),
		m_context(EGL_NO_CONTEXT),
		m_screenWidth(0),
		m_screenHeight(0),
		m_glesInitialized(false),
		m_eglContextInitialized(false) {

			
			
			m_debugOptions = DEBUG_OPTIONS::NONE;
			
			//if (initLog() != 0) { cout << "Error initializing log." << endl; }
			

//			RenderContext::renderer()->initialize();
//			
//			m_width = viewportWidth;
//			m_height = viewportHeight;
//			
//			m_framebufferScale = (useHighDPI ? scaleFactor : 1.0);
//			m_framebufferWidth = m_width * m_framebufferScale;
//			m_framebufferHeight = m_height * m_framebufferScale;
			
			
			//m_antialiasingMode = antialiasingMode;
			//m_inputManager = nullptr;
			
			m_recordingGIF = false;
			//m_cursorCaptured = false;
			m_recordingGIF = false;
			m_gifRecordingWidth = 0;
			m_gifRecordingHeight = 0;
			m_gifRecordingMaxFramerate = 0;
			m_gifRecordedFrames = 0;
			
}

Activity::~Activity() {
	terminate();
}

/***************************************************************************************
    		Public
 ***************************************************************************************/

void Activity::display(android_app* app) {
	// *** Move all this stuff into a function in AndroidActivity...
	// say "start()" that takes a handle to the android_app...

//	g_engine.SetState(state);
//
	ndk_helper::JNIHelper::Init(app->activity, "com/mkdinteractive/helper/NDKHelper");

	//app->userData = activity.get();
	app->userData = this;
	app->onAppCmd = Activity::HandleAppCommand;
	app->onInputEvent = Activity::HandleAppInput;

//#ifdef USE_NDK_PROFILER
//	monstartup("libTeapotNativeActivity.so");
//#endif

	LOGW("TestNativeActivity: android_main()!");
	while (true) {

		int id;
		int events;
		android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.

		while ((id = ALooper_pollAll(0, // is ready
									 NULL,
									 &events,
									 (void**)&source)) >= 0) {

			// Process this event.
			if (source != NULL) source->process(app, source);

			// Check if we are exiting.
//			if (state->destroyRequested != 0) {
//				g_engine.TermDisplay();
//				return;
//			}
		}

//		if (g_engine.IsReady()) {
//			// Drawing is throttled to the screen update rate, so there
//			// is no need to do timing here.
//			g_engine.DrawFrame();
//		}

		//activity->drawFrame();
		drawFrame();
	}
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

int Activity::initializeDisplay(android_app* app) {

	LOGW("initializeDisplay()");
	
	static bool initialized = false;
	
	if (!initialized) {

		LOGW("initializeDisplay!");
		

//	if (!initialized_resources) {
		initialize(app->window);
		//		LoadResources();
		//		initialized_resources_ = true;
//	}
//	else if(app->window != gl_context_->GetANativeWindow()) {
//		// Re-initialize ANativeWindow.
//		// On some devices, ANativeWindow is re-created when the app is resumed
//		//assert(gl_context_->GetANativeWindow());
//		assert(aNativeWindow());
//		//UnloadResources();
//		//gl_context_->Invalidate();
//		invalidate();
//		//app_ = app;
//		//gl_context_->Init(app->window);
//		initialize(app->window);
//		//LoadResources();
//		//initialized_resources_ = true;
//	}
//	else {
//		// initialize OpenGL ES and EGL
//		if (EGL_SUCCESS == resume(app->window)) {
//			//UnloadResources();
//			//LoadResources();
//		}
//		else {
//			assert(0);
//		}
//	}
	
	//	// Initialize GL state.
	//	glEnable(GL_CULL_FACE);
	//	glEnable(GL_DEPTH_TEST);
	//	glDepthFunc(GL_LEQUAL);
	//	
	//	// Note that screen size might have been changed
	//	glViewport(0, 0, gl_context_->GetScreenWidth(),
	//			   gl_context_->GetScreenHeight());
	//	renderer_.UpdateViewport();
	//	
	//	tap_camera_.SetFlip(1.f, -1.f, -1.f);
	//	tap_camera_.SetPinchTransformFactor(2.f, 2.f, 8.f);
	
	initialized = true;
}	

	return 0;
}

void Activity::drawFrame() {

	if (m_eglContextInitialized) {
	
		AE_LOG->trace("-------------------------------------------------------------------------------");

		m_renderer->beginFrame(*this);

#warning move to saveGIFFrame()
		float time = sceneTime();
		static double previousSeconds = time;
		float deltaSeconds = time - previousSeconds;
		previousSeconds = time;

		if (RenderContext::updateCallback()) RenderContext::updateCallback()(*this, sceneTime());

		auto pov = pointOfView();
		float aspectRatio = (float)m_framebufferWidth/(float)m_framebufferHeight;
		pov->camera()->aspectRatio(aspectRatio);

		m_renderer->renderStats().cameraPosition = pov->position();

		// simulate physics
		//	auto physicsWorld = m_scene->physicsWorld();
		//	if (physicsWorld) {
		//		physicsWorld->step();
		//		
		//		if (didSimulatePhysicsCallback()) {
		//			didSimulatePhysicsCallback()(*this, sceneTime());
		//		}
		//	}
		
		if (RenderContext::willRenderCallback()) RenderContext::willRenderCallback()(*this, sceneTime());

		m_scene->draw(*RenderContext::renderer(),
					  m_framebufferWidth, m_framebufferHeight,
					  *pov,
					  m_debugOptions, m_renderer->renderStats());

		m_renderer->endFrame(*this);
		
		//glfwSwapBuffers(m_glfwWindow);
		swap();
		
		//if (m_recordingGIF) saveGIFFrame(deltaSeconds);
		
		if (RenderContext::didRenderCallback()) RenderContext::didRenderCallback()(*this, sceneTime());
		
		//glfwPollEvents();
		//if (inputManager()) inputManager()->update();
	}
}

EGLint Activity::swap() {
	bool b = eglSwapBuffers(m_display, m_surface);
	if (!b) {
		EGLint err = eglGetError();
		if (err == EGL_BAD_SURFACE) {
			// Recreate surface
			initializeEGLSurface();
			return EGL_SUCCESS;  // Still consider glContext is valid
		}
		else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
			// Context has been lost!!
			m_contextValid = false;
			terminate();
			initializeEGLContext();
		}
		return err;
	}
	return EGL_SUCCESS;
}

void Activity::terminate() {

	if (m_display != EGL_NO_DISPLAY) {
		eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (m_context != EGL_NO_CONTEXT) {
			eglDestroyContext(m_display, m_context);
		}

		if (m_surface != EGL_NO_SURFACE) {
			eglDestroySurface(m_display, m_surface);
		}
		eglTerminate(m_display);
	}

	m_display = EGL_NO_DISPLAY;
	m_context = EGL_NO_CONTEXT;
	m_surface = EGL_NO_SURFACE;
	m_window = nullptr;
	m_contextValid = false;
}

EGLint Activity::resume(ANativeWindow* window) {

	LOGW("resume()");
	
	if (m_eglContextInitialized == false) {
		initialize(window);
		return EGL_SUCCESS;
	}
	else {
		int32_t originalWidth = m_screenWidth;
		int32_t originalHeight = m_screenHeight;

		// Create surface
		m_window = window;
		m_surface = eglCreateWindowSurface(m_display, m_config, m_window, NULL);
		eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_screenWidth);
		eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_screenHeight);

		if (m_screenWidth != originalWidth || m_screenHeight != originalHeight) {
			// Screen resized
			LOGW("Screen resized");
		}

		if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_TRUE)
			return EGL_SUCCESS;

		EGLint err = eglGetError();
		//LOGW("Unable to eglMakeCurrent %d", err);

		if (err == EGL_CONTEXT_LOST) {
			// Recreate context
			LOGW("Re-creating egl context");
			initializeEGLContext();
		}
		else {
			// Recreate surface
			terminate();
			initializeEGLSurface();
			initializeEGLContext();
		}

		return err;
	}
}

void Activity::suspend() {

	if (m_surface != EGL_NO_SURFACE) {
		eglDestroySurface(m_display, m_surface);
		m_surface = EGL_NO_SURFACE;
	}
}

bool Activity::invalidate() {

	terminate();
	m_eglContextInitialized = false;

	return true;
}

ANativeWindow* Activity::nativeWindow(void) const {
	return m_window;
}

int32_t Activity::screenWidth() const {
	return m_screenWidth;
}

int32_t Activity::screenHeight() const {
	return m_screenHeight;
}

int32_t Activity::bufferColorSize() const {
	return m_colorSize;
}

int32_t Activity::bufferDepthSize() const {
	return m_depthSize;
}

EGLDisplay Activity::display() const {
	return m_display;
}

EGLSurface Activity::surface() const {
	return m_surface;
}

void Activity::HandleAppCommand(struct android_app* app, int32_t cmd) {
	//LOGI("AndroidActivity::handleAppCommand()");

	
	Activity* activity = (Activity*)app->userData;

//	Engine* eng = (Engine*)app->userData;
//	switch (cmd) {
//		case APP_CMD_SAVE_STATE:
//			break;
//		case APP_CMD_INIT_WINDOW:
//			// The window is being shown, get it ready.
//			if (app->window != NULL) {
//				eng->InitDisplay(app);
//				eng->DrawFrame();
//			}
//			break;
//		case APP_CMD_TERM_WINDOW:
//			// The window is being hidden or closed, clean it up.
//			eng->TermDisplay();
//			//eng->has_focus_ = false;
//			break;
//		case APP_CMD_STOP:
//			break;
//		case APP_CMD_GAINED_FOCUS:
//			//eng->ResumeSensors();
//			// Start animation
//			//eng->has_focus_ = true;
//			break;
//		case APP_CMD_LOST_FOCUS:
//			//eng->SuspendSensors();
//			// Also stop animating.
//			//eng->has_focus_ = false;
//			eng->DrawFrame();
//			break;
//		case APP_CMD_LOW_MEMORY:
//			// Free up GL resources
//			eng->TrimMemory();
//			break;
//	}

	//	Engine* eng = (Engine*)app->userData;
	switch (cmd) {
		case APP_CMD_SAVE_STATE:
			break;
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			//if (activity->window != NULL) {
				activity->initializeDisplay(app);
				activity->drawFrame();
			//}
			break;
		case APP_CMD_TERM_WINDOW:
			// The window is being hidden or closed, clean it up.
			//activity->termDisplay();
			//eng->has_focus_ = false;
			break;
		case APP_CMD_STOP:
			break;
		case APP_CMD_GAINED_FOCUS:
			//eng->ResumeSensors();
			// Start animation
			//eng->has_focus_ = true;
			break;
		case APP_CMD_LOST_FOCUS:
			//eng->SuspendSensors();
			// Also stop animating.
			//eng->has_focus_ = false;
			activity->drawFrame();
			break;
		case APP_CMD_LOW_MEMORY:
			// Free up GL resources
			//activity->TrimMemory();
			break;
	}
}

int32_t Activity::HandleAppInput(android_app* app, AInputEvent* event) {
	//LOGI("AndroidActivity::handleAppInput()");

	Activity* activity = (Activity*)app->userData;

//	Engine* eng = (Engine*)app->userData;
	int32_t eventType = AInputEvent_getType(event);

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

		float xPos = AMotionEvent_getX(event, 0);
		float yPos = AMotionEvent_getY(event, 0);

		LOGW("xPos: %f, yPos: %f", xPos, yPos);
	}
	else if (eventType == AINPUT_EVENT_TYPE_KEY) {


		int32_t key_val = AKeyEvent_getKeyCode(event);
		//cout << "Received key event: " << key_val << endl;
		LOGW("Received key event: %d", key_val);

		if((key_val >= AKEYCODE_A && key_val <= AKEYCODE_Z)) {
			LOGW("LETTER");
		}
		return 0;
	}

	return 0;
}

/**************************************************************************************
     Renderer
 ***************************************************************************************/

//void Window::enableVSync(bool enabled) {
//	RenderContext::enableVSync(enabled);
//	
//	if (!enabled) glfwSwapInterval(0);
//	else glfwSwapInterval(1);
//}

void Activity::debugOptions(DEBUG_OPTIONS options) {
	RenderContext::debugOptions(options);
	
//#warning Refactor this
//	if (m_scene && m_scene->physicsWorld()) {
//		m_scene->physicsWorld()->debugOptions(m_debugOptions);
//	}
}

//shared_ptr<InputManager> Window::inputManager() {
//#warning Refactor this (DesktopInputManager?)
//	
//	if (m_inputManager == nullptr) {
//		m_inputManager = make_shared<InputManager>(this);
//	}
//	return m_inputManager;
//}

//float AndroidActivity::sceneTime() const {
//
////	struct timeval time;
////	gettimeofday(&time, NULL);
////	return time.tv_sec + time.tv_usec * 1.0 / 1000.0;
//
////	// should probably override in subclass to use library's time utilities (GLFW, for example)
////	static auto startDate = chrono::high_resolution_clock::now();
////	auto nowDate = chrono::high_resolution_clock::now();
////	return (chrono::duration<float>(nowDate - startDate)).count();
//}

/***************************************************************************************
     Private
 ***************************************************************************************/

void Activity::initializeGLES() {
	LOGW("initializeGLES()");
	
	if (m_eglContextInitialized) return;
	//
	// Initialize OpenGL ES 3 if available
	//
//	const char* versionStr = (const char*)glGetString(GL_VERSION);
//	//if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
//	if (strstr(versionStr, "OpenGL ES 3.")) {
//		es3_supported_ = true;
//		gl_version_ = 3.0f;
//	} else {
//		gl_version_ = 2.0f;
//	}

	m_eglContextInitialized = true;
}

bool Activity::initialize(ANativeWindow* window) {
	LOGW("initialize()");
	
	if (!m_eglContextInitialized) {
		m_window = window;
		initializeEGLSurface();
		initializeEGLContext();
		initializeGLES();
		
		
		
		RenderContext::renderer()->initialize();
		
		m_width = m_screenWidth;
		m_height = m_screenHeight;
		
//		m_framebufferScale = (useHighDPI ? scaleFactor : 1.0);
		m_framebufferScale = 1.0;
		m_framebufferWidth = m_width * m_framebufferScale;
		m_framebufferHeight = m_height * m_framebufferScale;
		
		
		

		m_eglContextInitialized = true;
	}
	return true;
}

bool Activity::initializeEGLSurface() {
	
	LOGW("initializeEGLSurface()");

	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(m_display, 0, 0);
	
	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE,
			EGL_OPENGL_ES2_BIT, // OpenGL ES 2.0 -- there is no 3.0 bit?
			EGL_SURFACE_TYPE,
			EGL_WINDOW_BIT,
			EGL_BLUE_SIZE,
			8,
			EGL_GREEN_SIZE,
			8,
			EGL_RED_SIZE,
			8,
			EGL_DEPTH_SIZE,
			24,
			EGL_NONE};
//			EGL_MIN_SWAP_INTERVAL,
//			0,
//			EGL_MAX_SWAP_INTERVAL,
//			0};
	m_colorSize = 8;
	m_depthSize = 24;
	
	EGLint num_configs;
	eglChooseConfig(m_display, attribs, &m_config, 1, &num_configs);
	
	if (!num_configs) {
		LOGW("Fall back to 16bit depth buffer");

		// Fall back to 16bit depth buffer
		const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE,
			EGL_OPENGL_ES2_BIT, // OpenGL ES 2.0 -- there is no 3.0 bit?
			EGL_SURFACE_TYPE,
			EGL_WINDOW_BIT,
			EGL_BLUE_SIZE,
			8,
			EGL_GREEN_SIZE,
			8,
			EGL_RED_SIZE,
			8,
			EGL_DEPTH_SIZE,
			16,
			EGL_NONE};
		eglChooseConfig(m_display, attribs, &m_config, 1, &num_configs);
		m_depthSize = 16;
	}
	
	if (!num_configs) {
		LOGW("Unable to retrieve EGL config");
		return false;
	}

	m_surface = eglCreateWindowSurface(m_display, m_config, m_window, NULL);
	eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_screenWidth);
	eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_screenHeight);

	eglSwapInterval(m_display, 1);
	
	return true;
}

bool Activity::initializeEGLContext() {
	
	LOGW("initializeEGLContext()");
	
	const EGLint context_attribs[] = {
			EGL_CONTEXT_CLIENT_VERSION,
			3, // OpenGL ES 3.0
			EGL_NONE};
	
	m_context = eglCreateContext(m_display, m_config, NULL, context_attribs);
	
	if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return false;
	}
	
	m_contextValid = true;
	return true;
}


/* NEEDS to be somewhere in AE to make the linker happy.
 * If a child class defines their own it will be used instead... */
void android_main(android_app* app) {}




#endif // ANDROID
