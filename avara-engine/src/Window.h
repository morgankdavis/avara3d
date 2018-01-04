//
//  Window.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Window_h
#define Window_h


#include <functional>
#include <memory>

#include <GLFW/glfw3.h>

#include "Types.h"


namespace ae {


	class Scene;
	class Camera;
	class Color;
	class Image;
	class InputManager;
	class Node;
	
	
	using windowWillUpdateFuction = std::function<void(Scene& scene, float deltaSeconds)>;
	using windowDidUpdateFuction = std::function<void(Scene& scene, float deltaSeconds)>;
	
	
	class Window {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Window(bool fullScreen, unsigned width, unsigned height, bool useHighDPI);
		~Window();
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		void display();

		std::shared_ptr<Scene> scene() const;
		void scene(const std::shared_ptr<Scene> scene);

		void enableCursor(bool enabled);
		
		bool vSyncEnabled() const;
		void vSyncEnabled(bool enabled);
		
		float maximumFramerate() const;
		void maximumFramerate(float max);
		
		DebugOption debugOptions() const;
		void debugOptions(DebugOption options);
		
		std::shared_ptr<Image> snapshot() const;
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/

		unsigned width() const;
		void width(unsigned width);
		
		unsigned height() const;
		void height(unsigned height);
		
		unsigned framebufferScale() const;
		void framebufferScale(unsigned aScale);
		
		unsigned framebufferWidth() const;
		void framebufferWidth(unsigned width);
		
		unsigned framebufferHeight() const;
		void framebufferHeight(unsigned height);
		
		AntialiasingMode antialiasingMode() const;
		void antialiasingMode(AntialiasingMode mode);

		std::shared_ptr<Node> pointOfView();
		void pointOfView(const std::shared_ptr<Node> camera);
		
		std::shared_ptr<InputManager> inputManager();

		std::shared_ptr<Node> defaultPointOfView();
		
		GLFWwindow* glfwWindow() const;
		
		windowWillUpdateFuction willUpdateCallback();
		void willUpdateCallback(windowWillUpdateFuction function);
		
		windowDidUpdateFuction didUpdateCallback();
		void didUpdateCallback(windowDidUpdateFuction function);

	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/

		void updateFrametime(unsigned int numPolygons);
		void mainLoop(const float deltaSeconds);
		
		std::shared_ptr<Scene>			m_scene;
		unsigned						m_width;
		unsigned						m_height;
		float							m_framebufferScale;
		unsigned						m_framebufferWidth;
		unsigned						m_framebufferHeight;
		AntialiasingMode				m_antialiasingMode;
		DebugOption						m_debugOptions;
		std::shared_ptr<Color>			m_backgroundColor;
		std::shared_ptr<Node>			m_pointOfView;
		std::shared_ptr<InputManager> 	m_inputManager;
		bool							m_vSyncEnabled;
		float 							m_maximumFramerate;
		
		windowWillUpdateFuction 		m_willUpdateCallback;
		windowDidUpdateFuction 			m_didUpdateCallback;
	};
}


#endif /* Window_h */
