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

//	struct DestroyGLFWWindow;

	class Camera;
	class Color;
	class Image;
	class InputManager;
	class Node;
	class Renderer;
	class Scene;

	
	class Window : public RenderContext {

/*********************************************************************************************
	Internal Static
 *********************************************************************************************/

	public:

		static void	DestroyGLFEWindow(GLFWwindow* window);
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		struct DestroyGLFWWindow {
			void operator()(GLFWwindow* window){
				//glfwDestroyWindow(ptr);
				Window::DestroyGLFEWindow(window);
			}
		};

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

		void 							open();
		void							close();

		bool 							cursorCaptured() const;
		void 							cursorCaptured(bool captured);

/*********************************************************************************************
	RenderContext
 *********************************************************************************************/

		void 							swapBuffers() override;
		bool 							vSyncEnabled() const override; // why is this necessary?
		void 							vSyncEnabled(bool enabled) override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 							pollInput();
		GLFWwindow* 					glfwWindow() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

//		GLFWwindow*						_glfwWindow;
		bool							_cursorCaptured;

		std::unique_ptr<GLFWwindow, DestroyGLFWWindow>		_glfwWindow;
	};
}


#endif // DESKTOP

#endif /* Window_h */
