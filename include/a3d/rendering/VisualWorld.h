//
// Created by mkd on 11/25/23.
//

#ifndef AVARA3D_VISUALWORLD_H
#define AVARA3D_VISUALWORLD_H


#include <functional>
#include <memory>

#include "a3d/Types.h"
#include "a3d/rendering/material/Material.h"


namespace a3d {


	class Color;
	class Mesh;
	class Node;
	class PhysicalWorld;
	class RenderContext;
	class Renderer;
	class Scene;


	class VisualWorld {

/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		using WillRenderCallback = 		std::function<void(VisualWorld& world, double time)>;
		using DidRenderCallback = 		std::function<void(VisualWorld& world, double time)>;

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		VisualWorld(std::shared_ptr<RenderContext> context);
		VisualWorld(const VisualWorld& other) = delete; // copy constructor
		VisualWorld& operator=(const VisualWorld& other) = delete; // copy assignment
		virtual ~VisualWorld();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		Material::Property						background() const;
		void 									background(Material::Property background);

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
		void 									fogColor(std::shared_ptr<Color> color);

		std::shared_ptr<Node>					pointOfView();
		void 									pointOfView(const std::shared_ptr<Node> cameraNode);

		bool									automaticallyAddDefaultLighting() const;
		void									automaticallyAddDefaultLighting(bool enabled);

		std::shared_ptr<RenderContext> 			renderContext() const;

		Scene*									scene() const;

		WillRenderCallback 						willRender() const;
		void 									willRender(WillRenderCallback function);

		DidRenderCallback 						didRender() const;
		void 									didRender(DidRenderCallback function);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void									attachedToScene(Scene* scene);
		void									detachedFromScene(Scene* scene);

		void									checkAddDefaultLighting();

		void									draw(const Scene& scene,
													 const PhysicalWorld* physicalWorld,
													 double runT,
													 double deltaRunT,
													 DebugOptions debugOptions,
													 Stats& stats);

		std::shared_ptr<Mesh>					skyboxMesh() const;
		std::shared_ptr<Node> 					defaultPointOfView();

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		Material::Property						_background;
		std::shared_ptr<Mesh>					_skyboxMesh;
		float									_fogStartDistance;
		float									_fogEndDistance;
		float									_fogDensityExponent;
		std::shared_ptr<Color>					_fogColor;
		bool									_automaticallyAddDefaultLighting;
		std::shared_ptr<Node>					_pointOfView;
		std::shared_ptr<RenderContext>			_renderContext;
		Scene*									_scene;
		WillRenderCallback 						_willRender;
		DidRenderCallback 						_didRender;
	};
}


#endif //AVARA3D_VISUALWORLD_H
