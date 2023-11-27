//
//  Window.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Window_h
#define Window_h

#ifdef DESKTOP


#include <memory>

#include "rendering/context/RenderContext.h"
#include "Types.h"


struct GLFWwindow;


namespace ae {

	
	class Camera;
	class Color;
	class Image;
	class InputManager;
	class Node;
	class Renderer;
	class Scene;

	
	class Window : public RenderContext {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Window(RENDER_API renderAPI,
			   bool fullScreen,
			   unsigned width,
			   unsigned height,
			   bool useHighDPI = true,
			   ANTIALIASING_MODE antialiasingMode = ANTIALIASING_MODE::NONE);

		Window(const Window& other) = delete; // copy constructor
		Window& operator=(const Window& other) = delete; // copy assignment
		
		~Window();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		void 							display();

		bool 							cursorCaptured() const;
		void 							cursorCaptured(bool captured);
		
		void 							setShouldClose();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		bool 							wantsClose() const; // *** temporary? ***
		GLFWwindow* 					glfwWindow() const;

/*********************************************************************************************
	RenderContext
 *********************************************************************************************/

		void 							swapBuffers() override;
		void 							pollInput() override;
		bool 							vSyncEnabled() const override; // why is this necessary?
		void 							vSyncEnabled(bool enabled) override;

/*********************************************************************************************
	GLFW Callbacks
 *********************************************************************************************/

		static void 					glfwWindowSizeCallback(GLFWwindow* glfwWindow,
															  int width,
															  int height);
		static void						glfwFramebufferSizeCallback(GLFWwindow* glfwWindow,
																	   int width,
																	   int height);
		static void 					glfwErrorCallback(int error,
														 const char* description);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		GLFWwindow*						_glfwWindow;
		std::shared_ptr<InputManager> 	_inputManager;
		bool							_cursorCaptured;
	};
}


#endif // DESKTOP

#endif /* Window_h */
