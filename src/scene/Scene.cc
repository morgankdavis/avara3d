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
												 shared_ptr<Node> parent,
												 const filesystem::path& directory);

static shared_ptr<Geometry>			GeometryFromGlFTNode(fastgltf::Asset& asset,
															fastgltf::Node& node,
															const filesystem::path& directory);

static shared_ptr<GeometryElement>	GeometryElementFromGlFTPrimitive(fastgltf::Asset& asset,
																	   fastgltf::Primitive& primitive);

static shared_ptr<Material>			MaterialFromGlFTPrimitive(fastgltf::Asset& asset,
																 fastgltf::Primitive& primitive,
																 const filesystem::path& directory);

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

	AE_LOG_I("Loading glTF: '{}'...", path.string());

	auto startTime = aeScene.time();

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
					VisitGlTFNode(asset, node, aeScene.rootNode(), directory);
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
				   shared_ptr<Node> parent,
				   const filesystem::path& directory) {

	auto aeNode = Node::NamedNode(string(node.name));

	aeNode->light(LightFromGlTFNode(asset, node));
	aeNode->camera(CameraFromGlTFNode(asset, node));
	aeNode->geometry(GeometryFromGlFTNode(asset, node, directory));
	aeNode->transform(TransformFromGlFTNode(node));

	parent->addChild(aeNode);

	for (auto c : node.children) {
		VisitGlTFNode(asset, asset.nodes[c], aeNode, directory);
	}
}

