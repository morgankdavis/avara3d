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

#include "fmt/format.h"
#include "glm/gtc/type_ptr.hpp"
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

#include "ae/Color.h"
#include "ae/CubeImage.h"
#include "ae/Image.h"
#include "ae/Utilities.h"
#include "ae/diagnostic/exceptions/UnsupportedFormat.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/geometry/Geometry.h"
#include "ae/geometry/GeometryElement.h"
#include "ae/input/platform/desktop/WindowInputManager.h"
#include "ae/physics/PhysicsBody.h"
#include "ae/physics/PhysicalWorld.h"
#include "ae/rendering/Light.h"
#include "ae/rendering/Renderer.h"
#include "ae/rendering/VisualWorld.h"
#include "ae/rendering/context/RenderContext.h"
#include "ae/rendering/camera/PerspectiveCamera.h"
#include "ae/rendering/materials/Material.h"
#include "ae/rendering/materials/MaterialProperty.h"
#include "ae/scene/Node.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;
using namespace std::filesystem;


constexpr float FRAMETIME_AVERAGING_INTERVAL = .25;


/*********************************************************************************************
	Private Static Prototypes
 *********************************************************************************************/

static void 						LoadGlTF(Scene& aeScene, const filesystem::path& path);
static void 						VisitGlTFNode(tinygltf::Model& model,
												 tinygltf::Node& node,
												 shared_ptr<Node> parent);
static shared_ptr<Geometry>			GeometryFromGlFTNode(tinygltf::Model& model,
															tinygltf::Node& node);
static shared_ptr<GeometryElement>	GeometryElementFromGlFTMesh(const tinygltf::Mesh& mesh);
static shared_ptr<Material>			MaterialFromGlFTMaterial(const tinygltf::Material& material);
static shared_ptr<Light>			LightFromGlTFNode(tinygltf::Model& model,
													  tinygltf::Node& node);
static shared_ptr<Camera>			CameraFromGlTFNode(tinygltf::Model& model,
														tinygltf::Node& node);
static mat4							TransformFromGlFTNode(tinygltf::Node& node);
static shared_ptr<Color>			ColorFromGlTFColorVec(vector<double>& vec);

static void 						GetRunTime(double time, // time since reference
											  bool paused,
											  double& runT, // time since reference excluding paused time
											  double& deltaRunT); // time since last call excluding paused time
