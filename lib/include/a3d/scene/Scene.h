//
//  Scene.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SCENE_SCENE_H
#define AVARA3D_SCENE_SCENE_H

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "a3d/Timing.h"
#include "a3d/Math.h"
#include "a3d/physics/PhysicsInventory.h"
#include "a3d/util/Bitmask.h"

namespace a3d {

	struct AABB;

	class Color;
	class InputManager;
	class Mesh;
	class Node;
	class PhysicsWorld;
	class Profiler;
	class Renderer;
	class RenderContext;
	class VisualWorld;

	class Scene {

	public:
		/// Public Types ///

		enum class ImportOptions : uint16_t {
			None = 					0,
			ImportMeshes =			1 << 0,
			ImportMaterials =		1 << 1,
			ImportLights =			1 << 2,
			ImportCameras = 		1 << 3,
			ImportAll =				UINT16_MAX
		};

		// TODO: move to VisualWorld?
		enum class DebugOptions : uint32_t {
			None =							0,
			ShowStatsOverlay = 				1 << 0,
			ShowBoundingBoxes = 			1 << 1,
			ShowWireframes = 				1 << 2,
			ShowCameras = 					1 << 3,
			ShowLights = 					1 << 4,
			ShowLightExtents = 				1 << 5,
			ShowPhysicsBoundingBoxes = 		1 << 6,
			ShowPhysicsWireframes = 		1 << 7,
			ShowPhysicsContactPoints = 		1 << 8,
			ShowPhysicsNormals = 			1 << 9,
			ShowPhysicsConstraints =		1 << 10,
			ShowPhysicsConstraintLimits	=	1 << 11
		};

		using WillSimulateCallback = std::function<void(Scene& scene,
		                                                const SimulationStepInfo& info)>;
		using DidSimulateCallback = std::function<void(Scene& scene,
		                                               const SimulationStepInfo& info)>;

		/// Public Static Member Functions ///

		static std::unique_ptr<Scene> 		FromFile(const std::filesystem::path& path,
													  ImportOptions options =
													  ImportOptions::ImportAll);

		/// Public Lifecycle Functions ///

		Scene();
		explicit Scene(const std::string& name);
		Scene(std::unique_ptr<VisualWorld> visualWorld,
			  std::unique_ptr<PhysicsWorld> physicsWorld,
			  std::unique_ptr<InputManager> inputManager);
		Scene(const std::string& name,
			  std::unique_ptr<VisualWorld> visualWorld,
			  std::unique_ptr<PhysicsWorld> physicsWorld,
			  std::unique_ptr<InputManager> inputManager);

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		Scene(Scene&&) = delete;
		Scene& operator=(Scene&&) = delete;

		~Scene();

		/// Public Member Functions ///

		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);

		const std::shared_ptr<Node>&		rootNode() const;
		void 								rootNode(const std::shared_ptr<Node>& node);

		VisualWorld* 						visualWorld() const;
		void 								visualWorld(std::unique_ptr<VisualWorld> world);
		
		PhysicsWorld* 						physicsWorld() const;
		void 								physicsWorld(std::unique_ptr<PhysicsWorld> world);

		InputManager* 						inputManager() const;
		void 								inputManager(std::unique_ptr<InputManager> manager);

		AABB 								aabb(bool vertfit = false) const;
		math::vec3 							extent(bool vertfit = false) const;

		DebugOptions 						debugOptions() const;
		void 								debugOptions(DebugOptions options);

		WillSimulateCallback				willSimulateCallback() const;
		void								willSimulateCallback(WillSimulateCallback callback);

		DidSimulateCallback					didSimulateCallback() const;
		void								didSimulateCallback(DidSimulateCallback callback);

		/// Internal Member Functions ///

		void 								updateHostEvents(Profiler& profiler);
		void 								updateInput(Profiler& profiler);
		PhysicsInventory					simulate(const SimulationStepInfo& info,
												 Profiler& profiler);

	private:

		/// Private Member Variables ///

		std::optional<std::string>			_name;
		std::shared_ptr<Node>				_rootNode;
		std::unique_ptr<VisualWorld> 		_visualWorld;
		std::unique_ptr<PhysicsWorld> 		_physicsWorld;
		std::unique_ptr<InputManager>		_inputManager;
		DebugOptions						_debugOptions;
		WillSimulateCallback				_willSimulateCallback;
		DidSimulateCallback					_didSimulateCallback;
	};

	namespace util::bitmask {
		template <> struct enable_ops<Scene::ImportOptions> : std::true_type {};
		template <> struct enable_ops<Scene::DebugOptions> : std::true_type {};
	}
}

#endif /* AVARA3D_SCENE_SCENE_H */