shared_ptr<Geometry> GeometryFromGlFTNode(fastgltf::Asset& asset,
										  fastgltf::Node& node,
										  const filesystem::path& directory) {

	if (auto meshIndex = node.meshIndex) {

		auto& mesh = asset.meshes[*meshIndex];

		auto elements = vector<shared_ptr<GeometryElement>>();
		auto materials = vector<shared_ptr<Material>>();

		AE_LOG_D("mesh.primitives.size(): {}", mesh.primitives.size()); // primitive = geometry element
		for (auto& primitive : mesh.primitives) {


			auto element = GeometryElementFromGlFTPrimitive(asset, primitive);
			if (element) elements.push_back(element);


			auto material = MaterialFromGlFTPrimitive(asset, primitive, directory);
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
	int f = 0;



	// *************** crashes on .gltf ********************
//	if (auto indiciesAccessorIndex = primitive.indicesAccessor) {
//
//		vector<uint32_t> indices;
//		if (primitive.indicesAccessor.has_value()) {
//			auto &accessor = asset.accessors[*indiciesAccessorIndex]; // also has materialIndex
//			indices.resize(accessor.count);
//
//			iterateAccessorWithIndex<uint32_t>(
//					asset, accessor, [&](uint32_t index, size_t idx) {
//						indices[idx] = index;
//					});
//
//
////		Vertex* vPtr = reinterpret_cast<Vertex*>(&asset.buffers[0]);
//			for (auto i: indices) {
//
////				AE_LOG_D("i: {}", i);
////			Vertex vert = {};
////			memcpy(&vert.position, &buffer, sizeof(vert.position));
////			verts.push_back(vert);
//
//
////			Face face = {f+0, f+1, f+2};
////			f += 3;
////			faces.push_back(face);
//
//
////			auto element = getAccessorElement(asset, accessor, indices[i]);
//			}
//		}
//	}
//	else {
//		AE_LOG_E("No indiciesAccessorIndex!");
//	}




//	auto positionAttribIt = primitive.findAttribute("POSITION");
//	auto& positionAccessor = asset.accessors[positionAttribIt->second];
//	if (positionAccessor.bufferViewIndex.has_value()) {
//
//		std::size_t idx = 0;
//		for (auto element : iterateAccessor(asset, positionAccessor)) {
//			array[idx++] = element;
//		}
//	}

//	AE_LOG_D("indicies size: {}", indices.size());

	// (3 prim) (pineapple?)
	// indicies size: 1434
	// indicies size: 3675
	// indicies size: 5718

//	2024-01-27 19:13:00.597 [ae] [debug] [main.cc:214] [main()] verts: 1434 *
//	2024-01-27 19:13:00.597 [ae] [debug] [main.cc:215] [main()] faces: 478
//	2024-01-27 19:13:00.597 [ae] [debug] [main.cc:214] [main()] verts: 3675 *
//	2024-01-27 19:13:00.598 [ae] [debug] [main.cc:215] [main()] faces: 1225 ^^ /3
//	2024-01-27 19:13:00.598 [ae] [debug] [main.cc:214] [main()] verts: 5718 *
//	2024-01-27 19:13:00.598 [ae] [debug] [main.cc:215] [main()] faces: 1906



	return make_shared<GeometryElement>(verts, faces);






//	auto primitiveType = primitive.type;
//	if (primitiveType == PrimitiveType::Triangles) {
//
//		auto positionAttribIt = primitive.findAttribute("POSITION");
//		auto normalAttribIt = primitive.findAttribute("NORMAL");
//		auto texcoordAttribIt = primitive.findAttribute("TEXCOORD_0");
//
//
//
//		auto& positionAccessor = asset.accessors[positionAttribIt->second];
//		if (positionAccessor.bufferViewIndex.has_value()) {
//
//			auto& bufferView = asset.bufferViews[*positionAccessor.bufferViewIndex];
//
//			AE_LOG_D("bufferView.bufferIndex: {}", bufferView.bufferIndex); // 0
//			AE_LOG_D("bufferView.byteOffset: {}", bufferView.byteOffset); // 0
//			AE_LOG_D("bufferView.byteLength: {}", bufferView.byteLength); // 27193
//			if (auto stride = bufferView.byteStride) {
//				AE_LOG_D("bufferView.byteStride: {}", *stride);
//			}
//
//			// 4,4,4 * 3 = 48 bytes per vertex?
//
//			auto& buffer = asset.buffers[bufferView.bufferIndex];
//
//
//			Vertex vert = {};
//			memcpy(&vert.position, &buffer, sizeof(vert.position));
//			verts.push_back(vert);
//
//
//			Face face = {f+0, f+1, f+2};
//			f += 3;
//			faces.push_back(face);
//
//
//
////			AccessorType accessorType = positionAccessor.type;
////			ComponentType accessorComponantType = positionAccessor.componentType;
////
////			if (accessorType == AccessorType::Vec2) {
////				AE_LOG_D("Vec2");
////			}
////			else if (accessorType == AccessorType::Vec3) {
////				AE_LOG_D("Vec3"); // yes
////			}
////
////			if (accessorComponantType == ComponentType::UnsignedInt) {
////				AE_LOG_D("UnsignedInt");
////			}
////			if (accessorComponantType == ComponentType::UnsignedShort) {
////				AE_LOG_D("UnsignedShort");
////			}
//////			if (accessorComponantType != ComponentType::Float) {
//////				AE_LOG_E("!Float");
//////			}
////
////			AE_LOG_D("accessorType: {}", static_cast<underlying_type<AccessorType>::type>(accessorType));
////			AE_LOG_D("accessorComponantType: {}", static_cast<underlying_type<ComponentType>::type>(accessorComponantType));
////
////			AE_LOG_D("accessorType: {}", magic_enum::enum_name<AccessorType>(accessorType));
////			AE_LOG_D("accessorComponantType: {}", magic_enum::enum_name<ComponentType>(accessorComponantType));
////
////			auto typeNumComponents = getNumComponents(accessorType);
////			auto glComponantType = getGLComponentType(accessorComponantType);
////
////			AE_LOG_D("typeNumComponents: {}", typeNumComponents); // 3 * Vec3<float> ?
////			AE_LOG_D("glComponantType: {}", glComponantType); // 5126 = GL_FLOAT
////
////			auto& positionView = asset.bufferViews[*positionAccessor.bufferViewIndex];
////			auto offset = positionView.byteOffset + positionAccessor.byteOffset;
////
//////			GLuint vaobj,
//////			GLuint bindingindex,
//////			GLuint buffer,
//////			GLintptr offset,
//////			GLsizei stride);
//////			glVertexArrayVertexBuffer(vao, 0, viewer->buffers[positionView.bufferIndex],
//////									  static_cast<GLintptr>(offset),
//////									  static_cast<GLsizei>(positionView.byteStride.value()));
////
////			auto buffer = asset.buffers[positionView.bufferIndex];
////			//auto offset = offset;
////			auto stride = *positionView.byteStride;
////
////			AE_LOG_D("stride: {}", stride);
////
////			Vertex vert = {};
////			memcpy(&vert.position, &buffer, sizeof(vert.position));
////			verts.push_back(vert);
////
////			static int f = 0;
////			Face face = {f+0, f+1, f+2};
////			f += 3;
////			faces.push_back(face);
//
//		}
//		else {
//			AE_LOG_W("No position data!");
//		}
//
//
//		for (auto& attribute : primitive.attributes) {
//
////				AE_LOG_W("attribute type: {}",
////						 magic_enum::enum_name<Primitive::attribute_type>(a.first));
//			AE_LOG_D("attribute.first: {}", attribute.first); // POSITION, NORMAL, TEXCOORD_0
//			AE_LOG_D("attribute.second: {}", attribute.second); // 100, 101, 102
//
//			// POSITION = name
//			// 100 = "corresponding accessor index"
//		}
//
//
//
//
//
////		vector<uint32_t> indices;
////		if (primitive.indicesAccessor.has_value()) {
////			auto& accessor = asset.accessors[*primitive.indicesAccessor]; // also has materialIndex
////			indices.resize(accessor.count);
////
////			fastgltf::iterateAccessorWithIndex<uint32_t>(
////					asset, accessor, [&](uint32_t index, size_t idx) {
////						indices[idx] = index;
////					});
////
////
////		}
//
//
//	}
//	else {
//		AE_LOG_W("Unsupported primitive type: {}",
//				 magic_enum::enum_name<fastgltf::PrimitiveType>(primitiveType));
//	}
//
//	return make_shared<GeometryElement>(verts, faces);


	return nullptr;
}

shared_ptr<Material> MaterialFromGlFTPrimitive(fastgltf::Asset& asset,
											   fastgltf::Primitive& primitive,
											   const filesystem::path& directory) {

	using namespace fastgltf;

	if (auto materialIndex = primitive.materialIndex) {

		auto& material = asset.materials[*materialIndex];

		AE_LOG_D("material.name: {}", material.name);

		auto& pbrData = material.pbrData;
		if (pbrData.baseColorTexture) {

			if (auto baseColorTextureIndex = (*pbrData.baseColorTexture).textureIndex) {
				auto& texture = asset.textures[baseColorTextureIndex];

				if (auto imageIndex = texture.imageIndex) {

					auto& image = asset.images[*imageIndex];

					auto& dataSource = image.data;

					if (holds_alternative<sources::BufferView>(dataSource)) { // .glb
						AE_LOG_D("BufferView");

						auto bufferViewIndex = get<sources::BufferView>(dataSource).bufferViewIndex;
						auto& bufferView = asset.bufferViews[bufferViewIndex];

						//AE_LOG_D("bufferIndex: {}", bufferView.bufferIndex);
						//AE_LOG_D("byteOffset: {}", bufferView.byteOffset);
						//AE_LOG_D("byteLength: {}", bufferView.byteLength);
						if (auto byteStride = bufferView.byteStride) {
							//AE_LOG_D("byteStride: {}", *(bufferView.byteStride));
						}

						auto buffer = asset.buffers[bufferView.bufferIndex];
						auto bufferData = buffer.data;

						if (holds_alternative<sources::Vector>(bufferData)) {
							AE_LOG_D("Vector");
							// apparently this can happen:
							// https://github.com/spnda/fastgltf/blob/0272e598eed28632ba7e9cb8a55ce0f8a25da1ea/examples/gl_viewer/gl_viewer.cpp#L458
						}
						else if (holds_alternative<sources::ByteView>(bufferData)) {
							AE_LOG_D("ByteView");

							auto byteView = get<sources::ByteView>(bufferData);
							span<const std::byte> bytes = byteView.bytes;

							auto pointer = bytes.data();
							auto sizeBytes = bytes.size_bytes();

							AE_LOG_D("loading image buffer for: {}", material.name);

							auto imageData = reinterpret_cast<const unsigned char*>(pointer);
							size_t imageDataSize = sizeBytes;
							auto aeBuffer = make_shared<ae::Buffer>(imageData, imageDataSize);
							AE_LOG_D("imageDataSize: {} KB", imageDataSize/1024);


//							auto aeImage = make_shared<Image>(aeBuffer);

							return ae::Material::DefaultMaterial();

//							auto property = make_shared<MaterialProperty>(aeImage);
//							return make_shared<Material>(property, property, nullptr);
						}
					}
					else if (holds_alternative<sources::URI>(dataSource)) { // .gltf
						AE_LOG_D("URI");

						auto uri = get<sources::URI>(dataSource);

						//AE_LOG_D("URL: {}", uri.uri.string()); // ex: 'textures/pineapple_diffuse1.jpg'

						auto fullpath = directory / uri.uri.string();
						//AE_LOG_D("fullpath: {}", fullpath.string());
						auto image = make_shared<ae::Image>(fullpath);
						auto property = make_shared<MaterialProperty>(image);
						return make_shared<ae::Material>(property, property, nullptr);
					}
				}
			}
		}
	}

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
