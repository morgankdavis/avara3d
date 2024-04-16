//
//  Activity.h
//	avara3d
//
//  Created by Morgan Davis on 5/3/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef Activity_h
#define Activity_h

#ifdef ANDROID


#include <memory>

#include "a3d/RenderContext.h"
#include "a3d/Types.h"


struct android_app;
struct AInputEvent;
struct ANativeWindow;
typedef void *EGLDisplay;
typedef void *EGLConfig;
typedef void *EGLContext;
typedef void *EGLSurface;


namespace a3d {


	class InputManager;
	class Renderer;

	
	class Activity : public RenderContext {
		
	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Activity(RENDER_API renderAPI = RENDER_API::OPENGL);
		
		Activity(const Activity& other) = delete; // copy constructor
		Activity& operator=(const Activity& other) = delete; // copy assignment
		
		~Activity();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		void display(android_app* app);

/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		static void appCommandCallback(struct android_app* app, int32_t cmd);
		static int32_t appInputCallback(android_app* app, AInputEvent* event);
		
/*********************************************************************************************
	RenderContext
 *********************************************************************************************/
		
		void update() override;
		bool vSyncEnabled() const override;
		void enableVSync(bool enabled) override;
		void debugOptions(DEBUG_OPTIONS options) override;
		std::shared_ptr<InputManager> inputManager() override;

	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

		bool initialize(ANativeWindow* window);
		int initDisplay(struct android_app* app);
		void initGLES();
		bool initEGLSurface();
		bool initEGLContext();
		void suspend();
		int resume(ANativeWindow* window);
		bool invalidate();
		void terminate();
		int swap();

		ANativeWindow* 					_nativeWindow;
		EGLDisplay 						_display;
		EGLSurface 						_surface;
		EGLContext 						_context;
		EGLConfig 						_config;
		int 							_colorSize;
		int 							_depthSize;
		bool 							_initialized;
		bool 							_contextValid;
		std::shared_ptr<InputManager>	_inputManager;
	};
}

#endif // ANDROID

#endif /* Activity_h */
