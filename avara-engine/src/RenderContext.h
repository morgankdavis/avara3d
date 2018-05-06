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
	class Renderer;
	class RenderContext;
	class Scene;

	
	using RenderContextUpdateFuction = std::function<void(RenderContext& renderContext, float time)>;
	using RenderContextDidSimulatePhysicsFuction = std::function<void(RenderContext& renderContext, float time)>;
	using RenderContextWillRenderFuction = std::function<void(RenderContext& renderContext, float time)>;
	using RenderContextDidRenderFuction = std::function<void(RenderContext& renderContext, float time)>;

	
	class RenderContext : public std::enable_shared_from_this<RenderContext> {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		RenderContext(std::shared_ptr<Renderer> renderer);
		
		RenderContext(const RenderContext& other) = delete; // copy constructor
		RenderContext& operator=(const RenderContext& other) = delete; // copy assignment
		
		virtual ~RenderContext();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
			
		std::shared_ptr<Renderer> renderer() const;
		void renderer(std::shared_ptr<Renderer> renderer);
		
		std::shared_ptr<Scene> scene() const;
		void scene(const std::shared_ptr<Scene> scene);
		
		unsigned width() const;
		unsigned height() const;
		
		unsigned framebufferScale() const;
		
		unsigned framebufferWidth() const;
		unsigned framebufferHeight() const;
		
		virtual bool vSyncEnabled() const;
		virtual void enableVSync(bool enabled);
		
		virtual DEBUG_OPTIONS debugOptions() const;
		virtual void debugOptions(DEBUG_OPTIONS options);
		
		virtual std::shared_ptr<Node> pointOfView();
		virtual void pointOfView(const std::shared_ptr<Node> camera);
		
		ANTIALIASING_MODE antialiasingMode() const;
		
		virtual std::shared_ptr<InputManager> inputManager();
		
		virtual float sceneTime() const;
		
		std::shared_ptr<Image> snapshot() const;
		
		virtual bool recordingGIF() const;
		virtual void startGIFRecording(const boost::filesystem::path& path,
									   unsigned maxHeight, unsigned maxFramerate);
		virtual unsigned recordedGIFFrames() const;
		virtual void stopGIFRecording();
		
		RenderContextUpdateFuction updateCallback();
		void updateCallback(RenderContextUpdateFuction function);
		
		RenderContextDidSimulatePhysicsFuction didSimulatePhysicsCallback();
		void didSimulatePhysicsCallback(RenderContextDidSimulatePhysicsFuction function);
		
		RenderContextWillRenderFuction willRenderCallback();
		void willRenderCallback(RenderContextWillRenderFuction function);
		
		RenderContextDidRenderFuction didRenderCallback();
		void didRenderCallback(RenderContextDidRenderFuction function);
		
		/**************************************************************************************
		     Internal
		 **************************************************************************************/
		
		void width(unsigned width);
		void height(unsigned height);
		
		void framebufferScale(unsigned scale);
		
		void framebufferWidth(unsigned width);
		void framebufferHeight(unsigned height);
		
	protected:
		
		/**************************************************************************************
		     Protected
		 **************************************************************************************/
		
		virtual std::shared_ptr<Node> defaultPointOfView();
		virtual void saveGIFFrame(float deltaSeconds);

		std::shared_ptr<Renderer>				m_renderer;
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
		
		RenderContextUpdateFuction				m_updateCallback;
		RenderContextDidSimulatePhysicsFuction	m_didSimulatePhysicsCallback;
		RenderContextWillRenderFuction 			m_willRenderCallback;
		RenderContextDidRenderFuction 			m_didRenderCallback;
	};
}


#endif /* RenderContext_h */
