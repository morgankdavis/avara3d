//
//  Activity.h
//	avara-engine
//
//  Created by Morgan Davis on 5/3/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Activity_h
#define Activity_h

#ifdef ANDROID


#include <memory>

#include "RenderContext.h"
#include "Types.h"


struct android_app;
struct AInputEvent;
struct ANativeWindow;
typedef void *EGLDisplay;
typedef void *EGLConfig;
typedef void *EGLContext;
typedef void *EGLSurface;


namespace ae {


	class InputManager;
	class Renderer;

	
	class Activity : public RenderContext {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		Activity(std::shared_ptr<Renderer> renderer);
		
		Activity(const Activity& other) = delete; // copy constructor
		Activity& operator=(const Activity& other) = delete; // copy assignment
		
		~Activity();

		/***************************************************************************************
     		Public
 		***************************************************************************************/

		void display(android_app* app);

		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		static void appCommandCallback(struct android_app* app, int32_t cmd);
		static int32_t appInputCallback(android_app* app, AInputEvent* event);
		
		/**************************************************************************************
		     RenderContext
		 **************************************************************************************/
		
		bool vSyncEnabled() const override;
		void enableVSync(bool enabled) override;
		void debugOptions(DEBUG_OPTIONS options) override;
		std::shared_ptr<InputManager> inputManager() override;

	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/

		bool initialize(ANativeWindow* window);
		int initDisplay(struct android_app* app);
		void initGLES();
		bool initEGLSurface();
		bool initEGLContext();
		void suspend();
		int resume(ANativeWindow* window);
		bool invalidate();
		void terminate();
		void drawFrame();
		int swap();

		ANativeWindow* 					m_nativeWindow;
		EGLDisplay 						m_display;
		EGLSurface 						m_surface;
		EGLContext 						m_context;
		EGLConfig 						m_config;
		int 							m_colorSize;
		int 							m_depthSize;
		bool 							m_initialized;
		bool 							m_contextValid;
		std::shared_ptr<InputManager>	m_inputManager;
	};
}

#endif // ANDROID

#endif /* Activity_h */
