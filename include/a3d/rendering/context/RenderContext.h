//
//  RenderContext.h
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDERCONTEXT_H
#define AVARA3D_RENDERCONTEXT_H

#include <filesystem>
#include <functional>
#include <memory>
#include <vector>

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

	public:
		/// Public Member Functions ///

		virtual bool 					vSyncEnabled() const = 0;
		virtual void 					vSyncEnabled(bool enabled) = 0;

		AntialiasingMode 				antialiasingMode() const;

		std::unique_ptr<Image> 			snapshot() const;

		virtual bool 					recordingGIF() const;
		virtual void 					startGIFRecording(const std::filesystem::path& path,
														  math::uvec2 fitInside,
														  unsigned maxFramerate);
		virtual double 					recordedGIFTime() const;
		virtual unsigned 				recordedGIFFrames() const;
		virtual void 					stopGIFRecording();

		VisualWorld*					visualWorld() const;

		Renderer* 						renderer() const;

		/// Internal Lifecycle Functions ///

		explicit RenderContext(RenderingApi renderingApi);
		RenderContext(const RenderContext& other) = delete; // copy constructor
		RenderContext& operator=(const RenderContext& other) = delete; // copy assignment
		virtual ~RenderContext();

		/// Internal Member Functions ///

		virtual void 					beginFrame(const Scene& scene) = 0;
		virtual void 					endFrame(const Scene& scene) = 0;

		virtual void 					swapBuffers() = 0;

		virtual math::uvec2				framebufferSize() const = 0;
		virtual math::vec2				framebufferScale() const = 0;

		virtual void 					saveGIFFrame(double deltaRunT);

		void							attachedToVisualWorld(VisualWorld* world);
		void							detachedFromVisualWorld(VisualWorld* world);

		virtual unsigned				defaultFramebuffer() const = 0;

	protected:
		/// Protected Member Variables ///

		AntialiasingMode				_antialiasingMode;
		std::unique_ptr<GifWriter>		_gifWriter;
		bool							_recordingGIF;
		unsigned						_gifRecordingWidth;
		unsigned						_gifRecordingHeight;
		unsigned						_gifRecordingMaxFramerate;
		double 							_gifRecordingCurrentFrameTimeAccum;
		double 							_gifRecordedTime;
		unsigned						_gifRecordedFrames;
		VisualWorld*					_visualWorld;
		std::unique_ptr<Renderer>		_renderer;
	};
}

#endif /* AVARA3D_RENDERCONTEXT_H */
