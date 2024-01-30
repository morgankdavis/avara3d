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
#include <variant>

#include "fastgltf/parser.hpp"
#include "fastgltf/tools.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/util.hpp"
#include "fmt/format.h"
#include "glm/gtc/type_ptr.hpp"
#include "magic_enum.hpp"

#include "ae/Buffer.h"
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

static shared_ptr<Material>			MaterialFromGlFTPrimitive(fastgltf::Asset& asset,
																 fastgltf::Primitive& primitive);

static shared_ptr<Light>			LightFromGlTFNode(fastgltf::Asset& asset,
													  fastgltf::Node& node);

static shared_ptr<Camera>			CameraFromGlTFNode(fastgltf::Asset& asset,
														fastgltf::Node& node);

static mat4							TransformFromGlFTNode(fastgltf::Node& node);

static shared_ptr<Color> 			ColorFromGlTFColorArray(array<float, 3>& arr);

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

	AE_LOG_I("Loading glTF: '{}'...", path.string());

	auto startTime = aeScene.time();

	using namespace fastgltf;

	GltfDataBuffer data;
	data.loadFromFile(path);

	auto parser = Parser(Extensions::KHR_lights_punctual);
	auto expectedAsset = Expected<Asset>(Error::None);

	auto options = Options::LoadGLBBuffers
			| Options::LoadExternalBuffers
			| Options::LoadExternalImages
			| Options::GenerateMeshIndices;

	auto extension = path.extension();
	if (extension == ".glb") {
		expectedAsset = parser.loadBinaryGLTF(&data, path.parent_path(), options);
	}
	else if (extension == ".gltf") {
		expectedAsset = parser.loadGLTF(&data, path.parent_path(), options);
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

			auto directory = path.parent_path();

			auto& scene = scenes[asset.defaultScene.has_value()
								 ? *asset.defaultScene
								 : 0];

			auto nodeIndicies = scene.nodeIndices;
			if (!nodeIndicies.empty()) {

				auto parseTime = aeScene.time() - startTime;
				AE_LOG_D("parseTime: {}", parseTime);

				for (auto n : nodeIndicies) {

					auto node = asset.nodes[n];
					VisitGlTFNode(asset, node, aeScene.rootNode());
				}

				AE_LOG_D("Done loading glTF.");

				auto loadTime = aeScene.time() - startTime;
				AE_LOG_D("loadTime: {}", loadTime);
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

	auto aeNode = Node::NamedNode(string(node.name));

	aeNode->light(LightFromGlTFNode(asset, node));
	aeNode->camera(CameraFromGlTFNode(asset, node));
	aeNode->geometry(GeometryFromGlFTNode(asset, node));
	aeNode->transform(TransformFromGlFTNode(node));

	parent->addChild(aeNode);

	for (auto c : node.children) {
		VisitGlTFNode(asset, asset.nodes[c], aeNode);
	}
}

shared_ptr<Geometry> GeometryFromGlFTNode(fastgltf::Asset& asset,
										  fastgltf::Node& node) {

	if (auto meshIndex = node.meshIndex) {
		auto& mesh = asset.meshes[*meshIndex];

		AE_LOG_D("mesh.name: {}", mesh.name);

		auto elements = vector<shared_ptr<GeometryElement>>();
		auto materials = vector<shared_ptr<Material>>();

		for (auto& primitive : mesh.primitives) {

			auto element = GeometryElementFromGlFTPrimitive(asset, primitive);
			if (element) elements.push_back(element);

			auto material = MaterialFromGlFTPrimitive(asset, primitive);
			if (material) materials.push_back(material);
		}

		auto geometry = make_shared<Geometry>(elements, materials);
		geometry->name(string(mesh.name));
		return geometry;
	}

	return nullptr;
}

shared_ptr<GeometryElement> GeometryElementFromGlFTPrimitive(fastgltf::Asset& asset,
															 fastgltf::Primitive& primitive) {

	using namespace fastgltf;

	vector<Vertex> verts;
	vector<Face> faces;

	if (auto indiciesAccessorIndex = primitive.indicesAccessor) {

		if (primitive.indicesAccessor.has_value()) {
			auto &accessor = asset.accessors[*indiciesAccessorIndex];

			vector<uint32_t> indices;
			indices.resize(accessor.count);

			iterateAccessorWithIndex<uint32_t>(
					asset, accessor, [&](uint32_t index, size_t idx) {
						indices[idx] = index;
					});

			for (size_t i=0; i<indices.size(); i+=3) {
				faces.push_back({ int(indices[i+0]),
								  int(indices[i+1]),
								  int(indices[i+2]) });
			}



			// PUT BELOW HERE

		}
	}



	{
		auto attrib = primitive.findAttribute("POSITION");

		auto accessorIndex = attrib->second;
		auto accessor = asset.accessors[accessorIndex];

		auto type = accessor.type;
		if (type == AccessorType::Vec2
			|| type == AccessorType::Vec3) {

		}
		else {
			AE_LOG_W("Unsupported accessor type: {}",
					 magic_enum::enum_name<AccessorType>(type));
		}

		auto componentType = accessor.componentType;
		if (componentType == ComponentType::Float) {

		}
		else {
			AE_LOG_W("Unsupported accessor component type: {}",
					 magic_enum::enum_name<ComponentType>(componentType));
		}

		auto numComponants = getNumComponents(type);
//			AE_LOG_D("numComponants: {}", numComponants);

		auto &bufferView = asset.bufferViews[*accessor.bufferViewIndex];
		if (!bufferView.byteStride.has_value()) {

			auto &buffer = asset.buffers[bufferView.bufferIndex];
			auto &bufferData = buffer.data;

			if (auto vec = std::get_if<sources::Vector>(&bufferData)) {

				auto offset = bufferView.byteOffset + accessor.byteOffset;
				auto length = bufferView.byteLength;
				auto elementByteSize = getElementByteSize(type, componentType);
				auto numElements = length / elementByteSize;

				auto pPtr = reinterpret_cast<glm::vec3*>(&vec->bytes[offset]);
				for (size_t p = 0; p < numElements; ++p) {
					verts.push_back({(*pPtr++), {}, {}});
				}
			}
			else {
				AE_LOG_W("Unsupported buffer data.");
			}
		}
		else {
			AE_LOG_W("Accessor has stride.  Skipping.");
		}
	}


	{
		auto attrib = primitive.findAttribute("NORMAL");

		auto accessorIndex = attrib->second;
		auto accessor = asset.accessors[accessorIndex];

		auto type = accessor.type;
		if (type == AccessorType::Vec2
			|| type == AccessorType::Vec3) {

		}
		else {
			AE_LOG_W("Unsupported accessor type: {}",
					 magic_enum::enum_name<AccessorType>(type));
		}

		auto componentType = accessor.componentType;
		if (componentType == ComponentType::Float) {

		}
		else {
			AE_LOG_W("Unsupported accessor component type: {}",
					 magic_enum::enum_name<ComponentType>(componentType));
		}

		auto numComponants = getNumComponents(type);
//			AE_LOG_D("numComponants: {}", numComponants);

		auto &bufferView = asset.bufferViews[*accessor.bufferViewIndex];
		if (!bufferView.byteStride.has_value()) {

			auto &buffer = asset.buffers[bufferView.bufferIndex];
			auto &bufferData = buffer.data;

			if (auto vec = std::get_if<sources::Vector>(&bufferData)) {
//					AE_LOG_D("Vector");

				auto offset = bufferView.byteOffset + accessor.byteOffset;
				auto length = bufferView.byteLength;
				auto elementByteSize = getElementByteSize(type, componentType);
				auto numElements = length / elementByteSize;

//					AE_LOG_D("offset: {}", offset);
//					AE_LOG_D("length: {}", length);
//					AE_LOG_D("elementByteSize: {}", elementByteSize);
//					AE_LOG_D("numElements: {}", numElements);

				auto nPtr = reinterpret_cast<glm::vec3*>(&vec->bytes[offset]);
				for (size_t n = 0; n < numElements; ++n) {
					verts[n].normal = *(nPtr++);
				}
			}
			else {
				AE_LOG_W("Unsupported buffer data.");
			}
		}
		else {
			AE_LOG_W("Accessor has stride.  Skipping.");
		}
	}


	{
		auto attrib = primitive.findAttribute("TEXCOORD_0");

		auto accessorIndex = attrib->second;
		auto accessor = asset.accessors[accessorIndex];

		auto type = accessor.type;
		if (type == AccessorType::Vec2
			|| type == AccessorType::Vec3) {

		}
		else {
			AE_LOG_W("Unsupported accessor type: {}",
					 magic_enum::enum_name<AccessorType>(type));
		}

		auto componentType = accessor.componentType;
		if (componentType == ComponentType::Float) {

		}
		else {
			AE_LOG_W("Unsupported accessor component type: {}",
					 magic_enum::enum_name<ComponentType>(componentType));
		}

		auto numComponants = getNumComponents(type);
//			AE_LOG_D("numComponants: {}", numComponants);

		auto &bufferView = asset.bufferViews[*accessor.bufferViewIndex];
		if (!bufferView.byteStride.has_value()) {

			auto &buffer = asset.buffers[bufferView.bufferIndex];
			auto &bufferData = buffer.data;

			if (auto vec = std::get_if<sources::Vector>(&bufferData)) {
//					AE_LOG_D("Vector");

				auto offset = bufferView.byteOffset + accessor.byteOffset;
				auto length = bufferView.byteLength;
				auto elementByteSize = getElementByteSize(type, componentType);
				auto numElements = length / elementByteSize;

//					AE_LOG_D("offset: {}", offset);
//					AE_LOG_D("length: {}", length);
//					AE_LOG_D("elementByteSize: {}", elementByteSize);
//					AE_LOG_D("numElements: {}", numElements);

				auto cPtr = reinterpret_cast<glm::vec2*>(&vec->bytes[offset]);
				for (size_t c = 0; c < numElements; ++c) {
					verts[c].texCoord = *(cPtr++);
				}
			}
			else {
				AE_LOG_W("Unsupported buffer data.");
			}
		}
		else {
			AE_LOG_W("Accessor has stride.  Skipping.");
		}
	}







	return make_shared<GeometryElement>(verts, faces);


	return nullptr;
}

shared_ptr<Material> MaterialFromGlFTPrimitive(fastgltf::Asset& asset,
											   fastgltf::Primitive& primitive) {

	using namespace fastgltf;

	if (auto materialIndex = primitive.materialIndex) {

		auto& material = asset.materials[*materialIndex];

		if (auto& pbrData = material.pbrData; pbrData.baseColorTexture) {

			if (auto baseColorTextureIndex = (*pbrData.baseColorTexture).textureIndex) {

				auto& texture = asset.textures[baseColorTextureIndex];

				if (auto imageIndex = texture.imageIndex) {

					auto& image = asset.images[*imageIndex];

					auto& dataSource = image.data;
					if (holds_alternative<sources::Vector>(dataSource)) { // .gltf

//						AE_LOG_D("Loading .gltf texture buffer...");

						auto uint8Vec = get<sources::Vector>(dataSource).bytes;
						auto aeBuffer = make_shared<ae::Buffer>(uint8Vec.data(), uint8Vec.size());
						auto aeImage = make_shared<ae::Image>(aeBuffer);//, true);
						auto property = make_shared<MaterialProperty>(aeImage);
						return make_shared<ae::Material>(property, property, nullptr);
					}
					else if (holds_alternative<sources::BufferView>(dataSource)) { // .glb

						auto bufferViewIndex = get<sources::BufferView>(dataSource).bufferViewIndex;

						auto& bufferView = asset.bufferViews[bufferViewIndex];

						if (auto byteStride = bufferView.byteStride) {
							AE_LOG_W("Texture buffer has stride: {}.  Skipping.", *(bufferView.byteStride));
							return ae::Material::MissingTextureMaterial();
						}
						else {
//							AE_LOG_D("Loading .glb texture buffer...");

							auto buffer = asset.buffers[bufferView.bufferIndex];
							auto byteOffset = bufferView.byteOffset;
							auto byteLength = bufferView.byteLength;

							auto bufferData = buffer.data;
							if (holds_alternative<sources::Vector>(bufferData)) {

								auto uint8Vec = get<sources::Vector>(bufferData).bytes;
								auto aeBuffer = make_shared<ae::Buffer>(&uint8Vec[byteOffset], byteLength);

								auto aeImage = make_shared<ae::Image>(aeBuffer);//, true);
								auto property = make_shared<MaterialProperty>(aeImage);
								return make_shared<ae::Material>(property, property, nullptr);
							}
						}
					}
					else {
						AE_LOG_W("Unexpected texture data.  Skipping.");
						return ae::Material::MissingTextureMaterial();
					}
				}
			}
		}
	}

	return ae::Material::DefaultMaterial();
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
															: 1000000), // cheating
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

		auto r = glm::mat4_cast(quat{ trs.rotation[3],
									  trs.rotation[0],
									  trs.rotation[1],
									  trs.rotation[2] });

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

void GetRunTime(double time, // time since reference
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
