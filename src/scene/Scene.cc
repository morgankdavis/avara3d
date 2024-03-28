//
//  Scene.cc
//	avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "a3d/scene/Scene.h"

#include <chrono>
#include <filesystem>
#include <thread>

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/CubeImage.h"
#include "a3d/Image.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/input/platform/desktop/WindowInputManager.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/rendering/Light.h"
#include "a3d/rendering/Renderer.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/importer/GlTFImporter.h"


using namespace a3d;
using namespace glm;
using namespace std;
using namespace std::filesystem;


constexpr double FRAMETIME_AVERAGING_INTERVAL = .5;


/*********************************************************************************************
	Private Static Prototypes
 *********************************************************************************************/

static void 						GetRunTime(double time, // time since reference
											  bool paused,
											  double& runT, // time since reference excluding paused time
											  double& deltaRunT); // time since last call excluding paused time
static void 						UpdateUserTimeStats(Stats& stats, double startTime, double endTime);
static void							UpdateFrameTimeStats(Stats& stats, double time);

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

unique_ptr<Scene> Scene::FromFile(const filesystem::path& path,
								  SceneImportOptions options) {
	return GlTFImporter(path, options).scene();
}

double Scene::Time() {
	static auto startTime = chrono::high_resolution_clock::now();
	auto nowTime = chrono::high_resolution_clock::now();
	return (chrono::duration<double>(nowTime - startTime)).count();
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Scene::Scene():
		_name{},
		_rootNode{make_shared<Node>("root node")},
//		_rootNode(make_unique<Node>("root node")),
		_visualWorld{},
		_physicalWorld{},
		_inputManager{},
		_debugOptions{DebugOptions::None},
		_stats{},
		_running{false},
		_paused{false},
		_update{} {

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
	if (_inputManager) _inputManager->attachedToScene(*this);
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
	if (_inputManager) _inputManager->detachedFromScene(*this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

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
////	if () // check they are not the same
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
		_inputManager->detachedFromScene(*this);
	}

	_inputManager = std::move(inputManager);

	if (_inputManager) {
		_inputManager->attachedToScene(*this);
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

void Scene::run() {

	if (_rootNode) {

		_running = true;

		if (_visualWorld) {
			_visualWorld->checkAddDefaultLighting();
		}

		double deltaT, runT, deltaRunT;

		do {

			GetRunTime(Scene::Time(),
					   _paused,
					   runT,
					   deltaRunT);

			memset(&_stats, 0, sizeof(Stats));
			UpdateFrameTimeStats(_stats, runT);

			if (_inputManager) {
				_inputManager->update();
			}

			if (_update) {

				auto updateStartTime = Scene::Time();
				(_update)(*this, runT);
				UpdateUserTimeStats(_stats, updateStartTime, Scene::Time());
			}

			if (!_paused) {

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
				this_thread::sleep_for(chrono::microseconds(16667));
			}

		} while (_running);
	}
	else {
		A3D_LOG_E("No root node attached to Scene {:p}", static_cast<void*>(this));
	}
}

void Scene::stop() {

	if (_running) {
		_running = false;
	}
	else {
		A3D_LOG_W("Attempting to stop when Scene not running.");
	}
}

bool Scene::running() const {
	return _running;
}

bool Scene::paused() const {
	return _paused;
}

void Scene::paused(bool flag) {
	_paused = flag;
}

const Stats& Scene::stats() const {
	return _stats;
}

Scene::UpdateCallback Scene::update() const {
	return _update;
}

void Scene::update(UpdateCallback function) {
	_update = function;
}

/*********************************************************************************************
	Private Static
 *********************************************************************************************/

void GetRunTime(double time, // time since reference
					   bool paused,
					   double& runT, // time since reference excluding paused time
					   double& deltaRunT) { // time since last call excluding paused time

	const double t = time;
	static double prevT = t;
	double deltaT = t - prevT;
	prevT = t;

	static double pauseTime = 0;
	runT = t - pauseTime;

	static double prevRunT = runT;
	deltaRunT = runT - prevRunT;
	prevRunT = runT;

	if (paused) pauseTime += deltaT;
}

void UpdateUserTimeStats(Stats& stats, double startTime, double endTime) {

	// current
	auto updateTime = endTime - startTime;
	stats.currentUsertime = updateTime * 1000.0f;

	constexpr double FRAMETIME_AVERAGING_INTERVAL = .5; // TEMPORARY

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
