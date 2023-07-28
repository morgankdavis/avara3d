//
//  RenderContext.h
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef RenderContext_h
#define RenderContext_h


#include <filesystem>
#include <functional>
#include <memory>

//#include <boost/filesystem.hpp>
#include <glm/glm.hpp>

#include "Types.h"


struct GifWriter;


namespace ae {
	
	
	class Camera;
	class Image;
	class InputManager;
	class Material;
	class Node;
	class PhysicsSimulator;
	class Renderer;
	class RenderContext;
	class Scene;

	
	class RenderContext : public std::enable_shared_from_this<RenderContext> {
		
/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		using UpdateFunction = 				std::function<void(RenderContext& renderContext, float time)>;
		using DidSimulatePhysicsFunction = 	std::function<void(RenderContext& renderContext, float time)>;
		using WillRenderFunction = 			std::function<void(RenderContext& renderContext, float time)>;
		using DidRenderFunction = 			std::function<void(RenderContext& renderContext, float time)>;
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		RenderContext(RENDER_API renderAPI);
		
		RenderContext(const RenderContext& other) = delete; // copy constructor
		RenderContext& operator=(const RenderContext& other) = delete; // copy assignment
		
		virtual ~RenderContext();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		RENDER_API 									renderAPI() const;
		
		std::shared_ptr<ae::Renderer> 				renderer() const;
		/* REMOVE? */ void 							renderer(std::shared_ptr<ae::Renderer> renderer);
		
		std::shared_ptr<ae::Scene> 					scene() const;
		void 										scene(const std::shared_ptr<ae::Scene> scene);
		
		unsigned 									width() const;
		unsigned 									height() const;
		
		float 										framebufferScale() const;
		
		unsigned 									framebufferWidth() const;
		unsigned 									framebufferHeight() const;
		
		virtual bool 								vSyncEnabled() const;
		virtual void 								enableVSync(bool enabled);
		
		virtual DEBUG_OPTIONS 						debugOptions() const;
		virtual void 								debugOptions(DEBUG_OPTIONS options);
		
		virtual std::shared_ptr<ae::Node>			pointOfView();
		virtual void 								pointOfView(const std::shared_ptr<ae::Node> camera);
		
		ANTIALIASING_MODE 							antialiasingMode() const;
		
		virtual std::shared_ptr<ae::InputManager> 	inputManager() = 0;
		
		virtual float 								sceneTime() const;
		
		std::shared_ptr<ae::Image> 					snapshot() const;
		
		virtual bool 								recordingGIF() const;
		virtual void 								startGIFRecording(const std::filesystem::path& path,
																	  unsigned maxHeight, unsigned maxFramerate);
		virtual unsigned 							recordedGIFFrames() const;
		virtual void 								stopGIFRecording();
		
		UpdateFunction 								updateCallback() const;
		void 										updateCallback(UpdateFunction function);
		
		DidSimulatePhysicsFunction 					didSimulatePhysicsCallback() const;
		void 										didSimulatePhysicsCallback(DidSimulatePhysicsFunction function);
		
		WillRenderFunction 							willRenderCallback() const;
		void 										willRenderCallback(WillRenderFunction function);
		
		DidRenderFunction 							didRenderCallback() const;
		void 										didRenderCallback(DidRenderFunction function);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		virtual void 								update();
		virtual void 								swapBuffers() = 0;
		virtual void 								pollInput() = 0;
		
		std::shared_ptr<ae::PhysicsSimulator> 		physicsSimulator() const;
		/* REMOVE? */ void 							physicsSimulator(std::shared_ptr<ae::PhysicsSimulator> physicsSimulator);
		
		void 										width(unsigned width);
		void 										height(unsigned height);
		
		void 										framebufferScale(float scale);
		
		void 										framebufferWidth(unsigned width);
		void 										framebufferHeight(unsigned height);
		
/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		virtual std::shared_ptr<ae::Node> 			defaultPointOfView();
		virtual void 								saveGIFFrame(float time);

		RENDER_API 									_renderAPI;
		std::shared_ptr<ae::Renderer>				_renderer;
		std::shared_ptr<ae::PhysicsSimulator>		_physicsSimulator;
		std::shared_ptr<ae::Scene>					_scene;
		unsigned									_width;
		unsigned									_height;
		float										_framebufferScale;
		unsigned									_framebufferWidth;
		unsigned									_framebufferHeight;
		bool										_vSyncEnabled;
		ANTIALIASING_MODE							_antialiasingMode;
		DEBUG_OPTIONS								_debugOptions;
		std::shared_ptr<ae::Node>					_pointOfView;
		
		std::shared_ptr<GifWriter>					_gifWriter;
		bool										_recordingGIF;
		unsigned									_gifRecordingWidth;
		unsigned									_gifRecordingHeight;
		unsigned									_gifRecordingMaxFramerate;
		unsigned									_gifRecordedFrames;
		
		UpdateFunction								_updateCallback;
		DidSimulatePhysicsFunction					_didSimulatePhysicsCallback;
		WillRenderFunction 							_willRenderCallback;
		DidRenderFunction 							_didRenderCallback;
	};
}


#endif /* RenderContext_h */
