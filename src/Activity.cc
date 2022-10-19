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
		m_nativeWindow(nullptr),
		m_display(EGL_NO_DISPLAY),
		m_surface(EGL_NO_SURFACE),
		m_context(EGL_NO_CONTEXT),
		m_initialized(false) {

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
	
	if (activity->m_inputManager) {
		return static_pointer_cast<ActivityInputManager>(activity->m_inputManager)->update(event);
	}
	
	return 0;
}

/*********************************************************************************************
	RenderContext
 *********************************************************************************************/

void Activity::update() {
	RenderContext::update();
	
	if (m_initialized) {
		
		AE_LOG_T("-------------------------------------------------------------------------------");
		
		m_renderer->beginFrame(*this);
		
//#warning move to saveGIFFrame()
//		float time = sceneTime();
//		static float previousSeconds = time;
//		float deltaSeconds = time - previousSeconds;
//		previousSeconds = time;
		
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
		
		swap();
		
		//if (m_recordingGIF) saveGIFFrame(deltaSeconds);
		
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
	//	if (m_scene && m_scene->physicsWorld()) {
	//		m_scene->physicsWorld()->debugOptions(m_debugOptions);
	//	}
}

shared_ptr<InputManager> Activity::inputManager() {
	if (m_inputManager == nullptr) {
		shared_ptr<Activity> activity = static_pointer_cast<Activity>(shared_from_this());
		auto inputManager = make_shared<ActivityInputManager>(activity);
		m_inputManager = static_pointer_cast<InputManager>(inputManager);
	}
	return m_inputManager;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

bool Activity::initialize(ANativeWindow* window) {
	
	AE_LOG_D("Activity::initialize()");

	if (!m_initialized) {
		m_nativeWindow = window;
		initEGLSurface();
		initEGLContext();
		initGLES();
		
		RenderContext::renderer()->initialize(*this);
		
		m_framebufferScale = 1.0;
		m_framebufferWidth = m_width * m_framebufferScale;
		m_framebufferHeight = m_height * m_framebufferScale;
		
		m_initialized = true;
	}
	return true;
}

void Activity::initGLES() {
	
	AE_LOG_D("Activity::initGLES()");

	if (m_initialized) return;
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

	m_initialized = true;
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

	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(m_display, 0, 0);
	
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
	m_colorSize = 8;
	m_depthSize = 24;
	
	EGLint num_configs;
	eglChooseConfig(m_display, attribs, &m_config, 1, &num_configs);
	
	if (!num_configs) {
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
		eglChooseConfig(m_display, attribs, &m_config, 1, &num_configs);
		m_depthSize = 16;
	}
	
	if (!num_configs) {
		AE_LOG_C("Unable to retrieve EGL config.");
		return false;
	}

	int32_t width;
	int32_t height;
	m_surface = eglCreateWindowSurface(m_display, m_config, m_nativeWindow, NULL);
	eglQuerySurface(m_display, m_surface, EGL_WIDTH, &width);
	eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &height);
	m_width = width;
	m_height = height;

	eglSwapInterval(m_display, 1);
	
	return true;
}

bool Activity::initEGLContext() {
	
	AE_LOG_D("Activity::initEGLContext()");
	
	const EGLint context_attribs[] = {
			EGL_CONTEXT_CLIENT_VERSION,
			3, // OpenGL ES 3.0
			EGL_NONE};
	
	m_context = eglCreateContext(m_display, m_config, NULL, context_attribs);
	
	if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE) {
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
	
	m_contextValid = true;
	return true;
}

void Activity::suspend() {
	
	AE_LOG_D("Activity::suspend()");
	
	if (m_surface != EGL_NO_SURFACE) {
		eglDestroySurface(m_display, m_surface);
		m_surface = EGL_NO_SURFACE;
	}
}

EGLint Activity::resume(ANativeWindow* window) {
	
	AE_LOG_D("Activity::resume()");
	
	if (m_initialized == false) {
		initialize(window);
		return EGL_SUCCESS;
	}
	else {
		int32_t originalWidth = m_width;
		int32_t originalHeight = m_height;
		
		// Create surface
		m_nativeWindow = window;
		m_surface = eglCreateWindowSurface(m_display, m_config, m_nativeWindow, NULL);
		int32_t width;
		int32_t height;
		eglQuerySurface(m_display, m_surface, EGL_WIDTH, &width);
		eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &height);
		m_width = width;
		m_height = height;
		
		if (m_width != originalWidth || m_height != originalHeight) {
			// Screen resized
			AE_LOG_D("Screen resized: ({}, {}) -> ({}, {})",
						  originalWidth, originalHeight, m_width, m_height);
		}
		
		if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_TRUE)
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
	m_initialized = false;
	
	return true;
}

void Activity::terminate() {
	
	AE_LOG_D("Activity::terminate()");
	
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
	m_nativeWindow = nullptr;
	m_contextValid = false;
}

EGLint Activity::swap() {
	bool b = eglSwapBuffers(m_display, m_surface);
	if (!b) {
		EGLint err = eglGetError();
		if (err == EGL_BAD_SURFACE) {
			// Recreate surface
			initEGLSurface();
			return EGL_SUCCESS;  // Still consider glContext is valid
		}
		else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
			// Context has been lost!!
			m_contextValid = false;
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
