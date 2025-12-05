//
//  Scene.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/scene/Scene.h"

#include <chrono>
#include <filesystem>
#include <thread>
#include <utility>

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/Configuration.h"
#include "a3d/CubeImage.h"
#include "a3d/Image.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/input/GLFWInputManager.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/light/Light.h"
#include "a3d/rendering/renderer/Renderer.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/importer/GlTFImporter.h"
#include "a3d/Utilities.h"

using namespace a3d;
using namespace glm;
using namespace std;
using namespace std::filesystem;

/// Private Static Non-Member Prototypes ///

static void 		GetRunTime(double time, // time since reference
							  double& runT, // time since reference excluding paused time
							  double& deltaRunT); // time since last call excluding paused time
static void 		UpdateUserTimeStats(Stats& stats, double startTime, double endTime);
static void			UpdateFrameTimeStats(Stats& stats, double time);

/// Public Static Member Functions ///

unique_ptr<Scene> Scene::FromFile(const filesystem::path& path,
								  SceneImportOptions options) {
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
		_stats{},
		_startTime{0},
		_updateCallback{} {

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
		A3D_LOG_D("Destroying Scene '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		A3D_LOG_D("Destroying Scene {:p}", static_cast<void*>(this));
	}

	if (_rootNode) _rootNode->detachedFromScene(*this);
	if (_visualWorld) _visualWorld->detachedFromScene(*this);
	if (_physicalWorld) _physicalWorld->detachedFromScene(*this);
//	if (_inputManager) _inputManager->detachedFromScene(*this);
}

/// Public Member Functions ///

const optional<std::string>& Scene::name() const {
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

DebugOptions Scene::debugOptions() const {
	return _debugOptions;
}

void Scene::debugOptions(DebugOptions options) {

#ifdef OPENGL_ES
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		throw Exception("DEBUG_OPTIONS::SHOW_WIREFRAMES not supported on this platform.");
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
		throw Exception("DEBUG_OPTIONS::SHOW_BOUNDING_BOXES not supported on this platform.");
	}
#endif

	_debugOptions = options;
}

void Scene::update() {

	if (_rootNode) {

		static auto now = std::chrono::system_clock::now();
		_startTime = std::chrono::duration<double>(now.time_since_epoch()).count();

		static double deltaT, runT, deltaRunT; // TODO: manage these in caller

		GetRunTime(time(),
				   runT,
				   deltaRunT);

		//_stats = {};
		memset(&_stats, 0, sizeof(Stats));
		UpdateFrameTimeStats(_stats, runT);

		if (_inputManager) {
			_inputManager->update();
		}

		if (_updateCallback) {

			auto updateStartTime = time();
			(_updateCallback)(*this, runT, deltaRunT);
			UpdateUserTimeStats(_stats, updateStartTime, time());
		}

		if (_physicalWorld) {

			_physicalWorld->step(*this,
								 runT,
								 deltaRunT,
								 _stats);
		}

		if (_visualWorld) {

			_visualWorld->draw(*this,
							   (_physicalWorld ? _physicalWorld.get() : nullptr),
							   runT,
							   deltaRunT,
							   _debugOptions,
							   _stats);
		}
	}
	else {
		A3D_LOG_E("No root node attached to Scene {:p}", static_cast<void*>(this));
	}
}

double Scene::time() const {

	// https://randomascii.wordpress.com/2012/02/13/dont-store-that-in-a-float/

	if (_startTime != 0) {
		auto now = chrono::system_clock::now();
		auto nowSinceEpoch = chrono::duration<double>(now.time_since_epoch()).count();
		return nowSinceEpoch - _startTime;
	}
	return 0;
}

//bool Scene::paused() const {
//	return _paused;
//}
//
//void Scene::paused(bool flag) {
//	_paused = flag;
//}

const Stats& Scene::stats() const {
	return _stats;
}

Scene::UpdateCallback Scene::updateCallback() const {
	return _updateCallback;
}

void Scene::updateCallback(UpdateCallback function) {
	_updateCallback = function;
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

void UpdateUserTimeStats(Stats& stats, double startTime, double endTime) {

	// current
	auto updateTime = endTime - startTime;
	stats.currentUsertime = updateTime * 1000.0f;

	// average
	static double avg = 0.0;
	static double sampleStartTime = startTime;
	static unsigned updatesSinceSampleStart = 0;
	static double accumulatedUpdateTimeSinceSampleStart = 0;
	double elapsedTimeSinceSampleStart = endTime - sampleStartTime;
	if (elapsedTimeSinceSampleStart >= FRAMETIME_AVERAGING_INTERVAL) {

		avg = (accumulatedUpdateTimeSinceSampleStart * 1000.0f) / updatesSinceSampleStart;

		sampleStartTime = startTime;
		updatesSinceSampleStart = 0;
		accumulatedUpdateTimeSinceSampleStart = 0;
	}
	else {
		++updatesSinceSampleStart;
		accumulatedUpdateTimeSinceSampleStart += updateTime;
	}

	stats.averageUsertime = avg;
//	stats.averagingInterval = FRAMETIME_AVERAGING_INTERVAL;
}

void UpdateFrameTimeStats(Stats& stats, double time) {

	// current
	static double previousTime = time;
	double deltaTime = time - previousTime;
	previousTime = time;
	stats.currentFramerate = 60.0f / deltaTime;
	stats.currentFrametime = deltaTime * 1000.0f;

	// average
	static double fpsAvg = 0.0;
	static double msAvg = 0.0;
	static unsigned framesSinceSampleStart = 0;
	static double sampleStartTime = time;
	double elapsedTimeSinceSampleStart = time - sampleStartTime;
	if (elapsedTimeSinceSampleStart >= FRAMETIME_AVERAGING_INTERVAL) {

		fpsAvg = (double)framesSinceSampleStart / elapsedTimeSinceSampleStart;
		msAvg = (elapsedTimeSinceSampleStart * 1000.0f) / framesSinceSampleStart;

		sampleStartTime = time;
		framesSinceSampleStart = 0;
	}
	else {
		++framesSinceSampleStart;
	}
	stats.averageFramerate = fpsAvg;
	stats.averageFrametime = msAvg;
	stats.averagingInterval = FRAMETIME_AVERAGING_INTERVAL;
}
