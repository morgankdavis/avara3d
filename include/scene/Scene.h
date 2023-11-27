//
//  Scene.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Scene_h
#define Scene_h


#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Types.h"


namespace ae {


	class Color;
	class Geometry;
	class InputManager;
	class MaterialProperty;
	class Node;
	class PhysicsSimulator;
	class PhysicalWorld;
	class Renderer;
	class RenderContext;
	class VisualWorld;

	
	class Scene {

/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		using UpdateCallback =					std::function<void(Scene& scene, float time)>;

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

#ifndef ANDROID
		static std::shared_ptr<Scene> 			FromFile(const std::filesystem::path &path);
#endif
//		static std::shared_ptr<Scene> LoadFromData(const std::vector<unsigned char>& data);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Scene();
		Scene(std::shared_ptr<VisualWorld> visualWorld,
			  std::shared_ptr<PhysicalWorld> physicsWorld,
			  std::shared_ptr<InputManager> inputManager);
		~Scene();


/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::shared_ptr<Node> 					rootNode() const;
		void 									rootNode(std::shared_ptr<Node> node);
		
//		std::shared_ptr<MaterialProperty>		background() const;
//		void 									background(std::shared_ptr<MaterialProperty> background);
//
//		float 									fogStartDistance() const;
//		void 									fogStartDistance(float distance);
//		float 									fogEndDistance() const;
//		void 									fogEndDistance(float distance);
//		float 									fogDensityExponent() const;
//		void 									fogDensityExponent(float exponent);
//													// 0 = constant, alpha respected
//													// 1 = linear, alpha ignored
//													// >=2 = exponential, alpha ignored
//		std::shared_ptr<Color> 					fogColor() const;
//		void 									fogColor(std::shared_ptr<Color> color);

		std::shared_ptr<VisualWorld> 			visualWorld() const;
		void 									visualWorld(std::shared_ptr<VisualWorld> world);
		
		std::shared_ptr<PhysicalWorld> 			physicalWorld() const;
		void 									physicalWorld(std::shared_ptr<PhysicalWorld> world);

		std::shared_ptr<InputManager> 			inputManager() const;
		void 									inputManager(std::shared_ptr<InputManager> inputManager);

		float 									time() const;

		UpdateCallback 							update() const;
		void 									update(UpdateCallback function);

		DEBUG_OPTIONS 						debugOptions() const;
		void 								debugOptions(DEBUG_OPTIONS options);

		void									run();
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

//		void 									update(Renderer& renderer,
//													   unsigned framebufferWidth,
//													   unsigned framebufferHeight,
//													   Node& pointOfView,
//													   const DEBUG_OPTIONS& debugOptions,
//													   FrameStats& stats);

//		std::shared_ptr<Geometry>				skyboxGeometry() const;
		
//		AABB									aabb() const;
//		glm::vec3 								extent() const;
		
//		void 									attachedToRenderContext(std::shared_ptr<RenderContext> renderContext);

//		std::weak_ptr<RenderContext> 			renderContext() const;
//		void 									renderContext(std::shared_ptr<RenderContext> context);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<Node>					_rootNode;
//		std::shared_ptr<MaterialProperty>		_background;
//		std::shared_ptr<Geometry>				_skyboxGeometry;
//		float									_fogStartDistance;
//		float									_fogEndDistance;
//		float									_fogDensityExponent;
//		std::shared_ptr<Color>					_fogColor;
//		std::shared_ptr<PhysicalWorld> 			_physicalWorld;
//		std::weak_ptr<RenderContext>			_renderContext;
//		RenderContext&	thing;

//		RenderContext -> Window
//				Renderer -> OpenGLRenderer
//				InputManager -> WindowInputManager
//		PhysicalWorld ("PhysicsEnvironment" ?)
//		PhysicsSimulator -> BulletPhysicsSimulator
//				Background, fog, etc ("VisualEnvironment" ?)

//		std::unique_ptr<RenderContext>			_renderContext;
//		std::unique_ptr<Renderer>				_renderer;
		std::shared_ptr<VisualWorld> 			_visualWorld;
		std::shared_ptr<PhysicalWorld> 			_physicalWorld;
//		std::unique_ptr<PhysicsSimulator>		_physicsSimulator;
		std::shared_ptr<InputManager>			_inputManager;

		UpdateCallback							_update;

		DEBUG_OPTIONS								_debugOptions;
	};
}


#endif /* Scene_h */
