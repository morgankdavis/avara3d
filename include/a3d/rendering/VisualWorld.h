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

	public:

/*********************************************************************************************
	Public
 *********************************************************************************************/

		using WillRenderCallback = 		std::function<void(VisualWorld& world, double time)>;
		using DidRenderCallback = 		std::function<void(VisualWorld& world, double time)>;

		VisualWorld() = delete;
		explicit VisualWorld(RenderContext* context);
		virtual ~VisualWorld();

		MaterialProperty&						background();
		void 									background(MaterialProperty background);

		float 									fogStartDistance() const;
		void 									fogStartDistance(float distance);
		float 									fogEndDistance() const;
		void 									fogEndDistance(float distance);
		float 									fogDensityExponent() const;
		void 									fogDensityExponent(float exponent);
		// 0 = constant, alpha respected
		// 1 = linear, alpha ignored
		// >=2 = exponential, alpha ignored

		std::shared_ptr<Color> 					fogColor() const;
		void 									fogColor(const std::shared_ptr<Color> & color);

		std::weak_ptr<Node>						pointOfView();
		void 									pointOfView(const std::weak_ptr<Node>& cameraNode);

		bool									automaticallyAddDefaultLighting() const;
		void									automaticallyAddDefaultLighting(bool enabled);

		RenderContext* 							renderContext() const;

		Scene*									scene() const;

		WillRenderCallback 						willRender() const;
		void 									willRender(WillRenderCallback function);

		DidRenderCallback 						didRender() const;
		void 									didRender(DidRenderCallback function);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void									attachedToScene(Scene& scene);
		void									detachedFromScene(Scene& scene);

		void									checkAddDefaultLighting();

		void									draw(const Scene& scene,
													 const PhysicalWorld* physicalWorld,
													 double runT,
													 double deltaRunT,
													 DebugOptions debugOptions,
													 Stats& stats);

		Mesh*									skyboxMesh() const;
		std::weak_ptr<Node>						defaultPointOfView();

	private:

/*********************************************************************************************
	Private
 *********************************************************************************************/

		MaterialProperty						_background;
		std::unique_ptr<Mesh>					_skyboxMesh;
		float									_fogStartDistance;
		float									_fogEndDistance;
		float									_fogDensityExponent;
		std::shared_ptr<Color>					_fogColor;
		bool									_automaticallyAddDefaultLighting;
		std::weak_ptr<Node>						_pointOfView;
		RenderContext*							_renderContext;
		Scene*									_scene;
		WillRenderCallback 						_willRender;
		DidRenderCallback 						_didRender;
	};
}


#endif //AVARA3D_VISUALWORLD_H
