//
//  Activity.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/3/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifdef ANDROID


#include "Activity.h"

#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <errno.h>
#include <string.h>
#include <jni.h>
#include <sys/time.h>
#include <unistd.h>

#include <NDKHelper.h>

#include "ActivityInputManager.h"
#include "Camera.h"
#include "Global.h"
#include "Logger.h"
#include "Node.h"
#include "Renderer.h"
#include "Scene.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

Activity::Activity(RENDER_API renderAPI):
		RenderContext(renderAPI),
		_nativeWindow(nullptr),
		_display(EGL_NO_DISPLAY),
		_surface(EGL_NO_SURFACE),
		_context(EGL_NO_CONTEXT),
		_initialized(false) {

}

Activity::~Activity() {
	AE_LOG_D("Destroying Activity {:p}", (void*)this);
	
	terminate();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

void Activity::display(android_app* app) {
	
	AE_LOG_T("Activity::display()");

	//ndk_helper::JNIHelper::Init(app->activity, "com/mkdinteractive/helper/NDKHelper");

	app->userData = this;
	app->onAppCmd = Activity::appCommandCallback;
	app->onInputEvent = Activity::appInputCallback;

//#ifdef USE_NDK_PROFILER
//	monstartup("libTeapotNativeActivity.so");
//#endif
	
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
		//drawFrame();
		update();
	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void Activity::appCommandCallback(struct android_app* app, int32_t cmd) {
	
	Activity* activity = (Activity*)app->userData;
	
	switch (cmd) {
		case APP_CMD_SAVE_STATE:
			break;
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			//if (activity->window != NULL) {
			activity->initDisplay(app);
			//activity->drawFrame();
			activity->update();
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
			//activity->drawFrame();
			activity->update();
			break;
		case APP_CMD_LOW_MEMORY:
			// Free up GL resources
			//activity->TrimMemory();
			break;
	}
}

int32_t Activity::appInputCallback(android_app* app, AInputEvent* event) {
	
	Activity* activity = (Activity*)app->userData;
	
	if (activity->_inputManager) {
		return static_pointer_cast<ActivityInputManager>(activity->_inputManager)->update(event);
	}
	
	return 0;
}

/*********************************************************************************************
	RenderContext
 *********************************************************************************************/

void Activity::update() {
	RenderContext::update();
	
	if (_initialized) {
		
		AE_LOG_T("-------------------------------------------------------------------------------");
		
		_renderer->beginFrame(*this);
		
//#warning move to saveGIFFrame()
//		float time = sceneTime();
//		static float previousSeconds = time;
//		float deltaSeconds = time - previousSeconds;
//		previousSeconds = time;
		
		if (RenderContext::updateCallback()) RenderContext::updateCallback()(*this, sceneTime());
		
		auto pov = pointOfView();
		float aspectRatio = (float)_framebufferWidth/(float)_framebufferHeight;
		pov->camera()->aspectRatio(aspectRatio);
		
		_renderer->renderStats().cameraPosition = pov->position();
		
		// simulate physics
		//	auto physicsWorld = _scene->physicsWorld();
		//	if (physicsWorld) {
		//		physicsWorld->step();
		//		
		//		if (didSimulate()) {
		//			didSimulate()(*this, sceneTime());
		//		}
		//	}
		
		if (RenderContext::willRenderCallback()) RenderContext::willRenderCallback()(*this, sceneTime());
		
		_scene->update(*RenderContext::renderer(),
					  _framebufferWidth, _framebufferHeight,
					  *pov,
					  _debugOptions, _renderer->renderStats());
		
		_renderer->endFrame(*this);
		
		swap();
		
		//if (_recordingGIF) saveGIFFrame(deltaSeconds);
		
		if (RenderContext::didRenderCallback()) RenderContext::didRenderCallback()(*this, sceneTime());
		
		//glfwPollEvents();
		//if (inputManager()) inputManager()->update();
	}
}

bool Activity::vSyncEnabled() const {
	return true;
}

void Activity::enableVSync(bool enabled) {
	if (!enabled) {
		throw Exception("Disabling V-Sync is not supported on this platform.");
	}
}

void Activity::debugOptions(DEBUG_OPTIONS options) {
	RenderContext::debugOptions(options);
	
	//#warning Refactor this
	//	if (_scene && _scene->physicalWorld()) {
	//		_scene->physicalWorld()->debugOptions(_debugOptions);
	//	}
}

shared_ptr<InputManager> Activity::inputManager() {
	if (_inputManager == nullptr) {
		shared_ptr<Activity> activity = static_pointer_cast<Activity>(shared_from_this());
		auto inputManager = make_shared<ActivityInputManager>(activity);
		_inputManager = static_pointer_cast<InputManager>(inputManager);
	}
	return _inputManager;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

bool Activity::initialize(ANativeWindow* window) {
	
	AE_LOG_D("Activity::initialize()");

	if (!_initialized) {
		_nativeWindow = window;
		initEGLSurface();
		initEGLContext();
		initGLES();
		
		RenderContext::renderer()->initialize(*this);
		
		_framebufferScale = 1.0;
		_framebufferWidth = _width * _framebufferScale;
		_framebufferHeight = _height * _framebufferScale;
		
		_initialized = true;
	}
	return true;
}

void Activity::initGLES() {
	
	AE_LOG_D("Activity::initGLES()");

	if (_initialized) return;
	//
	// init OpenGL ES 3 if available
	//
//	const char* versionStr = (const char*)glGetString(GL_VERSION);
//	//if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
//	if (strstr(versionStr, "OpenGL ES 3.")) {
//		es3_supported_ = true;
//		gl_version_ = 3.0f;
//	} else {
//		gl_version_ = 2.0f;
//	}

	_initialized = true;
}

int Activity::initDisplay(android_app* app) {
	
	AE_LOG_D("Activity::initDisplay()");

	static bool displayInitialized = false;
	
	if (!displayInitialized) {
		
		
		//	if (!initd_resources) {
		initialize(app->window);
		//		LoadResources();
		//		initd_resources_ = true;
		//	}
		//	else if(app->window != gl_context_->GetANativeWindow()) {
		//		// Re-init ANativeWindow.
		//		// On some devices, ANativeWindow is re-created when the app is resumed
		//		//assert(gl_context_->GetANativeWindow());
		//		assert(aNativeWindow());
		//		//UnloadResources();
		//		//gl_context_->Invalidate();
		//		invalidate();
		//		//app_ = app;
		//		//gl_context_->Init(app->window);
		//		init(app->window);
		//		//LoadResources();
		//		//initd_resources_ = true;
		//	}
		//	else {
		//		// init OpenGL ES and EGL
		//		if (EGL_SUCCESS == resume(app->window)) {
		//			//UnloadResources();
		//			//LoadResources();
		//		}
		//		else {
		//			assert(0);
		//		}
		//	}
		
		displayInitialized = true;
	}	
	
	return 0;
}

bool Activity::initEGLSurface() {
	
	AE_LOG_D("Activity::initEGLSurface()");

	_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(_display, 0, 0);
	
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
	_colorSize = 8;
	_depthSize = 24;
	
	EGLint nu_configs;
	eglChooseConfig(_display, attribs, &_config, 1, &nu_configs);
	
	if (!nu_configs) {
		AE_LOG_I("Falling back to 16-bit depth buffer.");

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
		eglChooseConfig(_display, attribs, &_config, 1, &nu_configs);
		_depthSize = 16;
	}
	
	if (!nu_configs) {
		AE_LOG_C("Unable to retrieve EGL config.");
		return false;
	}

	int32_t width;
	int32_t height;
	_surface = eglCreateWindowSurface(_display, _config, _nativeWindow, NULL);
	eglQuerySurface(_display, _surface, EGL_WIDTH, &width);
	eglQuerySurface(_display, _surface, EGL_HEIGHT, &height);
	_width = width;
	_height = height;

	eglSwapInterval(_display, 1);
	
	return true;
}

bool Activity::initEGLContext() {
	
	AE_LOG_D("Activity::initEGLContext()");
	
	const EGLint context_attribs[] = {
			EGL_CONTEXT_CLIENT_VERSION,
			3, // OpenGL ES 3.0
			EGL_NONE};
	
	_context = eglCreateContext(_display, _config, NULL, context_attribs);
	
	if (eglMakeCurrent(_display, _surface, _surface, _context) == EGL_FALSE) {
		AE_LOG_C("Unable to make EGL context current.");
		return false;
	}
	
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	
	AE_LOG_I("Renderer: {}", renderer);
	AE_LOG_I("Version: {}", version);
	
	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	AE_LOG_I("Extensions:\n");
	for (GLint e=0 ; e<numExtensions ; ++e) {
		AE_LOG_I("{}", glGetStringi(GL_EXTENSIONS, e));
	}
	
	_contextValid = true;
	return true;
}

void Activity::suspend() {
	
	AE_LOG_D("Activity::suspend()");
	
	if (_surface != EGL_NO_SURFACE) {
		eglDestroySurface(_display, _surface);
		_surface = EGL_NO_SURFACE;
	}
}

EGLint Activity::resume(ANativeWindow* window) {
	
	AE_LOG_D("Activity::resume()");
	
	if (_initialized == false) {
		initialize(window);
		return EGL_SUCCESS;
	}
	else {
		int32_t originalWidth = _width;
		int32_t originalHeight = _height;
		
		// Create surface
		_nativeWindow = window;
		_surface = eglCreateWindowSurface(_display, _config, _nativeWindow, NULL);
		int32_t width;
		int32_t height;
		eglQuerySurface(_display, _surface, EGL_WIDTH, &width);
		eglQuerySurface(_display, _surface, EGL_HEIGHT, &height);
		_width = width;
		_height = height;
		
		if (_width != originalWidth || _height != originalHeight) {
			// Screen resized
			AE_LOG_D("Screen resized: ({}, {}) -> ({}, {})",
						  originalWidth, originalHeight, _width, _height);
		}
		
		if (eglMakeCurrent(_display, _surface, _surface, _context) == EGL_TRUE)
			return EGL_SUCCESS;
		
		EGLint err = eglGetError();
		if (err == EGL_CONTEXT_LOST) {
			// Recreate context
			AE_LOG_I("Re-creating EGL context...");
			initEGLContext();
		}
		else {
			// Recreate surface
			terminate();
			initEGLSurface();
			initEGLContext();
		}
		
		return err;
	}
}

bool Activity::invalidate() {
	
	AE_LOG_D("Activity::invalidate()");
	
	terminate();
	_initialized = false;
	
	return true;
}

void Activity::terminate() {
	
	AE_LOG_D("Activity::terminate()");
	
	if (_display != EGL_NO_DISPLAY) {
		eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (_context != EGL_NO_CONTEXT) {
			eglDestroyContext(_display, _context);
		}
		
		if (_surface != EGL_NO_SURFACE) {
			eglDestroySurface(_display, _surface);
		}
		eglTerminate(_display);
	}
	
	_display = EGL_NO_DISPLAY;
	_context = EGL_NO_CONTEXT;
	_surface = EGL_NO_SURFACE;
	_nativeWindow = nullptr;
	_contextValid = false;
}

EGLint Activity::swap() {
	bool b = eglSwapBuffers(_display, _surface);
	if (!b) {
		EGLint err = eglGetError();
		if (err == EGL_BAD_SURFACE) {
			// Recreate surface
			initEGLSurface();
			return EGL_SUCCESS;  // Still consider glContext is valid
		}
		else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
			// Context has been lost!!
			_contextValid = false;
			terminate();
			initEGLContext();
		}
		return err;
	}
	return EGL_SUCCESS;
}

/* If we DYNAMICALLY link into Android, this has to be here.
 If we STATICALLY link it cannot be!  For dynamic, mayabe we can try 'extern'? */
//void android_main(android_app* app) {}

#endif // ANDROID
