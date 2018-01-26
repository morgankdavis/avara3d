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


struct FONScontext;
struct GLFWwindow;


namespace ae {


	class Scene;
	class Camera;
	class Color;
	class GIFRecording;
	class Image;
	class InputManager;
	class Node;
	
	
	using WindowUpdateFuction = std::function<void(Scene& scene, float time)>;
	using WindowWillRenderFuction = std::function<void(Scene& scene, float time)>;
	using WindowDidRenderFuction = std::function<void(Scene& scene, float time)>;

	
	class Window {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Window(bool fullScreen, unsigned width, unsigned height,
			   bool useHighDPI = true, AntialiasingMode antialiasingMode = AntialiasingMode_None);
		~Window();
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		void display();

		std::shared_ptr<Scene> scene() const;
		void scene(const std::shared_ptr<Scene> scene);

		bool cursorCaptured() const;
		void captureCursor(bool captured);
		
		bool vSyncEnabled() const;
		void enableVSync(bool enabled);
		
		float maximumFramerate() const;
		void maximumFramerate(float max);
		
		DebugOption debugOptions() const;
		void debugOptions(DebugOption options);
		
		AntialiasingMode antialiasingMode() const;
		
		std::shared_ptr<Node> pointOfView();
		void pointOfView(const std::shared_ptr<Node> camera);
		
		std::shared_ptr<InputManager> inputManager();
		
		std::shared_ptr<Image> snapshot() const;
		
		bool recordingGIF() const;
		void startGIFRecording(std::string filename, unsigned maxHeight, unsigned maxFramerate);
		void stopGIFRecording();
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/

		unsigned width() const;
		/* PROBABLY REMOVE */ void width(unsigned width);
		
		unsigned height() const;
		/* PROBABLY REMOVE */ void height(unsigned height);
		
		unsigned framebufferScale() const;
		/* PROBABLY REMOVE */ void framebufferScale(unsigned aScale);
		
		unsigned framebufferWidth() const;
		/* PROBABLY REMOVE */ void framebufferWidth(unsigned width);
		
		unsigned framebufferHeight() const;
		/* PROBABLY REMOVE */ void framebufferHeight(unsigned height);

		std::shared_ptr<Node> defaultPointOfView();
		
		GLFWwindow* glfwWindow() const;
		
		WindowUpdateFuction updateCallback();
		void updateCallback(WindowUpdateFuction function);
		
		WindowWillRenderFuction willRenderCallback();
		void willRenderCallback(WindowWillRenderFuction function);
		
		WindowDidRenderFuction didRenderCallback();
		void didRenderCallback(WindowDidRenderFuction function);

	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/

		void initFontstash();
		void setupRenderBuffer();
		void mainLoop();
		void updateStatsOverlay(DrawStats& stats);
		float drawText(std::string line, float size, float dx, float dy);
		void saveGIFFrame(float deltaSeconds);
		
		
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
		FONScontext* 					m_fonsContext;
		int								m_fonsFont;
		bool							m_recordingGIF;
		unsigned						m_gifRecordingWidth;
		unsigned						m_gifRecordingHeight;
		unsigned						m_gifRecordingMaxFramerate;
		unsigned						m_renderFramebuffer;
		//unsigned						m_drawFramebuffer;
		bool							m_cursorCaptured;
		
		WindowUpdateFuction				m_updateCallback;
		WindowWillRenderFuction 		m_willRenderCallback;
		WindowDidRenderFuction 			m_didRenderCallback;
	};
}


#endif /* Window_h */
