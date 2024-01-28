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

#include "fastgltf/parser.hpp"
#include "fastgltf/tools.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/util.hpp"
#include "fmt/format.h"
#include "glm/gtc/type_ptr.hpp"
#include "magic_enum.hpp"
//#define TINYGLTF_IMPLEMENTATION
//#include "tiny_gltf.h"

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

static void 						VisitGlTFNode(fastgltf::Asset& asset,
												 fastgltf::Node& node,
												 shared_ptr<Node> parent);

static shared_ptr<Geometry>			GeometryFromGlFTNode(fastgltf::Asset& asset,
															fastgltf::Node& node);

static shared_ptr<GeometryElement>	GeometryElementFromGlFTPrimitive(fastgltf::Asset& asset,
																	   fastgltf::Primitive& primitive);

static shared_ptr<Material>			MaterialFromGlFTMaterial(fastgltf::Asset& asset,
																fastgltf::Material& material);

static shared_ptr<Light>			LightFromGlTFNode(fastgltf::Asset& asset,
													  fastgltf::Node& node);

static shared_ptr<Camera>			CameraFromGlTFNode(fastgltf::Asset& asset,
														fastgltf::Node& node);

static mat4							TransformFromGlFTNode(fastgltf::Node& node);

static shared_ptr<Color> 			ColorFromGlTFColorArray(array<float, 3>& arr);


