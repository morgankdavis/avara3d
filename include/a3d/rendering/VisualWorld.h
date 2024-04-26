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
	class Material;
	class Mesh;
	class Node;
	class PhysicalWorld;
	class RenderContext;
	class Renderer;
	class Scene;


	class VisualWorld {

/*********************************************************************************************
	Public Types
 *********************************************************************************************/

	public:

		using WillRenderCallback = 		std::function<void(VisualWorld& world, float time, float deltaTime)>;
		using DidRenderCallback = 		std::function<void(VisualWorld& world, float time, float deltaTime)>;

/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

		VisualWorld() = delete;
		explicit VisualWorld(RenderContext& context);
		virtual ~VisualWorld();

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

		const MaterialProperty&					background();
		void 									background(const MaterialProperty& background);

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
		void 									fogColor(const std::shared_ptr<Color> & color);

		std::weak_ptr<Node>						pointOfView();
		void 									pointOfView(const std::weak_ptr<Node>& cameraNode);

		bool									autoEnablesDefaultLighting() const;
		void									autoEnablesDefaultLighting(bool enabled);

		bool									usesDefaultLighting() const;
		void									usesDefaultLighting(bool enabled);

		RenderContext* 							renderContext() const;

		Scene*									scene() const;

		WillRenderCallback 						willRender() const;
		void 									willRender(WillRenderCallback function);

		DidRenderCallback 						didRender() const;
		void 									didRender(DidRenderCallback function);

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

		void									attachedToScene(Scene& scene);
		void									detachedFromScene(Scene& scene);

		void									draw(const Scene& scene,
													 const PhysicalWorld* physicalWorld,
													 double runT,
													 double deltaRunT,
													 DebugOptions debugOptions,
													 Stats& stats);

		Mesh*									skyboxMesh() const;
		std::weak_ptr<Node>						defaultPointOfView();

/*********************************************************************************************
	Private IVars
 *********************************************************************************************/

	private:

		MaterialProperty						_background;
		std::unique_ptr<Mesh>					_skyboxMesh;
		float									_fogStartDistance;
		float									_fogEndDistance;
		float									_fogDensityExponent;
		std::shared_ptr<Color>					_fogColor;
		bool									_autoEnablesDefaultLighting;
		bool									_usesDefaultLighting;
		std::weak_ptr<Node>						_pointOfView;
		RenderContext*							_renderContext;
		Scene*									_scene;
		WillRenderCallback 						_willRender;
		DidRenderCallback 						_didRender;
	};
}


#endif //AVARA3D_VISUALWORLD_H
