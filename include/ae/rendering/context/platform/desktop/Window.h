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

#include "ae/rendering/context/RenderContext.h"
#include "ae/Types.h"


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
	Types
 *********************************************************************************************/

		struct DestroyGLFWWindow {
			void operator()(GLFWwindow* window){
				Window::Destroy(window);
			}
		};

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Window(RenderApi renderAPI,
			   bool fullScreen,
			   unsigned width,
			   unsigned height,
			   bool useHighDPI = true,
			   AntialiasingMode antialiasingMode = AntialiasingMode::None);

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
	Internal Static
 *********************************************************************************************/

		static void						Destroy(GLFWwindow* window);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::unique_ptr<GLFWwindow, DestroyGLFWWindow>	_glfwWindow;
		bool											_cursorCaptured;
	};
}


#endif // DESKTOP

#endif /* Window_h */
