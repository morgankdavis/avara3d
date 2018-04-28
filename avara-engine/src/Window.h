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

#include "RenderContext.h"
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
	
//	struct FONScontext;
//	struct GLFWwindow;

	
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
		~Window();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/

		void display();

		bool cursorCaptured() const;
		void captureCursor(bool captured);

		/***************************************************************************************
		     Internal
		 ***************************************************************************************/

		GLFWwindow* glfwWindow() const;
		
		/**************************************************************************************
		     RenderContext
		 **************************************************************************************/
		
		void enableVSync(bool enabled) override;
		void debugOptions(DEBUG_OPTIONS options) override;
		std::shared_ptr<InputManager> inputManager() override;
		float sceneTime() override;
		
//		void enableVSync(bool enabled) override;
//		void debugOptions(DEBUG_OPTIONS options) override;
//		std::shared_ptr<Node> defaultPointOfView() override;
//		std::shared_ptr<Image> snapshot() const override;
//		bool recordingGIF() const override;
//		void startGIFRecording(const boost::filesystem::path& path,
//							   unsigned maxHeight, unsigned maxFramerate) override;
//		void stopGIFRecording() override;
//		void saveGIFFrame(float deltaSeconds) override;

	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/

		void drawLoop();

		std::shared_ptr<Color>				m_backgroundColor;
		std::shared_ptr<InputManager> 		m_inputManager;
		bool								m_cursorCaptured;
		
//		bool								m_recordingGIF;
//		unsigned							m_gifRecordingWidth;
//		unsigned							m_gifRecordingHeight;
//		unsigned							m_gifRecordingMaxFramerate;
//		unsigned							m_gifRecordedFrames;
	};
}


#endif /* Window_h */