//static void 						LoadGlTF(Scene& aeScene, const filesystem::path& path);
//static void 						VisitGlTFNode(tinygltf::Model& model,
//												 tinygltf::Node& node,
//												 shared_ptr<Node> parent);
//static shared_ptr<Geometry>			GeometryFromGlFTNode(tinygltf::Model& model,
//															tinygltf::Node& node);
//static shared_ptr<GeometryElement>	GeometryElementFromGlFTPrimitive(tinygltf::Model& model,
//																	   tinygltf::Primitive& primitive);
//static shared_ptr<Material>			MaterialFromGlFTMaterial(tinygltf::Model& model,
//																tinygltf::Material& material);
//static shared_ptr<Light>			LightFromGlTFNode(tinygltf::Model& model,
//													  tinygltf::Node& node);
//static shared_ptr<Camera>			CameraFromGlTFNode(tinygltf::Model& model,
//														tinygltf::Node& node);
//static mat4							TransformFromGlFTNode(tinygltf::Node& node);
//static shared_ptr<Color>			ColorFromGlTFColorVec(vector<double>& vec);

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

	using namespace fastgltf;

	GltfDataBuffer data;
	data.loadFromFile(path);

	auto parser = Parser(Extensions::KHR_lights_punctual);
	auto expectedAsset = Expected<Asset>(Error::None);

	auto extension = path.extension();
	if (extension == ".glb") {
		expectedAsset = parser.loadBinaryGLTF(&data, path.parent_path(), Options::None);
	}
	else if (extension == ".gltf") {
		expectedAsset = parser.loadGLTF(&data, path.parent_path(), Options::None);
	}
	else {
		AE_LOG_E("Unsupported file extension: {}", extension.string());
	}

	if (auto error = expectedAsset.error(); error != Error::None) {
		AE_LOG_E("Error parsing glTF file: {}", magic_enum::enum_name<Error>(error));
	}
	else {

		auto& asset = expectedAsset.get();

		if (auto& info = asset.assetInfo) {
			AE_LOG_D("glTF parsed. version: {}, copyright: {}, generator: {}",
					 info->gltfVersion, info->copyright, info->generator);
		}
		else {
			AE_LOG_D("glTF parsed.");
		}

		auto& scenes = asset.scenes;
		if (!scenes.empty()) {

			if (scenes.size() > 1) {
				AE_LOG_W("Ignoring extra scenes.");
			}

			auto& scene = scenes[asset.defaultScene.has_value()
								 ? *asset.defaultScene
								 : 0];

			auto nodeIndicies = scene.nodeIndices;
			if (!nodeIndicies.empty()) {

				for (auto n : nodeIndicies) {

					AE_LOG_D("Visiting node {}...", n);

					auto node = asset.nodes[n];
					VisitGlTFNode(asset, node, aeScene.rootNode());
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
}

void VisitGlTFNode(fastgltf::Asset& asset,
				   fastgltf::Node& node,
				   shared_ptr<Node> parent) {

	auto name = node.name;
	AE_LOG_D("name: {}", name);
	auto aeNode = Node::NamedNode(string(name));

	aeNode->light(LightFromGlTFNode(asset, node));
	aeNode->camera(CameraFromGlTFNode(asset, node));
	aeNode->geometry(GeometryFromGlFTNode(asset, node));
	aeNode->transform(TransformFromGlFTNode(node));

	AE_LOG_D("position: {}", StringFromGLMVec3(aeNode->position()));
	AE_LOG_D("rotation: {}", StringFromGLMVec4(aeNode->rotation()));
	AE_LOG_D("scale: {}", StringFromGLMVec3(aeNode->scale()));

	if (aeNode->camera()) {
		AE_LOG_D("camera.name: {}", *aeNode->camera()->name());
	}
	if (aeNode->light()) {
		AE_LOG_D("light.name: {}", *aeNode->light()->name());
	}

	parent->addChild(aeNode);

	for (auto c : node.children) {
		VisitGlTFNode(asset, asset.nodes[c], aeNode);
	}
}

shared_ptr<Geometry> GeometryFromGlFTNode(fastgltf::Asset& asset,
										  fastgltf::Node& node) {

	return nullptr;
}

shared_ptr<GeometryElement> GeometryElementFromGlFTPrimitive(fastgltf::Asset& asset,
															 fastgltf::Primitive& primitive) {

	return nullptr;
}

shared_ptr<Material> MaterialFromGlFTMaterial(fastgltf::Asset& asset,
											  fastgltf::Material& material) {

	return nullptr;
}

shared_ptr<Light> LightFromGlTFNode(fastgltf::Asset& asset,
									fastgltf::Node& node) {

	if (auto lightIndex = node.lightIndex) {

		auto& light = asset.lights[*lightIndex];
		auto& type = light.type;

		if (type == fastgltf::LightType::Point) {

			auto aeLight = make_shared<Light>(LIGHT_TYPE::POINT);
			aeLight->name(string(light.name));
			aeLight->attenuationFactor(0); // temporary
			aeLight->color(ColorFromGlTFColorArray(light.color));
//			// TODO: range, intensity

			return aeLight;
		}
		else {

			AE_LOG_W("Unsupported light type: {}",
					 magic_enum::enum_name<fastgltf::LightType>(type));
		}
	}

	return nullptr;
}

shared_ptr<Camera> CameraFromGlTFNode(fastgltf::Asset& asset,
									  fastgltf::Node& node) {

	if (auto cameraIndex = node.cameraIndex) {

		auto& camera = asset.cameras[*cameraIndex];

		auto cameraVar = camera.camera;
		if (holds_alternative<fastgltf::Camera::Perspective>(cameraVar)) {

			auto perspective = get<fastgltf::Camera::Perspective>(cameraVar);

			auto aeCamera = make_shared<PerspectiveCamera>(string(camera.name),
														   perspective.znear,
														   (perspective.zfar.has_value()
															? *perspective.zfar
															: 100000), // cheating
														   perspective.yfov);

			if (auto ratio = perspective.aspectRatio) {
				aeCamera->aspectRatio(*ratio);
			}

			return aeCamera;
		}
		else if (holds_alternative<fastgltf::Camera::Orthographic>(cameraVar)) {

			AE_LOG_W("Orthographic cameras are not supported.");
		}
	}

	return nullptr;
}

mat4 TransformFromGlFTNode(fastgltf::Node& node) {

	auto transform = node.transform;
	if (holds_alternative<fastgltf::Node::TRS>(transform)) {

		auto trs = get<fastgltf::Node::TRS>(transform);
		const auto& id4 = mat4(1.0);

		auto t = glm::translate(id4, { trs.translation[0],
									   trs.translation[1],
									   trs.translation[2] });

		auto r = glm::mat4_cast(quat{ trs.rotation[0],
									  trs.rotation[1],
									  trs.rotation[2],
									  trs.rotation[3] });

		auto s = glm::scale(id4, { trs.scale[0],
								   trs.scale[1],
								   trs.scale[2] });

		return t * r * s;

	}
	else if (holds_alternative<fastgltf::Node::TransformMatrix>(transform)) {

		auto matrix = get<fastgltf::Node::TransformMatrix>(transform);
		return make_mat4(&matrix[0]);

	}
	else {
		AE_LOG_W("No transform associated with node: {}", node.name);
	}

	return mat4(1.0);
}

shared_ptr<Color> ColorFromGlTFColorArray(array<float, 3>& arr) {

	return make_shared<Color>(arr[0], arr[1], arr[2]);
}

//void LoadGlTF(Scene& aeScene, const filesystem::path& path) {
//
//	using namespace tinygltf;
//
//	TinyGLTF loader;
//	Model model;
//	string error;
//	string warning;
//
//	auto extension = path.extension();
//	bool res = false;
//	if (extension == ".glb") {
//		res = loader.LoadBinaryFromFile(&model, &error, &warning, path.string());
//	}
//	else if (extension == ".gltf") {
//		res = loader.LoadASCIIFromFile(&model, &error, &warning, path.string());
//	}
//	else {
//		AE_LOG_E("Unsupported file extension: {}", extension.string());
//	}
//
//	if (res) {
//
//		if (!warning.empty()) {
//			AE_LOG_W("Warning loading glTF: {}", warning);
//		}
//
//		auto& scenes = model.scenes;
//		if (!scenes.empty()) {
//
//			if (scenes.size() > 1) {
//				AE_LOG_W("Ignoring additional scenes.");
//			}
//
//			// example uses: 'model.scenes[model.defaultScene > -1 ? model.defaultScene : 0]'
//			auto& scene = scenes[0];
//			auto& nodes = scene.nodes;
//
//			if (!nodes.empty()) {
//
//				for (auto n : scene.nodes) {
//					AE_LOG_D("Visiting node {}...", n);
//
//					tinygltf::Node& node = model.nodes[n];
//
//					VisitGlTFNode(model, node, aeScene.rootNode());
//				}
//
//				AE_LOG_D("Done loading glTF.");
//			}
//			else {
//				AE_LOG_W("No nodes in scene: {}", scene.name);
//			}
//		}
//		else {
//			AE_LOG_E("No scenes.");
//		}
//	}
//	else {
//
//		if (!error.empty()) {
//			AE_LOG_E("Error loading glTF: {}", error);
//		}
//		else {
//			AE_LOG_E("Unknown error loading glTF.");
//		}
//	}
//}
//
//void VisitGlTFNode(tinygltf::Model& model,
//				   tinygltf::Node& node,
//				   shared_ptr<Node> parent) {
//
//	auto aeNode = Node::NamedNode(node.name);
//
//	aeNode->light(LightFromGlTFNode(model, node));
//	aeNode->camera(CameraFromGlTFNode(model, node));
//	aeNode->geometry(GeometryFromGlFTNode(model, node));
//	aeNode->transform(TransformFromGlFTNode(node));
//
//	parent->addChild(aeNode);
//
//	for (auto c : node.children) {
//		VisitGlTFNode(model, model.nodes[c], aeNode);
//	}
//}
//
//shared_ptr<Geometry> GeometryFromGlFTNode(tinygltf::Model& model,
//										  tinygltf::Node& node) {
//
//	auto meshIndex = node.mesh;
//	if (meshIndex > -1) {
//
//		auto& mesh = model.meshes[meshIndex];
//		auto& name = mesh.name;
//		auto& primitives = mesh.primitives;
//
//		auto aeElements = vector<shared_ptr<GeometryElement>>();
//		auto aeMaterials = vector<shared_ptr<Material>>();
//
//		AE_LOG_D("mesh name: {}, primitives.size(): {}", name, primitives.size());
//
//		for (auto& primitive : primitives) {
//
//			auto aeElement = GeometryElementFromGlFTPrimitive(model, primitive);
//			aeElements.push_back(aeElement);
//
////			auto materialIndex = primitive.material;
////			if (materialIndex > -1) {
////				auto& material = model.materials[materialIndex];
////				auto aeMaterial = MaterialFromGlFTMaterial(model, material);
////				aeMaterials.push_back(aeMaterial);
////			}
//		}
//
////		auto aeGeometry = make_shared<Geometry>(aeElements, aeMaterials);
////		aeGeometry->name(mesh.name);
////		return aeGeometry;
//	}
//
//	return nullptr;
//}
//
//shared_ptr<GeometryElement> GeometryElementFromGlFTPrimitive(tinygltf::Model& model,
//															 tinygltf::Primitive& primitive) {
//
//	auto verts = vector<Vertex>();
//	auto faces = vector<Face>();
//
//	for (pair<string, int> item : primitive.attributes) {
//		AE_LOG_D("primitive: ({}, {})", item.first, item.second);
//		// POSITION, NORMAL, TEXCOORD_0
//
//		auto mode = primitive.mode;
//		if (mode == TINYGLTF_MODE_TRIANGLES) {
//
//			auto indiciesIndex = primitive.indices;
//			if (indiciesIndex > -1) {
//				auto accessor = model.accessors[indiciesIndex];
//
//				auto type = accessor.type;
//				if (type == TINYGLTF_TYPE_SCALAR) {
//
//					auto componentType = accessor.componentType;
//					if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
//
//						auto count = accessor.count;
//						AE_LOG_D("count: {}", count);
//
//						auto bufferViewIndex = accessor.bufferView;
//						if (bufferViewIndex > -1) {
//
//							auto& bufferView = model.bufferViews[bufferViewIndex];
//
//							auto bufferIndex = bufferView.buffer;
//							if (bufferIndex > -1) {
//
//								auto& buffer = model.buffers[bufferIndex];
//
//
//
//
//							}
//							else {
//								AE_LOG_W("Missing Buffer.");
//							}
//						}
//						else {
//							AE_LOG_W("Missing BufferView.");
//						}
//					}
//					else {
//						AE_LOG_W("Unsupported glTF accessor component type: {}", mode);
//					}
//				}
//				else {
//					AE_LOG_W("Unsupported glTF accessor type: {}", mode);
//				}
//			}
//		}
//		else {
//			AE_LOG_W("Unsupported glTF primitive type: {}", mode);
//		}
//	}
//
//	return nullptr;
//}
//
//shared_ptr<Material> MaterialFromGlFTMaterial(tinygltf::Model& model,
//											  tinygltf::Material& material) {
//
//	return nullptr;
//}
//
//shared_ptr<Light> LightFromGlTFNode(tinygltf::Model& model,
//									tinygltf::Node& node) {
//
//	auto index = node.light;
//	if (index > -1) {
//
//		auto& light = model.lights[index];
//		auto& type = light.type;
//
//		if (type == "point") {
//
//			auto aeLight = make_shared<Light>(LIGHT_TYPE::POINT);
//			aeLight->name(light.name);
////			aeLight->attenuationFactor(float(light.intensity)); // not the same thing
//			aeLight->attenuationFactor(0); // temporary
//			aeLight->color(ColorFromGlTFColorVec(light.color));
////			// TODO: range, intensity
//
//			return aeLight;
//		}
//		else {
//
//			AE_LOG_W("Unsupported light type: {}", type);
//		}
//	}
//
//	return nullptr;
//}
//
//shared_ptr<Camera> CameraFromGlTFNode(tinygltf::Model& model,
//									  tinygltf::Node& node) {
//
//	auto index = node.camera;
//	if (index > -1) {
//
//		auto& camera = model.cameras[index];
//		auto& type = camera.type;
//
//		if (type == "perspective") {
//
//			auto& perspectiveCamera = camera.perspective;
//
//			auto aeCamera = make_shared<PerspectiveCamera>(camera.name,
//														   perspectiveCamera.znear,
//														   perspectiveCamera.zfar,
//														   perspectiveCamera.yfov);
//			aeCamera->aspectRatio(float(perspectiveCamera.aspectRatio));
//
//			return aeCamera;
//		}
//		else if (type == "orthographic") {
//
//			AE_LOG_W("Orthographic cameras are not supported.");
//		}
//	}
//
//	return nullptr;
//}
//
//mat4 TransformFromGlFTNode(tinygltf::Node& node) {
//
//	// if it has a transformation matrix, use it
//	auto& matrix = node.matrix;
//	auto matrixSize = matrix.size();
//	switch (matrixSize) {
//
//		case 16: {
//
//			float floatMatrix[16];
//			for (int i=0; i<matrix.size(); ++i) {
//				floatMatrix[i] = float(matrix[i]);
//			}
//
//			return make_mat4(&floatMatrix[0]);
//		}
//
//		case 0: {
//
//			mat4 t = mat4(1.0);
//			mat4 r = mat4(1.0);
//			mat4 s = mat4(1.0);
//
//			auto tSize = node.translation.size();
//			auto rSize = node.rotation.size();
//			auto sSize = node.scale.size();
//
//			if (tSize == 3) {
//				t = glm::translate(t, { node.translation[0],
//										node.translation[1],
//										node.translation[2] });
//			}
//
//			if (rSize == 4) {
//				r = glm::mat4_cast(quat{ float(node.rotation[3]),
//										 float(node.rotation[0]),
//										 float(node.rotation[1]),
//										 float(node.rotation[2]) });
//			}
//
//			if (sSize == 3) {
//				s = glm::scale(s, { node.scale[0],
//									node.scale[1],
//									node.scale[2] });
//			}
//
//			return t * r * s;
//		}
//	}
//
//	return mat4(1.0);
//}
//
//shared_ptr<Color> ColorFromGlTFColorVec(vector<double>& vec) {
//
//	auto vecSize = vec.size();
//	if (vecSize == 3) {
//		return make_shared<Color>(float(vec[0]),
//								  float(vec[1]),
//								  float(vec[2]));
//	}
//
//	return Color::Magenta();
//}

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
