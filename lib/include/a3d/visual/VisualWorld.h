//
//  VisualWorld.h
//  avara3d
//
//  Created by Morgan Davis on 11/25/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_VISUALWORLD_H
#define AVARA3D_VISUAL_VISUALWORLD_H

#include <functional>
#include <memory>

#include "a3d/scene/Scene.h"
#include "a3d/visual/material/Material.h"

namespace a3d {

	struct FrameStats;

	class Color;
	class FrameStatsHistory;
	class Material;
	class Mesh;
	class Node;
	class PhysicsWorld;
	class Profiler;
	class Renderer;
	class RenderContext;
	class Scene;

	class VisualWorld {

	public:
		/// Public Types ///

		using WillRenderCallback =	std::function<void(VisualWorld& world,
		                                                 double time,
		                                                 double deltaTime)>;
		using DidRenderCallback =	std::function<void(VisualWorld& world,
		                                                double time,
		                                                double deltaTime)>;

		/// Public Lifecycle Functions ///

		VisualWorld() = delete;
		explicit VisualWorld(RenderContext& context);

		VisualWorld(const VisualWorld&) = delete;
		VisualWorld& operator=(const VisualWorld&) = delete;

		VisualWorld(VisualWorld&&) = delete;
		VisualWorld& operator=(VisualWorld&&) = delete;

		virtual ~VisualWorld();

		/// Public Member Functions ///

		const Material::Property&			background();
		void 								background(const Material::Property& background);

		// TODO: make Fog its own class
		float 								fogStartDistance() const;
		void 								fogStartDistance(float distance);
		float 								fogEndDistance() const;
		void 								fogEndDistance(float distance);
		// 0 = constant, alpha respected
		// 1 = linear, alpha ignored
		// >=2 = exponential, alpha ignored
		float 								fogDensityExponent() const;
		void 								fogDensityExponent(float exponent);

		const std::shared_ptr<Color>&		fogColor() const;
		void 								fogColor(const std::shared_ptr<Color>& color);

		std::weak_ptr<Node>&				pointOfView();
		void 								pointOfView(const std::weak_ptr<Node>& cameraNode);

		bool								usesDefaultLighting() const;
		void								usesDefaultLighting(bool enabled);

		bool								autoEnablesDefaultLighting() const;
		void								autoEnablesDefaultLighting(bool enabled);

		RenderContext* 						renderContext() const;

		Scene*								scene() const;

		WillRenderCallback 					willRenderCallback() const;
		void 								willRenderCallback(WillRenderCallback function);

		DidRenderCallback 					didRenderCallback() const;
		void 								didRenderCallback(DidRenderCallback function);

		/// Internal Member Functions ///

		void								attachedToScene(Scene& scene);
		void								detachedFromScene(Scene& scene);

		void								draw(const Scene& scene,
												 const PhysicsWorld* physicalWorld,
												 double runT,
												 double deltaRunT,
												 Scene::DebugOptions debugOptions,
												 FrameStats& stats,
												 Profiler& profiler,
												 const FrameStatsHistory& statsHistory);

		std::shared_ptr<Material>			backgroundMaterial();

	private:
		/// Private Member Functions ///

		void								firstDraw();
		std::shared_ptr<Node>				defaultPOV();

		/// Private Member Variables ///

		Material::Property					_background;
		std::shared_ptr<Material>			_backgroundMaterial;
		float								_fogStartDistance;
		float								_fogEndDistance;
		float								_fogDensityExponent;
		std::shared_ptr<Color>				_fogColor;
		bool								_usesDefaultLighting;
		bool								_autoEnablesDefaultLighting;
		std::weak_ptr<Node>					_pointOfView;
		RenderContext*						_renderContext;
		Scene*								_scene;
		WillRenderCallback 					_willRenderCallback;
		DidRenderCallback 					_didRenderCallback;
	};
}

#endif //AVARA3D_VISUAL_VISUALWORLD_H
