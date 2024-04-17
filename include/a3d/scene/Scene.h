//
//  Scene.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SCENE_H
#define AVARA3D_SCENE_H


#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <optional>
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
	Public Types
 *********************************************************************************************/

	public:

		using UpdateCallback =				std::function<void(Scene& scene, float time)>;

/*********************************************************************************************
	Public Static Members
 *********************************************************************************************/

		static std::unique_ptr<Scene> 		FromFile(const std::filesystem::path& path,
													  SceneImportOptions options =
															  SceneImportOptions::ImportAll);
		static double 						Time();

/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

		Scene();
		explicit Scene(const std::string& name);
		Scene(std::unique_ptr<VisualWorld> visualWorld,
			  std::unique_ptr<PhysicalWorld> physicsWorld,
			  std::unique_ptr<InputManager> inputManager);
		Scene(const std::string& name,
			  std::unique_ptr<VisualWorld> visualWorld,
			  std::unique_ptr<PhysicalWorld> physicsWorld,
			  std::unique_ptr<InputManager> inputManager);
		~Scene();

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);

		const std::shared_ptr<Node>&		rootNode() const;
		void 								rootNode(const std::shared_ptr<Node>& node);

		VisualWorld* 						visualWorld() const;
		void 								visualWorld(std::unique_ptr<VisualWorld> world);
		
		PhysicalWorld* 						physicalWorld() const;
		void 								physicalWorld(std::unique_ptr<PhysicalWorld> world);

		InputManager* 						inputManager() const;
		void 								inputManager(std::unique_ptr<InputManager> manager);

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
	Private IVars
 *********************************************************************************************/

	private:

		std::optional<std::string>			_name;
		std::shared_ptr<Node>				_rootNode;
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


#endif /* AVARA3D_SCENE_H */
