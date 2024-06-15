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


//struct GLFWmonitor;
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
//			   unsigned width,
//			   unsigned height,
			   const glm::vec2 size,
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

//		unsigned 				width() const;
//		unsigned 				height() const;
		const glm::uvec2&		size() const;

		bool					highDPIEnabled() const;

		bool 					cursorCaptured() const;
		void 					cursorCaptured(bool captured);

/*********************************************************************************************
	RenderContext Internal Members
 *********************************************************************************************/

		void 					swapBuffers() override;
		bool 					vSyncEnabled() const override; // why is this necessary?
		void 					vSyncEnabled(bool enabled) override;

		glm::uvec2				framebufferSize() const override;
		glm::vec2				framebufferScale() const override;

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

//		void 					width(unsigned width);
//		void 					height(unsigned height);
		void					size(const glm::uvec2& size);

//		void 					framebufferWidth(unsigned width);
//		void 					framebufferHeight(unsigned height);
//		void 					framebufferSizes(const std::vector<glm::vec2>& sizes);
//		void 					framebufferScales(const std::vector<glm::vec2>& scales);

//		const glm::vec2&		framebufferSize() const;
//		void 					framebufferSize(const glm::vec2& size);
//
//		const glm::vec2&		framebufferScale() const;
//		void 					framebufferScale(const glm::vec2& scale);


		void					highDPIEnabled(bool enabled);

//		void 					monitorIndex(const std::vector<glm::vec2>& scales);

//		void					calculateFramebufferSize();

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


		glm::uvec2										_size;
//		unsigned						_width;
//		unsigned						_height;
		//glm::vec2						_framebufferScale;
//		std::vector<glm::vec2>			_framebufferSizes;
//		std::vector<glm::vec2>			_framebufferScales;
//		glm::vec2										_framebufferSize;
//		glm::vec2										_framebufferScale;

		bool											_highDPIEnabled;

//		unsigned						_monitorIndex;
	};
}


#endif /* AVARA3D_WINDOW_H */
