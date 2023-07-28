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

#include "RenderContext.h"
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

		Window(bool fullScreen,
			   unsigned width, unsigned height,
			   bool useHighDPI = true,
			   ANTIALIASING_MODE antialiasingMode = ANTIALIASING_MODE::NONE,
			   RENDER_API renderAPI = RENDER_API::OPENGL);

		Window(const Window& other) = delete; // copy constructor
		Window& operator=(const Window& other) = delete; // copy assignment
		
		~Window();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		void 					display();

		bool 					cursorCaptured() const;
		void 					captureCursor(bool captured);
		
		void 					setShouldClose();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		GLFWwindow* 			glfwWindow() const;

/*********************************************************************************************
	RenderContext
 *********************************************************************************************/
		
		//void 					update() override;
		void 					swapBuffers() override;
		void 					pollInput() override;
		void 					enableVSync(bool enabled) override;
		void 					debugOptions(DEBUG_OPTIONS options) override;
		std::shared_ptr<ae::InputManager> 		inputManager() override;
		float 					sceneTime() const override;
		
/*********************************************************************************************
	GLFW Callbacks
 *********************************************************************************************/
		
		static void 			glfwWindowSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight);
		static void				glfwFramebufferSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight);
		static void 			glfwErrorCallback(int error, const char* description);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		GLFWwindow*				_glfwWindow;
		std::shared_ptr<ae::InputManager> 		_inputManager;
		bool					_cursorCaptured;
	};
}

#endif // DESKTOP

#endif /* Window_h */
