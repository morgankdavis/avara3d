//
//  GLFWWindow.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_GLFWWINDOW_H
#define AVARA3D_GLFWWINDOW_H

#include <memory>
#include <optional>
#include <string>

#include "a3d/Types.h"
#include "a3d/rendering/context/RenderContext.h"

struct GLFWwindow;

namespace a3d {

	class Camera;
	class Color;
	class GLFWInputManager;
	class Image;
	class Node;
	class Renderer;
	class Scene;
	
	class GLFWWindow : public RenderContext {

	public:
		/// Public Lifecycle Functions ///

		GLFWWindow(RenderingApi renderingAPI,
				   const std::string& title,
				   const glm::uvec2& size,
				   bool fullScreen,
				   bool enableHighDPI = true,
				   AntialiasingMode antialiasingMode = AntialiasingMode::None);
		GLFWWindow(const GLFWWindow& other) = delete; // copy constructor
		GLFWWindow& operator=(const GLFWWindow& other) = delete; // copy assignment
		~GLFWWindow() override;

		/// Public Member Functions ///

		void 				open();
		void				close();

		bool 				isOpen() const;

		std::string			title() const;
		void				title(const std::string& title);

		glm::uvec2			size() const;
		void				size(const glm::uvec2& size);

		glm::uvec2			position() const;
		void				position(const glm::uvec2& pos);

		void				center();

		bool 				hidden() const;
		void				hidden(bool hidden);

		bool 				cursorCaptured() const;
		void 				cursorCaptured(bool captured);

		bool				highDPIEnabled() const;

		/// RenderContext Public Member Functions ///

		bool 				vSyncEnabled() const override;
		void 				vSyncEnabled(bool enabled) override;

		/// RenderContext Internal Member Functions ///

		void 				beginFrame(const Scene& scene) override;
		void 				endFrame(const Scene& scene) override;

		void 				swapBuffers() override;

		glm::uvec2			framebufferSize() const override;
		glm::vec2			framebufferScale() const override;

		unsigned 			defaultFramebuffer() const override;

		/// Internal Member Functions ///

		void				inputManager(GLFWInputManager* manager);
		void 				pollInput(); // remove?
		GLFWwindow* 		glfwWindow() const; // remove?

		/// Internal Static Member Functions ///

		static void			Destroy(GLFWwindow* window);

	private:
		/// Private Static Member Functions ///

		static void 				GLFWCursorPositionCallback(GLFWwindow* glfwWindow,
															  double xPos,
															  double yPos);
		static void 				GLFWMouseButtonCallback(GLFWwindow* glfwWindow,
														   int button,
														   int action,
														   int mods);
		static void 				GLFWScrollWheelCallback(GLFWwindow* glfwWindow,
														   double xOffset,
														   double yOffset);
		static void 				GLFWKeyCallback(GLFWwindow* glfwWindow,
												   int key,
												   int scanCode,
												   int action,
												   int mods);
		static GLFWInputManager* 	InputManagerFromGLFWWindow(GLFWwindow* glfwWindow);

		/// Private Member Functions ///

		void registerGLFWCallbacks();
		void unregisterGLFWCallbacks();

		/// Private Types ///

		struct DestroyGLFWWindow {
			void operator()(GLFWwindow* window){
				GLFWWindow::Destroy(window);
			}
		};

		/// Private Member Variables ///

		std::unique_ptr<GLFWwindow,
				DestroyGLFWWindow>	_glfwWindow;
		bool						_vSyncEnabled;
		bool						_cursorCaptured;
		bool						_open;
		bool						_hidden;
		bool						_highDPIEnabled;
		GLFWInputManager*			_inputManager;
	};
}

#endif /* AVARA3D_GLFWWINDOW_H */
