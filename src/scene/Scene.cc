//
//  Scene.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "ae/scene/Scene.h"

#include <chrono>
#include <filesystem>
#include <thread>

#include "ae/Buffer.h"
#include "ae/Color.h"
#include "ae/CubeImage.h"
#include "ae/Image.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/geometry/Geometry.h"
#include "ae/geometry/GeometryElement.h"
#include "ae/input/platform/desktop/WindowInputManager.h"
#include "ae/physics/PhysicsBody.h"
#include "ae/physics/PhysicalWorld.h"
#include "ae/rendering/Light.h"
#include "ae/rendering/Renderer.h"
#include "ae/rendering/VisualWorld.h"
#include "ae/rendering/camera/Camera.h"
#include "ae/rendering/context/RenderContext.h"
#include "ae/rendering/materials/Material.h"
#include "ae/rendering/materials/MaterialProperty.h"
#include "ae/scene/Node.h"
#include "ae/scene/importer/GlTFImporter.h"


using namespace ae;
using namespace glm;
using namespace std;
using namespace std::filesystem;


constexpr float FRAMETIME_AVERAGING_INTERVAL = .25;


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

shared_ptr<Scene> Scene::FromFile(const filesystem::path& path) {
	return GlTFImporter(path).scene();
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Scene::Scene():
		_rootNode(make_shared<Node>("root node")),
		_visualWorld(nullptr),
		_physicalWorld(nullptr),
		_inputManager(nullptr),
		_debugOptions(DEBUG_OPTIONS::NONE),
		_stats({}),
		_running(false),
		_paused(false),
		_update(nullptr) {

	_rootNode->attachedToScene(this);
}

Scene::Scene(shared_ptr<VisualWorld> visualWorld,
			 shared_ptr<PhysicalWorld> physicsWorld,
			 shared_ptr<InputManager> inputManager):
		Scene() {

	_visualWorld = visualWorld;
	_physicalWorld = physicsWorld;
	_inputManager = inputManager;

	if (_visualWorld) _visualWorld->attachedToScene(this);
	if (_physicalWorld) _physicalWorld->attachedToScene(this);
	if (_inputManager) _inputManager->attachedToScene(this);
}

Scene::~Scene() {
	AE_LOG_D("Destroying Scene {:p}", static_cast<void*>(this));

	if (_rootNode) _rootNode->detachedFromScene(this);
	if (_visualWorld) _visualWorld->detachedFromScene(this);
	if (_physicalWorld) _physicalWorld->detachedFromScene(this);
	if (_inputManager) _inputManager->detachedFromScene(this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<Node> Scene::rootNode() const {
	return _rootNode;
}

void Scene::rootNode(shared_ptr<Node> node) {

//	if () // check they are not the same
	if (_rootNode) {
		_rootNode->detachedFromScene(this);
	}

	_rootNode = node;

	if (_rootNode) {
		_rootNode->attachedToScene(this);
	}
}

shared_ptr<VisualWorld> Scene::visualWorld() const {
	return _visualWorld;
}

void Scene::visualWorld(shared_ptr<VisualWorld> world) {

//	if (world != _visualWorld) {

		if (_visualWorld) {

			_visualWorld->detachedFromScene(this);

			if (_rootNode) {
				_rootNode->visualWorldDetachedFromScene(_visualWorld.get(), this);
			}
		}

		_visualWorld = world;

		if (_visualWorld) {

			_visualWorld->attachedToScene(this);

			if (_rootNode) {
				_rootNode->visualWorldAttachedToScene(_visualWorld.get(), this);
			}
		}
//	}
}

shared_ptr<PhysicalWorld> Scene::physicalWorld() const {
	return _physicalWorld;
}

void Scene::physicalWorld(shared_ptr<PhysicalWorld> world) {

	if (_physicalWorld) {

		_physicalWorld->detachedFromScene(this);

		if (_rootNode) {
			_rootNode->physicalWorldDetachedFromScene(_physicalWorld.get(), this);
		}
	}

	_physicalWorld = world;

	if (world) {

		world->attachedToScene(this);

		if (_rootNode) {
			_rootNode->physicalWorldAttachedToScene(world.get(), this);
		}
	}
}

shared_ptr<InputManager> Scene::inputManager() const {
	return _inputManager;
}

void Scene::inputManager(shared_ptr<InputManager> inputManager) {

	if (_inputManager) {
		_inputManager->detachedFromScene(this);
	}

	_inputManager = inputManager;

	if (inputManager) {
		inputManager->attachedToScene(this);
	}
}

double Scene::time() const { // TODO: move this? make it static?
	static auto startTime = chrono::high_resolution_clock::now();
	auto nowTime = chrono::high_resolution_clock::now();
	return (chrono::duration<double>(nowTime - startTime)).count();
}

DEBUG_OPTIONS Scene::debugOptions() const {
	return _debugOptions;
}

void Scene::debugOptions(DEBUG_OPTIONS options) {

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

			GetRunTime(time(),
					   _paused,
					   runT,
					   deltaRunT);

			memset(&_stats, 0, sizeof(Stats));
			UpdateFrameTimeStats(_stats, runT);

			if (_inputManager) {
				_inputManager->update();
			}

			if (_update) {

				auto updateStartTime = time();
				(_update)(*this, runT);
				UpdateUserTimeStats(_stats, updateStartTime, time());
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
		AE_LOG_E("No root node attached to Scene {:p}", static_cast<void*>(this));
	}
}

void Scene::stop() {

	if (_running) {
		_running = false;
	}
	else {
		AE_LOG_W("Attempting to stop when Scene not running.");
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

	static const double FRAMETIME_AVERAGING_INTERVAL = .25; // TEMPORARY

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
