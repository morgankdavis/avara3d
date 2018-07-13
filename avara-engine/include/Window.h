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
	class Scene;

	
	class Window : public RenderContext {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		Window(std::shared_ptr<Renderer> renderer,
			   bool fullScreen,
			   unsigned width, unsigned height,
			   bool useHighDPI = true,
			   ANTIALIASING_MODE antialiasingMode = ANTIALIASING_MODE::NONE);
		
		Window(const Window& other) = delete; // copy constructor
		Window& operator=(const Window& other) = delete; // copy assignment
		
		~Window();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/

		void display();

		bool cursorCaptured() const;
		void captureCursor(bool captured);
		
		void setShouldClose();

		/***************************************************************************************
		     Internal
		 ***************************************************************************************/

		GLFWwindow* glfwWindow() const;

		/**************************************************************************************
		     RenderContext
		 **************************************************************************************/
		
		void update() override;
		void enableVSync(bool enabled) override;
		void debugOptions(DEBUG_OPTIONS options) override;
		std::shared_ptr<InputManager> inputManager() override;;
		float sceneTime() const override;
		
		/***************************************************************************************
		     GLFW Callbacks
		 ***************************************************************************************/
		
		static void glfwWindowSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight);
		static void glfwFramebufferSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight);
		static void glfwErrorCallback(int error, const char* description);

	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/

		GLFWwindow*							m_glfwWindow;
		std::shared_ptr<InputManager> 		m_inputManager;
		bool								m_cursorCaptured;
	};
}

#endif // DESKTOP

#endif /* Window_h */
