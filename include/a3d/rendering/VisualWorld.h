//
//  VisualWorld.h
//  avara3d
//
//  Created by Morgan Davis on 11/25/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUALWORLD_H
#define AVARA3D_VISUALWORLD_H

#include <functional>
#include <memory>

#include "a3d/Types.h"

namespace a3d {

	class Color;
	class FrameStatsHistory;
	class Material;
	class Mesh;
	class Node;
	class PhysicalWorld;
	class Profiler;
	class Renderer;
	class RenderContext;
	class Scene;

	class VisualWorld {

	public:
		/// Public Types ///

		using WillRenderCallback =	std::function<void(VisualWorld& world, double time, double deltaTime)>;
		using DidRenderCallback =	std::function<void(VisualWorld& world, double time, double deltaTime)>;

		/// Public Lifecycle Functions ///

		VisualWorld() = delete;
		explicit VisualWorld(RenderContext& context);
		virtual ~VisualWorld();

		/// Public Member Functions ///

		const MaterialProperty&					background();
		void 									background(const MaterialProperty& background);

		// TODO: make Fog its own class
		float 									fogStartDistance() const;
		void 									fogStartDistance(float distance);
		float 									fogEndDistance() const;
		void 									fogEndDistance(float distance);
		// 0 = constant, alpha respected
		// 1 = linear, alpha ignored
		// >=2 = exponential, alpha ignored
		float 									fogDensityExponent() const;
		void 									fogDensityExponent(float exponent);

		const std::shared_ptr<Color>&			fogColor() const;
		void 									fogColor(const std::shared_ptr<Color>& color);

		std::weak_ptr<Node>&					pointOfView();
		void 									pointOfView(const std::weak_ptr<Node>& cameraNode);

		bool									usesDefaultLighting() const;
		void									usesDefaultLighting(bool enabled);

		bool									autoEnablesDefaultLighting() const;
		void									autoEnablesDefaultLighting(bool enabled);

		RenderContext* 							renderContext() const;

		Scene*									scene() const;

		WillRenderCallback 						willRenderCallback() const;
		void 									willRenderCallback(WillRenderCallback function);

		DidRenderCallback 						didRenderCallback() const;
		void 									didRenderCallback(DidRenderCallback function);

		/// Internal Member Functions ///

		void									attachedToScene(Scene& scene);
		void									detachedFromScene(Scene& scene);

		void									draw(const Scene& scene,
													 const PhysicalWorld* physicalWorld,
													 double runT,
													 double deltaRunT,
													 DebugOptions debugOptions,
													 FrameStats& stats,
													 Profiler& profiler,
													 const FrameStatsHistory& statsHistory);

		Mesh*									skyboxMesh() const; // TODO: move to renderer

	private:
		/// Private Member Functions ///

		void									firstDraw();
		std::shared_ptr<Node>					defaultPOV();

		/// Private Member Variables ///

		MaterialProperty						_background;
		std::unique_ptr<Mesh>					_skyboxMesh;
		float									_fogStartDistance;
		float									_fogEndDistance;
		float									_fogDensityExponent;
		std::shared_ptr<Color>					_fogColor;
		bool									_usesDefaultLighting;
		bool									_autoEnablesDefaultLighting;
		std::weak_ptr<Node>						_pointOfView;
		RenderContext*							_renderContext;
		Scene*									_scene;
		WillRenderCallback 						_willRenderCallback;
		DidRenderCallback 						_didRenderCallback;
	};
}

#endif //AVARA3D_VISUALWORLD_H
