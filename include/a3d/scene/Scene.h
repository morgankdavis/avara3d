//
//  Scene.h
//	avara3d
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

#include "a3d/Types.h"


namespace a3d {


	class Color;
	class InputManager;
	class Mesh;
	class Node;
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

		static std::unique_ptr<Scene> 			FromFile(const std::filesystem::path& path,
														  SceneImportOptions options = SceneImportOptions::ImportAll);
		static double 							Time();

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Scene();
		Scene(std::unique_ptr<VisualWorld> visualWorld,
			  std::unique_ptr<PhysicalWorld> physicsWorld,
			  std::unique_ptr<InputManager> inputManager);
		~Scene();

/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::shared_ptr<Node> 				rootNode() const;
		void 								rootNode(const std::shared_ptr<Node>& node);

//		Node* 								rootNode() const;
//		void 								rootNode(std::unique_ptr<Node>& node);

		VisualWorld* 						visualWorld() const;
		void 								visualWorld(std::unique_ptr<VisualWorld>& world);
		
		PhysicalWorld* 						physicalWorld() const;
		void 								physicalWorld(std::unique_ptr<PhysicalWorld>& world);

		InputManager* 						inputManager() const;
		void 								inputManager(std::unique_ptr<InputManager>& manager);

		DebugOptions 						debugOptions() const;
		void 								debugOptions(DebugOptions options);

		void								run();
		void								stop();

		bool								running() const;

		bool								paused() const;
		void								paused(bool flag);

		const Stats&						stats() const;

		UpdateCallback 						update() const;
		void 								update(UpdateCallback function);

/*********************************************************************************************
	Internal
 *********************************************************************************************/



/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<Node>				_rootNode;
//		std::unique_ptr<Node>				_rootNode;
		std::unique_ptr<VisualWorld> 		_visualWorld;
		std::unique_ptr<PhysicalWorld> 		_physicalWorld;
		std::unique_ptr<InputManager>		_inputManager;
		DebugOptions						_debugOptions;
		bool								_running;
		bool								_paused;
		Stats								_stats;
		UpdateCallback						_update;
	};
}


#endif /* Scene_h */
