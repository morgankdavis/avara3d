//
//  RenderContext.h
//	avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef RenderContext_h
#define RenderContext_h


#include <filesystem>
#include <functional>
#include <memory>

#include "glm/glm.hpp"

#include "a3d/Types.h"


struct GifWriter;


namespace a3d {
	
	
	class Camera;
	class Image;
	class InputManager;
	class Material;
	class Node;
	class Renderer;
	class RenderContext;
	class Scene;
	class VisualWorld;

	
	class RenderContext {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		explicit RenderContext(RenderingApi renderingApi);
		RenderContext(const RenderContext& other) = delete; // copy constructor
		RenderContext& operator=(const RenderContext& other) = delete; // copy assignment
		virtual ~RenderContext();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		int 								width() const;
		int 								height() const;

		int 								framebufferWidth() const;
		int 								framebufferHeight() const;
		glm::vec2 							framebufferScale() const;
		
		virtual bool 						vSyncEnabled() const;
		virtual void 						vSyncEnabled(bool enabled);

		AntialiasingMode 					antialiasingMode() const;

		std::unique_ptr<Image> 				snapshot() const;

		virtual bool 						recordingGIF() const;
		virtual void 						startGIFRecording(const std::filesystem::path& path,
															  int maxHeight,
															  int maxFramerate);
		virtual unsigned 					recordedGIFFrames() const;
		virtual void 						stopGIFRecording();

		VisualWorld*						visualWorld() const;

		Renderer* 							renderer() const;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		virtual void 						swapBuffers() = 0;

		void 								width(int width);
		void 								height(int height);

		void 								framebufferWidth(int width);
		void 								framebufferHeight(int height);
		void 								framebufferScale(glm::vec2& scale);

		virtual void 						saveGIFFrame(float deltaRunT);

		void								attachedToVisualWorld(VisualWorld* world);
		void								detachedFromVisualWorld(VisualWorld* world);
		
/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		int									_width;
		int									_height;
		int									_framebufferWidth;
		int									_framebufferHeight;
		glm::vec2							_framebufferScale;
		bool								_vSyncEnabled;
		AntialiasingMode					_antialiasingMode;
		
		std::unique_ptr<GifWriter>			_gifWriter;
		bool								_recordingGIF;
		int									_gifRecordingWidth;
		int									_gifRecordingHeight;
		int									_gifRecordingMaxFramerate;
		int									_gifRecordedFrames;

		VisualWorld*						_visualWorld;

		std::unique_ptr<Renderer>			_renderer;
	};
}


#endif /* RenderContext_h */
