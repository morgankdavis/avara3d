//
//  Scene.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/scene/Scene.h"

#include <filesystem>
#include <stdexcept>
#include <thread>
#include <utility>

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/Configuration.h"
#include "a3d/Image.h"
#include "a3d/log/Log.h"
#include "a3d/input/GLFWInputManager.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/profile/Profile.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/render/Renderer.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/importer/GlTFImporter.h"
#include "a3d/util/flow.h"
#include "a3d/visual/VisualWorld.h"
#include "a3d/visual/camera/Camera.h"
#include "a3d/visual/light/Light.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;
using namespace std::filesystem;

/// Private Static Non-Member Prototypes ///

static void 		GetRunTime(double time, // time since reference
							  double& runT, // time since reference excluding paused time
							  double& deltaRunT); // time since last call excluding paused time

/// Public Static Member Functions ///

unique_ptr<Scene> Scene::FromFile(const filesystem::path& path, ImportOptions options) {
	return GlTFImporter(path, options).scene();
}

/// Public Lifecycle Functions ///

Scene::Scene():
		_name{},
		_rootNode{make_shared<Node>("root node")},
		_visualWorld{},
		_physicalWorld{},
		_inputManager{},
		_debugOptions{DebugOptions::None},
		_startTime{0},
		_updateCallback{},
		_profiler{},
		_frameStatsHistory{a3d::config::FRAME_STATS_HISTORY_DURATION} {

	_rootNode->attachedToScene(*this);
}

Scene::Scene(const string& name):
	Scene() {

	_name = name;
}

Scene::Scene(unique_ptr<VisualWorld> visualWorld,
			 unique_ptr<PhysicalWorld> physicsWorld,
			 unique_ptr<InputManager> inputManager):
		Scene() {

	_visualWorld = std::move(visualWorld);
	_physicalWorld = std::move(physicsWorld);
	_inputManager = std::move(inputManager);

	if (_visualWorld) _visualWorld->attachedToScene(*this);
	if (_physicalWorld) _physicalWorld->attachedToScene(*this);
//	if (_inputManager) _inputManager->attachedToScene(*this);
}

Scene::Scene(const string& name,
			 unique_ptr<VisualWorld> visualWorld,
			 unique_ptr<PhysicalWorld> physicsWorld,
			 unique_ptr<InputManager> inputManager):
		Scene(std::move(visualWorld), std::move(physicsWorld), std::move(inputManager)) {

	_name = name;
}

Scene::~Scene() {

	if (_name != nullopt) {
		log::d()("Destroying Scene '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		log::d()("Destroying Scene {:p}", static_cast<void*>(this));
	}

	if (_rootNode) _rootNode->detachedFromScene(*this);
	if (_visualWorld) _visualWorld->detachedFromScene(*this);
	if (_physicalWorld) _physicalWorld->detachedFromScene(*this);
//	if (_inputManager) _inputManager->detachedFromScene(*this);
}

/// Public Member Functions ///

const optional<string>& Scene::name() const {
	return _name;
}

void Scene::name(const string& name) {
	_name = name;
}

const shared_ptr<Node>& Scene::rootNode() const {
	return _rootNode;
}

void Scene::rootNode(const shared_ptr<Node>& node) {

//	if () // check they are not the same
	if (_rootNode) {
		_rootNode->detachedFromScene(*this);
	}

	//_rootNode = std::move(node);
	_rootNode = node;

	if (_rootNode) {
		_rootNode->attachedToScene(*this);
	}
}

//Node* Scene::rootNode() const {
//	return _rootNode.get();
//}
//
//void Scene::rootNode(unique_ptr<Node>& node) {
//
//	if () // check they are not the same
//	if (_rootNode) {
//		_rootNode->detachedFromScene(this);
//	}
//
//	_rootNode = std::move(node);
//
//	if (_rootNode) {
//		_rootNode->attachedToScene(this);
//	}
//}

VisualWorld* Scene::visualWorld() const {
	return _visualWorld.get();
}

void Scene::visualWorld(unique_ptr<VisualWorld> world) {

//	if (world != _visualWorld) {

		if (_visualWorld) {

			_visualWorld->detachedFromScene(*this);

			if (_rootNode) {
				_rootNode->visualWorldDetachedFromScene(*_visualWorld, *this);
			}
		}

		_visualWorld = std::move(world);

		if (_visualWorld) {

			_visualWorld->attachedToScene(*this);

			if (_rootNode) {
				_rootNode->visualWorldAttachedToScene(*_visualWorld, *this);
			}
		}
//	}
}

PhysicalWorld* Scene::physicalWorld() const {
	return _physicalWorld.get();
}

void Scene::physicalWorld(unique_ptr<PhysicalWorld> world) {

	if (_physicalWorld) {

		_physicalWorld->detachedFromScene(*this);

		if (_rootNode) {
			_rootNode->physicalWorldDetachedFromScene(*_physicalWorld, *this);
		}
	}

	_physicalWorld = std::move(world);

	if (_physicalWorld) {

		_physicalWorld->attachedToScene(*this);

		if (_rootNode) {
			_rootNode->physicalWorldAttachedToScene(*_physicalWorld, *this);
		}
	}
}

InputManager* Scene::inputManager() const {
	return _inputManager.get();
}

void Scene::inputManager(unique_ptr<InputManager> inputManager) {

	if (_inputManager) {
//		_inputManager->detachedFromScene(*this);
	}

	_inputManager = std::move(inputManager);

	if (_inputManager) {
//		_inputManager->attachedToScene(*this);
	}
}

//AABB Scene::aabb() {
//
//}
//
//vec3 Scene::extent() {
//
//}

AABB Scene::aabb(bool vertfit) const {
	AABB out = AABB::Invalid();

	for (auto& node : rootNode()->children()) {
		out |= node->aabb();
	}

	return out;
}

vec3 Scene::extent(bool vertfit) const {
	auto aabb = Scene::aabb(vertfit);
	return aabb.max - aabb.min;
}

Scene::DebugOptions Scene::debugOptions() const {
	return _debugOptions;
}

void Scene::debugOptions(DebugOptions options) {

#ifdef A3D_GL_ES
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		throw Exception("DEBUG_OPTIONS::SHOW_WIREFRAMES not supported on this platform.");
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
		throw Exception("DEBUG_OPTIONS::SHOW_BOUNDING_BOXES not supported on this platform.");
	}
#endif

	_debugOptions = options;
}

