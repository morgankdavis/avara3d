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

#include "Types.h"


namespace ae {


	class Scene;
	class Camera;
	class Color;
	class InputManager;
	class Node;
	
	
	using windowWillUpdateFuction = std::function<void(Scene& scene, float deltaSeconds)>;
	using windowDidUpdateFuction = std::function<void(Scene& scene, float deltaSeconds)>;
	
	
	class Window {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Window(const unsigned width, const unsigned height, const float framebufferScale);
		~Window();
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		void display();

		std::shared_ptr<Scene> scene() const;
		void scene(const std::shared_ptr<Scene> scene);

		void enableCursor(bool enabled);

		DebugOption& debugOptions();
		void debugOptions(const DebugOption& options);
		
		float maximumFramerate();
		void maximumFramerate(float max);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/

		unsigned width() const;
		void width(const unsigned width);
		
		unsigned height() const;
		void height(const unsigned height);
		
		unsigned framebufferScale() const;
		void framebufferScale(const unsigned aScale);
		
		unsigned framebufferWidth() const;
		void framebufferWidth(const unsigned width);
		
		unsigned framebufferHeight() const;
		void framebufferHeight(const unsigned height);
		
		AntialiasingMode antialiasingMode() const;
		void antialiasingMode(const AntialiasingMode mode);

		std::shared_ptr<Color> backgroundColor() const;
		void backgroundColor(const std::shared_ptr<Color> color);

		std::shared_ptr<Node> pointOfView() const;
		void pointOfView(const std::shared_ptr<Node> camera);
		
		std::shared_ptr<InputManager> inputManager();

		std::shared_ptr<Node> addDefaultPointOfView();
		
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
		float 							m_maximumFramerate;
		
		windowWillUpdateFuction 		m_willUpdateCallback;
		windowDidUpdateFuction 			m_didUpdateCallback;
	};
}


#endif /* Window_h */
