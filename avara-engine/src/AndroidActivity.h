//
//  AndroidActivity.h
//	avara-engine
//
//  Created by Morgan Davis on 5/3/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef AndroidActivity_h
#define AndroidActivity_h

#ifdef ANDROID


#include <memory>

// GET RID OF THESE
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "RenderContext.h"
#include "Types.h"


namespace ae {

	
//	class Camera;
//	class Color;
//	class Image;
//	class InputManager;
//	class Node;
	class Renderer;
//	class Scene;

	
	class AndroidActivity : public RenderContext {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		AndroidActivity(std::shared_ptr<Renderer> renderer);
		
		AndroidActivity(const AndroidActivity& other) = delete; // copy constructor
		AndroidActivity& operator=(const AndroidActivity& other) = delete; // copy assignment
		
		~AndroidActivity();

		/***************************************************************************************
     		Public
 		***************************************************************************************/

		void display(android_app* app);
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		void drawFrame();

		int initializeDisplay(struct android_app* app);
		
		bool initialize(ANativeWindow* window);
		
		EGLint swap();
		bool invalidate();

		void suspend();
		EGLint resume(ANativeWindow* window);

		ANativeWindow* nativeWindow(void) const;
		int32_t screenWidth() const;
		int32_t screenHeight() const;

		int32_t bufferColorSize() const;
		int32_t bufferDepthSize() const;

		EGLDisplay display() const;
		EGLSurface surface() const;
		
		static void HandleAppCommand(struct android_app* app, int32_t cmd);
		static int32_t HandleAppInput(android_app* app, AInputEvent* event);

		/**************************************************************************************
		     RenderContext
		 **************************************************************************************/
		
//		void enableVSync(bool enabled) override;
		void debugOptions(DEBUG_OPTIONS options) override;
//		std::shared_ptr<InputManager> inputManager() override;
//		float sceneTime() const override;

	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/

		//void drawLoop();

		
		void initializeGLES();
		void terminate();
		bool initializeEGLSurface();
		bool initializeEGLContext();

		ANativeWindow* m_window;
		EGLDisplay m_display;
		EGLSurface m_surface;
		EGLContext m_context;
		EGLConfig m_config;

		int32_t m_screenWidth;
		int32_t m_screenHeight;
		int32_t m_colorSize;
		int32_t m_depthSize;

		bool m_glesInitialized;
		bool m_eglContextInitialized;
		bool m_contextValid;
	};
}

#endif // ANDROID

#endif /* AndroidActivity_h */
