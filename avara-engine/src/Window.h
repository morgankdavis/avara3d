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

#include <boost/filesystem.hpp>

#include "Renderer.h"
#include "Types.h"


struct FONScontext;
struct GLFWwindow;


namespace ae {

	
	class Camera;
	class Color;
	class Image;
	class InputManager;
	class Node;
	class Scene;

	
	class Window : public Renderer {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		Window(bool fullScreen, unsigned width, unsigned height,
			   bool useHighDPI = true,
			   ANTIALIASING_MODE antialiasingMode = ANTIALIASING_MODE::NONE);
		~Window();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/

		void display();

		bool cursorCaptured() const;
		void captureCursor(bool captured);

		ANTIALIASING_MODE antialiasingMode() const;

		std::shared_ptr<InputManager> inputManager();

		/***************************************************************************************
		     Internal
		 ***************************************************************************************/

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
		
		GLFWwindow* glfwWindow() const;
		
		/**************************************************************************************
		     Renderer
		 ***************************************************************************************/
		
		void enableVSync(bool enabled) override;
		void debugOptions(DEBUG_OPTIONS options) override;
		std::shared_ptr<Node> defaultPointOfView() override;
		std::shared_ptr<Image> snapshot() const override;
		bool recordingGIF() const override;
		void startGIFRecording(const boost::filesystem::path& path,
							   unsigned maxHeight, unsigned maxFramerate) override;
		void stopGIFRecording() override;
		void saveGIFFrame(float deltaSeconds) override;

	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/

		void initFontstash();
		void mainLoop();
		void updateStatsOverlay(DrawStats& stats);
		float drawText(std::string text, float size, float dx, float dy);

		unsigned							m_width;
		unsigned							m_height;
		float								m_framebufferScale;
		unsigned							m_framebufferWidth;
		unsigned							m_framebufferHeight;
		ANTIALIASING_MODE					m_antialiasingMode;
		std::shared_ptr<Color>				m_backgroundColor;
		std::shared_ptr<InputManager> 		m_inputManager;
		FONScontext* 						m_fonsContext;
		int									m_fonsFont;
		bool								m_cursorCaptured;
		
		bool								m_recordingGIF;
		unsigned							m_gifRecordingWidth;
		unsigned							m_gifRecordingHeight;
		unsigned							m_gifRecordingMaxFramerate;
		unsigned							m_gifRecordedFrames;
	};
}


#endif /* Window_h */
