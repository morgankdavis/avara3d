//
//  Window.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_WINDOW_H
#define AVARA3D_WINDOW_H


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
	Public Lifecycle
 *********************************************************************************************/

	public:

		Window(RenderingApi renderingAPI,
			   const std::string& title,
			   const glm::uvec2& size,
			   bool fullScreen,
			   bool enableHighDPI = true,
			   AntialiasingMode antialiasingMode = AntialiasingMode::None);
		Window(const Window& other) = delete; // copy constructor
		Window& operator=(const Window& other) = delete; // copy assignment
		~Window() override;

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

		void 					open();
		void					close();

		std::string				title() const;
		void					title(const std::string& title);

		glm::uvec2				size() const;
		void					size(const glm::uvec2& size);

		glm::uvec2				position() const;
		void					position(const glm::uvec2& pos);

		void					center();

		bool					highDPIEnabled() const;

		bool 					cursorCaptured() const;
		void 					cursorCaptured(bool captured);

/*********************************************************************************************
	RenderContext Public Members
 *********************************************************************************************/

		bool 					vSyncEnabled() const override; // why is this necessary?
		void 					vSyncEnabled(bool enabled) override;

/*********************************************************************************************
	RenderContext Internal Members
 *********************************************************************************************/

		void 					swapBuffers() override;

		glm::uvec2				framebufferSize() const override;
		glm::vec2				framebufferScale() const override;

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

		void 					pollInput();
		GLFWwindow* 			glfwWindow() const;

/*********************************************************************************************
	Internal Static Members
 *********************************************************************************************/

		static void				Destroy(GLFWwindow* window);

/*********************************************************************************************
	Private Types
 *********************************************************************************************/

	private:

		struct DestroyGLFWWindow {
			void operator()(GLFWwindow* window){
				Window::Destroy(window);
			}
		};

/*********************************************************************************************
	Private IVars
 *********************************************************************************************/

		std::unique_ptr<GLFWwindow, DestroyGLFWWindow>	_glfwWindow;
		bool											_cursorCaptured;
		bool											_highDPIEnabled;
	};
}


#endif /* AVARA3D_WINDOW_H */
