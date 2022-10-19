//
//  RenderContext.h
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef RenderContext_h
#define RenderContext_h


#include <functional>
#include <memory>

#include <boost/filesystem.hpp>
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
		
	public:
		
/*********************************************************************************************
	Types
 *********************************************************************************************/
		
		using UpdateFunction = std::function<void(RenderContext& renderContext, float time)>;
		using DidSimulatePhysicsFunction = std::function<void(RenderContext& renderContext, float time)>;
		using WillRenderFunction = std::function<void(RenderContext& renderContext, float time)>;
		using DidRenderFunction = std::function<void(RenderContext& renderContext, float time)>;
		
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
		
		RENDER_API renderAPI() const;
		
		std::shared_ptr<Renderer> renderer() const;
		/* REMOVE? */ void renderer(std::shared_ptr<Renderer> renderer);
		
		std::shared_ptr<Scene> scene() const;
		void scene(const std::shared_ptr<Scene> scene);
		
		unsigned width() const;
		unsigned height() const;
		
		float framebufferScale() const;
		
		unsigned framebufferWidth() const;
		unsigned framebufferHeight() const;
		
		virtual bool vSyncEnabled() const;
		virtual void enableVSync(bool enabled);
		
		virtual DEBUG_OPTIONS debugOptions() const;
		virtual void debugOptions(DEBUG_OPTIONS options);
		
		virtual std::shared_ptr<Node> pointOfView();
		virtual void pointOfView(const std::shared_ptr<Node> camera);
		
		ANTIALIASING_MODE antialiasingMode() const;
		
		virtual std::shared_ptr<InputManager> inputManager() = 0;
		
		virtual float sceneTime() const;
		
		std::shared_ptr<Image> snapshot() const;
		
		virtual bool recordingGIF() const;
		virtual void startGIFRecording(const boost::filesystem::path& path,
									   unsigned maxHeight, unsigned maxFramerate);
		virtual unsigned recordedGIFFrames() const;
		virtual void stopGIFRecording();
		
		UpdateFunction updateCallback() const;
		void updateCallback(UpdateFunction function);
		
		DidSimulatePhysicsFunction didSimulatePhysicsCallback() const;
		void didSimulatePhysicsCallback(DidSimulatePhysicsFunction function);
		
		WillRenderFunction willRenderCallback() const;
		void willRenderCallback(WillRenderFunction function);
		
		DidRenderFunction didRenderCallback() const;
		void didRenderCallback(DidRenderFunction function);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		virtual void update();
		virtual void swapBuffers() = 0;
		virtual void pollInput() = 0;
		
		std::shared_ptr<PhysicsSimulator> physicsSimulator() const;
		/* REMOVE? */ void physicsSimulator(std::shared_ptr<PhysicsSimulator> physicsSimulator);
		
		void width(unsigned width);
		void height(unsigned height);
		
		void framebufferScale(float scale);
		
		void framebufferWidth(unsigned width);
		void framebufferHeight(unsigned height);
		
	protected:
		
/*********************************************************************************************
	Protected
 *********************************************************************************************/
		
		virtual std::shared_ptr<Node> defaultPointOfView();
		virtual void saveGIFFrame(float time);

		RENDER_API 								m_renderAPI;
		std::shared_ptr<Renderer>				m_renderer;
		std::shared_ptr<PhysicsSimulator>		m_physicsSimulator;
		std::shared_ptr<Scene>					m_scene;
		unsigned								m_width;
		unsigned								m_height;
		float									m_framebufferScale;
		unsigned								m_framebufferWidth;
		unsigned								m_framebufferHeight;
		bool									m_vSyncEnabled;
		ANTIALIASING_MODE						m_antialiasingMode;
		DEBUG_OPTIONS							m_debugOptions;
		std::shared_ptr<Node>					m_pointOfView;
		
		std::shared_ptr<GifWriter> 				m_gifWriter;
		bool									m_recordingGIF;
		unsigned								m_gifRecordingWidth;
		unsigned								m_gifRecordingHeight;
		unsigned								m_gifRecordingMaxFramerate;
		unsigned								m_gifRecordedFrames;
		
		UpdateFunction							m_updateCallback;
		DidSimulatePhysicsFunction				m_didSimulatePhysicsCallback;
		WillRenderFunction 						m_willRenderCallback;
		DidRenderFunction 						m_didRenderCallback;
	};
}


#endif /* RenderContext_h */