static void 						UpdateUserTimeStats(Stats& stats, double startTime, double endTime);
static void							UpdateFrameTimeStats(Stats& stats, double time);
//static shared_ptr<Image> 			MissingTextureImage();

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<Scene> Scene::FromFile(const filesystem::path& path) {

	auto extension = path.extension();
	if (extension == ".glb" || extension == ".gltf") {
		auto scene = make_shared<Scene>();
		LoadGlTF(*scene, path);
		return scene;
	}
	else {
		throw UnsupportedFormat(fmt::format("Unsupported scene format: {}", extension.string()));
	}

	return nullptr;
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

double Scene::time() const {
	static auto startTime = chrono::high_resolution_clock::now();
	auto nowTime = chrono::high_resolution_clock::now();
	return (chrono::duration<double>(nowTime - startTime)).count();
}

DEBUG_OPTIONS Scene::debugOptions() const {
	return _debugOptions;
}

void Scene::debugOptions(DEBUG_OPTIONS options) {

#ifdef ANDROID
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

void LoadGlTF(Scene& aeScene, const filesystem::path& path) {

	using namespace tinygltf;

	TinyGLTF loader;
	Model model;
	string error;
	string warning;

	auto extension = path.extension();
	bool res = false;
	if (extension == ".glb") {
		res = loader.LoadBinaryFromFile(&model, &error, &warning, path.string());
	}
	else if (extension == ".gltf") {
		res = loader.LoadASCIIFromFile(&model, &error, &warning, path.string());
	}
	else {
		AE_LOG_E("Unsupported file extension: {}", extension.string());
	}

	if (res) {

		if (!warning.empty()) {
			AE_LOG_W("Warning loading glTF: {}", warning);
		}

		auto& scenes = model.scenes;
		if (!scenes.empty()) {

			if (scenes.size() > 1) {
				AE_LOG_W("Ignoring additional scenes.");
			}

			// example uses: 'model.scenes[model.defaultScene > -1 ? model.defaultScene : 0]'
			auto& scene = scenes[0];
			auto& nodes = scene.nodes;

			if (!nodes.empty()) {

				for (auto n : scene.nodes) {
					AE_LOG_D("Visiting node {}...", n);

					tinygltf::Node& node = model.nodes[n];

					VisitGlTFNode(model, node, aeScene.rootNode());
				}

				AE_LOG_D("Done loading glTF.");
			}
			else {
				AE_LOG_W("No nodes in scene: {}", scene.name);
			}
		}
		else {
			AE_LOG_E("No scenes.");
		}
	}
	else {

		if (!error.empty()) {
			AE_LOG_E("Error loading glTF: {}", error);
		}
		else {
			AE_LOG_E("Unknown error loading glTF.");
		}
	}
}

void VisitGlTFNode(tinygltf::Model& model,
				   tinygltf::Node& node,
				   shared_ptr<Node> parent) {

	auto aeNode = Node::NamedNode(node.name);

	aeNode->light(LightFromGlTFNode(model, node));
	aeNode->camera(CameraFromGlTFNode(model, node));
	aeNode->geometry(GeometryFromGlFTNode(model, node));
	aeNode->transform(TransformFromGlFTNode(node));

	parent->addChild(aeNode);

	for (auto c : node.children) {
		VisitGlTFNode(model, model.nodes[c], parent);
	}
}

shared_ptr<Geometry> GeometryFromGlFTNode(tinygltf::Model& model,
										  tinygltf::Node& node) {

	auto meshIndex = node.mesh;
	if (meshIndex >= 0) {

		auto& mesh = model.meshes[meshIndex];

		auto& name = mesh.name;
		auto& primitives = mesh.primitives;

		AE_LOG_D("mesh name: {}, primitives.size(): {}", name, primitives.size());
	}

	return nullptr;
}

shared_ptr<GeometryElement> GeometryElementFromGlFTMesh(const tinygltf::Mesh& mesh) {

	return nullptr;
}

shared_ptr<Material> MaterialFromGlFTMaterial(const tinygltf::Material& material) {

	return nullptr;
}

shared_ptr<Light> LightFromGlTFNode(tinygltf::Model& model,
									tinygltf::Node& node) {

	auto index = node.light;
	if (index >= 0) {

		auto& light = model.lights[index];
		auto& type = light.type;

		if (type == "point") {

			auto aeLight = make_shared<Light>(LIGHT_TYPE::POINT);
			aeLight->name(light.name);
//			aeLight->attenuationFactor(float(light.intensity)); // not the same thing
			aeLight->attenuationFactor(0); // temporary
			aeLight->color(ColorFromGlTFColorVec(light.color));
//			// TODO: range, intensity

			return aeLight;
		}
		else {

			AE_LOG_W("Unsupported light type: {}", type);
		}
	}

	return nullptr;
}

shared_ptr<Camera> CameraFromGlTFNode(tinygltf::Model& model,
									  tinygltf::Node& node) {

	auto index = node.camera;
	if (index >= 0) {

		auto& camera = model.cameras[index];
		auto& type = camera.type;

		if (type == "perspective") {

			auto& perspectiveCamera = camera.perspective;

			auto aeCamera = make_shared<PerspectiveCamera>(camera.name,
														   perspectiveCamera.znear,
														   perspectiveCamera.zfar,
														   perspectiveCamera.yfov);
			aeCamera->aspectRatio(float(perspectiveCamera.aspectRatio));

			return aeCamera;
		}
		else if (type == "orthographic") {

			AE_LOG_W("Orthographic cameras are not supported.");
		}
	}

	return nullptr;
}

mat4 TransformFromGlFTNode(tinygltf::Node& node) {

	// if it has a transformation matrix, use it
	auto& matrix = node.matrix;
	auto matrixSize = matrix.size();
	switch (matrixSize) {

		case 16: {

			float floatMatrix[16];
			for (int i=0; i<matrix.size(); ++i) {
				floatMatrix[i] = float(matrix[i]);
			}

			return make_mat4(&floatMatrix[0]);
		}

		case 0: {

			mat4 t = mat4(1.0);
			mat4 r = mat4(1.0);
			mat4 s = mat4(1.0);

			auto tSize = node.translation.size();
			auto rSize = node.rotation.size();
			auto sSize = node.scale.size();

			if (tSize == 3) {
				t = glm::translate(t, { node.translation[0],
										node.translation[1],
										node.translation[2] });
			}

			if (rSize == 4) {
				r = glm::mat4_cast(quat{ float(node.rotation[3]),
										 float(node.rotation[0]),
										 float(node.rotation[1]),
										 float(node.rotation[2]) });
			}

			if (sSize == 3) {
				s = glm::scale(s, { node.scale[0],
									node.scale[1],
									node.scale[2] });
			}

			return t * r * s;
		}
	}

	return mat4(1.0);
}

shared_ptr<Color> ColorFromGlTFColorVec(vector<double>& vec) {

	auto vecSize = vec.size();
	if (vecSize == 3) {
		return make_shared<Color>(float(vec[0]),
								  float(vec[1]),
								  float(vec[2]));
	}

	return Color::Magenta();
}

static void GetRunTime(double time, // time since reference
					   bool paused,
					   double& runT, // time since reference excluding paused time
					   double& deltaRunT) {//, // time since last call excluding paused time

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

	// *** every 5 seconds something slows a frame down significantly ***
//	if (stats.currentFrametime > 15) {
//		AE_LOG_W("currentFrametime: {}", stats.currentFrametime);
//	}

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

//static shared_ptr<Image> MissingTextureImage() {
//	static shared_ptr<Image> image = nullptr;
//	if (!image) {
//		image = ImageNamed("missing_texture", "png");
//	}
//	return image;
//}
