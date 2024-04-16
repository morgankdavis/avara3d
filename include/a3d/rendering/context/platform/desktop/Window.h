//
//  Window.h
//	avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef Window_h
#define Window_h

#ifdef DESKTOP


#include <memory>
#include <optional>
#include <string>

#include "a3d/Types.h"
#include "a3d/rendering/context/RenderContext.h"


struct GLFWwindow;


namespace a3d {


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

		Window(RenderingApi renderAPI,
			   std::string title,
			   int width,
			   int height,
			   bool fullScreen,
			   bool useHighDPI = true,
			   AntialiasingMode antialiasingMode = AntialiasingMode::None);

		Window(const Window& other) = delete; // copy constructor
		Window& operator=(const Window& other) = delete; // copy assignment
		
		~Window() override;
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		void 							open();
		void							close();

		std::string						title() const;
		void							title(const std::string& title);

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
