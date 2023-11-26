//
// Created by mkd on 11/25/23.
//

#ifndef AVARA_ENGINE_VISUALWORLD_H
#define AVARA_ENGINE_VISUALWORLD_H


#include <functional>
#include <memory>

#include "Types.h"


namespace ae {


	class Color;
	class Geometry;
	class MaterialProperty;
	class Node;
	class RenderContext;
	class Renderer;
	class Scene;


	class VisualWorld {

/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		using WillRenderCallback = 		std::function<void(VisualWorld& world, float time)>;
		using DidRenderCallback = 		std::function<void(VisualWorld& world, float time)>;

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

		std::shared_ptr<MaterialProperty>		background() const;
		void 									background(std::shared_ptr<MaterialProperty> background);

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
		void 									pointOfView(const std::shared_ptr<Node> camera);

		std::shared_ptr<RenderContext> 			renderContext() const;
		void 									renderContext(std::shared_ptr<RenderContext> context);

//		RENDER_API								renderAPI() const;

//		std::weak_ptr<Scene>					scene() const;
//		void									scene(std::weak_ptr<Scene> scene);

		WillRenderCallback 						willRender() const;
		void 									willRender(WillRenderCallback function);

		DidRenderCallback 						didRender() const;
		void 									didRender(DidRenderCallback function);

		std::weak_ptr<Scene>					scene() const;
		void									scene(std::weak_ptr<Scene> scene);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		std::shared_ptr<Geometry>				skyboxGeometry() const;
		std::shared_ptr<Node> 					defaultPointOfView();

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<MaterialProperty>		_background;
		std::shared_ptr<Geometry>				_skyboxGeometry;
		float									_fogStartDistance;
		float									_fogEndDistance;
		float									_fogDensityExponent;
		std::shared_ptr<Color>					_fogColor;

		std::shared_ptr<Node>					_pointOfView;

		std::shared_ptr<RenderContext>			_renderContext;
//		RENDER_API								_renderAPI;

		WillRenderCallback 						_willRender;
		DidRenderCallback 						_didRender;

		std::weak_ptr<Scene>					_scene;
	};
}


#endif //AVARA_ENGINE_VISUALWORLD_H
