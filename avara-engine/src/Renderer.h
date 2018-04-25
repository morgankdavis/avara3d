//
//  Renderer.h
//	avara-engine
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Renderer_h
#define Renderer_h


#include <functional>
#include <memory>

#include <boost/filesystem.hpp>
#include <glm/glm.hpp>

#include "Types.h"


namespace ae {

	
	class Camera;
	class Geometry;
	class GeometryElement;
	class Image;
	class Material;
	class Node;
	class Renderer;
	class Scene;
	
	
	using RendererUpdateFuction = std::function<void(Renderer& renderer, float time)>;
	using RendererDidSimulatePhysicsFuction = std::function<void(Renderer& renderer, float time)>;
	using RendererWillRenderFuction = std::function<void(Renderer& renderer, float time)>;
	using RendererDidRenderFuction = std::function<void(Renderer& renderer, float time)>;

	
	class Renderer : public std::enable_shared_from_this<Renderer> {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Renderer();
		virtual ~Renderer();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
			
		virtual std::shared_ptr<Scene> scene() const;
		virtual void scene(const std::shared_ptr<Scene> scene);
		
		virtual bool vSyncEnabled() const;
		virtual void enableVSync(bool enabled);
		
		virtual float maximumFramerate() const;
		virtual void maximumFramerate(float max);
		
		virtual DEBUG_OPTIONS debugOptions() const;
		virtual void debugOptions(DEBUG_OPTIONS options);
		
		virtual std::shared_ptr<Node> pointOfView();
		virtual void pointOfView(const std::shared_ptr<Node> camera);
		
		virtual std::shared_ptr<Image> snapshot() const;
		
		virtual bool recordingGIF() const;
		virtual void startGIFRecording(const boost::filesystem::path& path,
							   unsigned maxHeight, unsigned maxFramerate);
		virtual void stopGIFRecording();
		
		RendererUpdateFuction updateCallback();
		void updateCallback(RendererUpdateFuction function);
		
		RendererDidSimulatePhysicsFuction didSimulatePhysicsCallback();
		void didSimulatePhysicsCallback(RendererDidSimulatePhysicsFuction function);
		
		RendererWillRenderFuction willRenderCallback();
		void willRenderCallback(RendererWillRenderFuction function);
		
		RendererDidRenderFuction didRenderCallback();
		void didRenderCallback(RendererDidRenderFuction function);
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/

		void render(const Scene& scene);
//		void render(const Geometry& geometry);
		void render(const GeometryElement& geometryElement,
					const Material& material,
					const glm::mat4& modelMat,
					const glm::mat4& viewMat,
					const glm::mat4& projectionMat);
		
		virtual std::shared_ptr<Node> defaultPointOfView();
		virtual void saveGIFFrame(float deltaSeconds);
		
		RenderStats& renderStats();
		
	protected:
		
		/**************************************************************************************
		     Protected
		 **************************************************************************************/

		std::shared_ptr<Scene>				m_scene;
		DEBUG_OPTIONS						m_debugOptions;
		std::shared_ptr<Node>				m_pointOfView;
		bool								m_vSyncEnabled;
		float 								m_maximumFramerate;

		RendererUpdateFuction				m_updateCallback;
		RendererDidSimulatePhysicsFuction	m_didSimulatePhysicsCallback;
		RendererWillRenderFuction 			m_willRenderCallback;
		RendererDidRenderFuction 			m_didRenderCallback;
		
	private:
		
		/**************************************************************************************
		     Private
		 **************************************************************************************/
		
		void bindEnvironment(const Scene& scene, RenderStats& stats) const;
		
		RenderStats							m_renderStats;
		int									m_glEnvironmentUBO;
	};
}


#endif /* Renderer_h */
