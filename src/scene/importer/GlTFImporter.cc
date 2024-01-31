//
// Created by mkd on 1/30/24.
//

#include "ae/scene/importer/GlTFImporter.h"


using namespace ae;
using namespace fastgltf;
using namespace glm;
using namespace std;


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
#include "ae/rendering/camera/Camera.h"
#include "ae/rendering/camera/PerspectiveCamera.h"
#include "ae/rendering/context/RenderContext.h"
#include "ae/rendering/materials/Material.h"
#include "ae/rendering/materials/MaterialProperty.h"
#include "ae/scene/Node.h"
#include "ae/scene/Scene.h"


GlTFImporter::GlTFImporter(const filesystem::path& path):
		_scene{nullptr},
		_path{path},
		_cameras{},
		_geometries{},
		_geometryElements{},
		_images{},
		_lights{},
		_materials{},
		_materialProperties{} {}

shared_ptr<ae::Scene> GlTFImporter::scene() {

	if (!_scene) {
		_scene = load();
	}
	return _scene;
}

shared_ptr<ae::Scene> GlTFImporter::load() {

	AE_LOG_I("Loading glTF: '{}'...", _path.string());

	auto aeScene = make_shared<ae::Scene>();

	auto startTime = aeScene->time();

	GltfDataBuffer data;
	data.loadFromFile(_path);

	auto parser = Parser(Extensions::KHR_lights_punctual);
	auto expectedAsset = Expected<Asset>(Error::None);

	auto options = Options::LoadGLBBuffers
				   | Options::LoadExternalBuffers
				   | Options::LoadExternalImages
				   | Options::GenerateMeshIndices;

	auto extension = _path.extension();
	auto directory = _path.parent_path();
	if (extension == ".glb") {
		expectedAsset = parser.loadBinaryGLTF(&data, directory, options);
	}
	else if (extension == ".gltf") {
		expectedAsset = parser.loadGLTF(&data, directory, options);
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

			auto directory = _path.parent_path();

			auto& scene = scenes[asset.defaultScene.has_value()
								 ? *asset.defaultScene
								 : 0];

			auto nodeIndicies = scene.nodeIndices;
			if (!nodeIndicies.empty()) {

				for (auto n : nodeIndicies) {
					auto node = asset.nodes[n];
					visitGlTFNode(asset, node, aeScene->rootNode());
				}

				AE_LOG_D("Done loading glTF.  Time: {}",  aeScene->time() - startTime);

				return aeScene;
			}
			else {
				AE_LOG_W("No nodes in scene: {}", scene.name);
			}
		}
		else {
			AE_LOG_E("No scenes.");
		}
	}

	return nullptr;
}

void GlTFImporter::visitGlTFNode(fastgltf::Asset& asset,
								 fastgltf::Node& node,
								 shared_ptr<ae::Node> parent) {

	auto aeNode = Node::NamedNode(string(node.name));

	aeNode->light(lightFromGlTFNode(asset, node));
	aeNode->camera(cameraFromGlTFNode(asset, node));
	aeNode->geometry(geometryFromGlFTNode(asset, node));
	aeNode->transform(transformFromGlFTNode(node));

	parent->addChild(aeNode);

	for (auto c : node.children) {
		visitGlTFNode(asset, asset.nodes[c], aeNode);
	}
}

shared_ptr<Geometry> GlTFImporter::geometryFromGlFTNode(fastgltf::Asset& asset,
														fastgltf::Node& node) {

	if (auto meshIndex = node.meshIndex) {
		auto& mesh = asset.meshes[*meshIndex];

		AE_LOG_D("mesh.name: {}", mesh.name);

		auto elements = vector<shared_ptr<GeometryElement>>();
		auto materials = vector<shared_ptr<Material>>();

		for (auto& primitive : mesh.primitives) {

			auto element = geometryElementFromGlFTPrimitive(asset, primitive);
			if (element) elements.push_back(element);

			auto material = materialFromGlFTPrimitive(asset, primitive);
			if (material) materials.push_back(material);
		}

		auto geometry = make_shared<Geometry>(elements, materials);
		geometry->name(string(mesh.name));
		return geometry;
	}

	return nullptr;
}

shared_ptr<ae::GeometryElement> GlTFImporter::geometryElementFromGlFTPrimitive(fastgltf::Asset& asset,
																			   fastgltf::Primitive& primitive) {

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

shared_ptr<ae::Material> GlTFImporter::materialFromGlFTPrimitive(fastgltf::Asset& asset,
																 fastgltf::Primitive& primitive) {

	if (auto materialIndex = primitive.materialIndex) {

		auto& material = asset.materials[*materialIndex];

//		auto& specularMaterial = material.specular; //
//		if (specularMaterial) {
////			unique_ptr<MaterialSpecular>:
////				num specularFactor;
////				Optional<TextureInfo> specularTexture;
////				std::array<num, 3> specularColorFactor;
////				Optional<TextureInfo> specularColorTexture;
//		}

		if (auto& pbrData = material.pbrData; pbrData.baseColorTexture) {

			auto baseColorTextureIndex = (*pbrData.baseColorTexture).textureIndex;
			auto& texture = asset.textures[baseColorTextureIndex];

			if (auto imageIndex = texture.imageIndex) {

				auto& image = asset.images[*imageIndex];

				if (auto& dataSource = image.data
						; holds_alternative<sources::Vector>(dataSource)) { // .gltf

//					AE_LOG_D("Loading .gltf texture buffer...");

					auto uint8Vec = get<sources::Vector>(dataSource).bytes;
					auto aeBuffer = make_shared<ae::Buffer>(uint8Vec.data(), uint8Vec.size());
					auto aeImage = make_shared<ae::Image>(aeBuffer, false);
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
//						AE_LOG_D("Loading .glb texture buffer...");

						auto buffer = asset.buffers[bufferView.bufferIndex];
						auto byteOffset = bufferView.byteOffset;
						auto byteLength = bufferView.byteLength;

						if (auto bufferData = buffer.data
								; holds_alternative<sources::Vector>(bufferData)) {

							auto uint8Vec = get<sources::Vector>(bufferData).bytes;
							auto aeBuffer = make_shared<ae::Buffer>(&uint8Vec[byteOffset], byteLength);
							auto aeImage = make_shared<ae::Image>(aeBuffer, false);
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

	return ae::Material::DefaultMaterial();
}

shared_ptr<ae::Light> GlTFImporter::lightFromGlTFNode(fastgltf::Asset& asset,
													  fastgltf::Node& node) {

	if (auto lightIndex = node.lightIndex) {

		auto& light = asset.lights[*lightIndex];
		auto& type = light.type;

		if (type == fastgltf::LightType::Point) {

			auto aeLight = make_shared<Light>(LIGHT_TYPE::POINT);

			aeLight->name(string(light.name));
			aeLight->attenuationFactor(0); // temporary
			aeLight->color(colorFromGlTFColorArray(light.color));
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

shared_ptr<ae::PerspectiveCamera> GlTFImporter::cameraFromGlTFNode(fastgltf::Asset& asset,
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

mat4 GlTFImporter::transformFromGlFTNode(fastgltf::Node& node) {

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

shared_ptr<ae::Color> GlTFImporter::colorFromGlTFColorArray(array<float, 3>& arr) {

	return make_shared<Color>(arr[0], arr[1], arr[2]);
}

