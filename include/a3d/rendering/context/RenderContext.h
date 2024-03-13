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

		RenderContext(RenderingApi renderingApi);
		RenderContext(const RenderContext& other) = delete; // copy constructor
		RenderContext& operator=(const RenderContext& other) = delete; // copy assignment
		virtual ~RenderContext();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		unsigned 							width() const;
		unsigned 							height() const;

		unsigned 							framebufferWidth() const;
		unsigned 							framebufferHeight() const;
		glm::vec2 							framebufferScale() const;
		
		virtual bool 						vSyncEnabled() const;
		virtual void 						vSyncEnabled(bool enabled);

		AntialiasingMode 					antialiasingMode() const;

		std::shared_ptr<Image> 				snapshot() const;

		virtual bool 						recordingGIF() const;
		virtual void 						startGIFRecording(const std::filesystem::path& path,
															  unsigned maxHeight,
															  unsigned maxFramerate);
		virtual unsigned 					recordedGIFFrames() const;
		virtual void 						stopGIFRecording();

		VisualWorld*						visualWorld() const;

		std::shared_ptr<Renderer> 			renderer() const;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		virtual void 						swapBuffers() = 0;

		void 								width(unsigned width);
		void 								height(unsigned height);

		void 								framebufferWidth(unsigned width);
		void 								framebufferHeight(unsigned height);
		void 								framebufferScale(glm::vec2 scale);

		virtual void 						saveGIFFrame(float deltaRunT);

		void								attachedToVisualWorld(VisualWorld* world);
		void								detachedFromVisualWorld(VisualWorld* world);
		
/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		unsigned							_width;
		unsigned							_height;
		unsigned							_framebufferWidth;
		unsigned							_framebufferHeight;
		glm::vec2							_framebufferScale;
		bool								_vSyncEnabled;
		AntialiasingMode					_antialiasingMode;
		
		std::shared_ptr<GifWriter>			_gifWriter;
		bool								_recordingGIF;
		unsigned							_gifRecordingWidth;
		unsigned							_gifRecordingHeight;
		unsigned							_gifRecordingMaxFramerate;
		unsigned							_gifRecordedFrames;

		VisualWorld*						_visualWorld;

		std::shared_ptr<Renderer>			_renderer;
	};
}


#endif /* RenderContext_h */
