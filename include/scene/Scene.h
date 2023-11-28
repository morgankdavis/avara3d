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

		std::shared_ptr<VisualWorld> 			visualWorld() const;
		void 									visualWorld(std::shared_ptr<VisualWorld> world);
		
		std::shared_ptr<PhysicalWorld> 			physicalWorld() const;
		void 									physicalWorld(std::shared_ptr<PhysicalWorld> world);

		std::shared_ptr<InputManager> 			inputManager() const;
		void 									inputManager(std::shared_ptr<InputManager> inputManager);

		float 									time() const;

		DEBUG_OPTIONS 							debugOptions() const;
		void 									debugOptions(DEBUG_OPTIONS options);

		const Stats&							stats() const;

		void									run();
		void									stop();

		bool									isRunning() const;

		UpdateCallback 							update() const;
		void 									update(UpdateCallback function);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<Node>					_rootNode;
		std::shared_ptr<VisualWorld> 			_visualWorld;
		std::shared_ptr<PhysicalWorld> 			_physicalWorld;
		std::shared_ptr<InputManager>			_inputManager;
		DEBUG_OPTIONS							_debugOptions;
		Stats									_stats;
		bool									_isRunning;
		UpdateCallback							_update;
	};
}


#endif /* Scene_h */