double Scene::time() const {

	// https://randomascii.wordpress.com/2012/02/13/dont-store-that-in-a-float/

	if (_startTime != 0) {
		auto now = chrono::steady_clock::now();
		auto nowSinceEpoch = chrono::duration<double>(now.time_since_epoch()).count();
		return nowSinceEpoch - _startTime;
	}
	return 0;
}

Scene::UpdateCallback Scene::updateCallback() const {
	return _updateCallback;
}

void Scene::updateCallback(UpdateCallback function) {
	_updateCallback = function;
}

/// Internal Member Functions ///

void Scene::update() {

	if (!util::flow::edge_guard(_rootNode, [&] {
		log::e()("No root node attached to Scene {:p}", static_cast<void *>(this));
	})) return;

	static FrameStats stats;
	memset(&stats, 0, sizeof(FrameStats));

	prof::profile(_profiler, Profiler::Tag::Frame, [&] {

		static auto now = chrono::system_clock::now();
		_startTime = chrono::duration<double>(now.time_since_epoch()).count();

		static double deltaT, runT, deltaRunT; // TODO: manage these in caller

		GetRunTime(time(),
				   runT,
				   deltaRunT);

		if (_inputManager) {

			prof::profile(_profiler, Profiler::Tag::EngineCpu, [&] {
				_inputManager->update();
			});
		}

		if (_updateCallback) {

			prof::profile(_profiler, Profiler::Tag::Application, [&] {
				(_updateCallback)(*this, runT, deltaRunT);
			});
		}

		if (_physicalWorld) {

			_physicalWorld->step(*this,
								 runT,
								 deltaRunT,
								 stats,
								 _profiler);
		}

		if (_visualWorld) {

			_visualWorld->draw(*this,
							   (_physicalWorld ? _physicalWorld.get() : nullptr),
							   runT,
							   deltaRunT,
							   _debugOptions,
							   stats,
							   _profiler,
							   _frameStatsHistory);
		}
	});

	stats.frameTime = _profiler.time(Profiler::Tag::Frame);
	stats.engineCpuTime = _profiler.time(Profiler::Tag::EngineCpu);
	stats.renderCpuTime = _profiler.time(Profiler::Tag::RenderCpu);
	stats.renderGpuTime = _profiler.time(Profiler::Tag::RenderGpu);
	stats.physicsTime = _profiler.time(Profiler::Tag::Physics);
	stats.applicationTime = _profiler.time(Profiler::Tag::Application);

	_frameStatsHistory.add(stats);

	_profiler.reset();
}

/// Private Static ///

void GetRunTime(double time, // time since reference
				double& runT, // time since reference excluding paused time
				double& deltaRunT) { // time since last call excluding paused time

	const double t = time;
	static double prevT = t;
	double deltaT = t - prevT;
	prevT = t;

	runT = t;

	static double prevRunT = runT;
	deltaRunT = runT - prevRunT;
	prevRunT = runT;
}
