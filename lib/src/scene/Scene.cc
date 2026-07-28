//
//  Scene.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/scene/Scene.h"

#include <filesystem>
#include <utility>

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/Configuration.h"
#include "a3d/log/Log.h"
#include "a3d/input/GLFWInputManager.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/profile/Profile.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/importer/GlTFImporter.h"
#include "a3d/util/Flow.h"
#include "a3d/visual/VisualWorld.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;
using namespace std::filesystem;

/// Public Static Member Functions ///

unique_ptr<Scene> Scene::FromFile(const filesystem::path& path, ImportOptions options) {
	return GlTFImporter(path, options).scene();
}

/// Public Lifecycle Functions ///

Scene::Scene():
		_name{},
		_rootNode{make_shared<Node>("root node")},
		_visualWorld{},
		_physicsWorld{},
		_inputManager{},
		_debugOptions{DebugOptions::None},
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
			 unique_ptr<PhysicsWorld> physicsWorld,
			 unique_ptr<InputManager> inputManager):
		Scene() {

	_visualWorld = std::move(visualWorld);
	_physicsWorld = std::move(physicsWorld);
	_inputManager = std::move(inputManager);

	if (_visualWorld) _visualWorld->attachedToScene(*this);
	if (_physicsWorld) _physicsWorld->attachedToScene(*this);
	if (_inputManager) _inputManager->attachedToScene(*this);
}

Scene::Scene(const string& name,
			 unique_ptr<VisualWorld> visualWorld,
			 unique_ptr<PhysicsWorld> physicsWorld,
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
	if (_physicsWorld) _physicsWorld->detachedFromScene(*this);
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

			if (_inputManager) {
				_inputManager->visualWorldAttachedToScene(*this);
			}
		}
//	}
}

PhysicsWorld* Scene::physicsWorld() const {
	return _physicsWorld.get();
}

void Scene::physicsWorld(unique_ptr<PhysicsWorld> world) {

	if (_physicsWorld) {

		_physicsWorld->detachedFromScene(*this);

		if (_rootNode) {
			_rootNode->physicsWorldDetachedFromScene(*_physicsWorld, *this);
		}
	}

	_physicsWorld = std::move(world);

	if (_physicsWorld) {

		_physicsWorld->attachedToScene(*this);

		if (_rootNode) {
			_rootNode->physicsWorldAttachedToScene(*_physicsWorld, *this);
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
		_inputManager->attachedToScene(*this);
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

Scene::UpdateCallback Scene::updateCallback() const {
	return _updateCallback;
}

void Scene::updateCallback(UpdateCallback function) {
	_updateCallback = function;
}

/// Internal Member Functions ///

void Scene::update(const HostUpdateInfo& info) {

	if (!util::flow::edge_guard(_rootNode, [&] {
		log::e()("No root node attached to Scene {:p}", static_cast<void *>(this));
	})) return;

	FrameStats stats{};

	prof::profile(_profiler, Profiler::Tag::Frame, [&] {

		if (_inputManager) {

			prof::profile(_profiler, Profiler::Tag::EngineCpu, [&] {
				_inputManager->update();
			});
		}

		if (_updateCallback) {

			prof::profile(_profiler, Profiler::Tag::Application, [&] {
				(_updateCallback)(*this, info.elapsedTime, info.deltaTime);
			});
		}

		if (_physicsWorld) {

			_physicsWorld->step(*this,
								 info.elapsedTime,
								 info.deltaTime,
								 stats,
								 _profiler);
		}

		if (_visualWorld) {

			_visualWorld->draw(*this,
							   (_physicsWorld ? _physicsWorld.get() : nullptr),
							   info.elapsedTime,
							   info.deltaTime,
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
