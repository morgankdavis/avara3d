//
// Created by mkd on 1/30/24.
//

#include "ae/scene/importer/GlTFImporter.h"


using namespace ae;
using namespace fastgltf;
using namespace glm;
using namespace std;


#include <filesystem>
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
#include "ae/Image.h"
#include "ae/Types.h"
#include "ae/diagnostic/exceptions/UnsupportedFormat.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/geometry/Geometry.h"
#include "ae/geometry/GeometryElement.h"
#include "ae/rendering/Light.h"
#include "ae/rendering/camera/Camera.h"
#include "ae/rendering/camera/PerspectiveCamera.h"
#include "ae/rendering/materials/Material.h"
#include "ae/rendering/materials/MaterialProperty.h"
#include "ae/scene/Node.h"
#include "ae/scene/Scene.h"


GlTFImporter::GlTFImporter(const filesystem::path& path):
		_scene{nullptr},
		_path{path},
		_cameras{},
		_geometries{},
		_images{},
		_lights{},
		_materials{},
		_materialProperties{} {

	auto extension = path.extension();
	if (!(extension == ".glb" || extension == ".gltf")) {
		throw UnsupportedFormat(fmt::format("Unsupported format: {}", extension.string()));
	}
}

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

	auto extensions = Extensions::KHR_lights_punctual
					  | Extensions::KHR_materials_specular
					  | Extensions::KHR_materials_anisotropy
					  | Extensions::KHR_texture_transform;
	auto parser = Parser(extensions);

	auto extension = _path.extension();
	auto directory = _path.parent_path();

	auto options = Options::LoadGLBBuffers
				   | Options::LoadExternalBuffers
				   | Options::LoadExternalImages
				   | Options::GenerateMeshIndices;

	GltfDataBuffer data;
	data.loadFromFile(_path);

	auto expectedAsset = Expected<Asset>(Error::None);

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

				AE_LOG_I("Done loading glTF.  Time: {}",  aeScene->time() - startTime);

				return aeScene;
			}
			else {
				AE_LOG_E("No nodes in scene: {}", scene.name);
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

	aeNode->transform(transformFromGlFTNode(node));
	aeNode->geometry(geometryFromGlFTNode(asset, node));
	aeNode->light(lightFromGlTFNode(asset, node));
	aeNode->camera(cameraFromGlTFNode(asset, node));

	parent->addChild(aeNode);

	for (auto c : node.children) {
		visitGlTFNode(asset, asset.nodes[c], aeNode);
	}
}

shared_ptr<Geometry> GlTFImporter::geometryFromGlFTNode(fastgltf::Asset& asset,
														fastgltf::Node& node) {

	if (auto meshIndex = node.meshIndex) {

		if (auto existing = _geometries.find(*meshIndex)
				; existing == _geometries.end()) {

			auto &mesh = asset.meshes[*meshIndex];

			AE_LOG_D("Mesh name: {}", mesh.name);

			auto elements = vector<shared_ptr<GeometryElement>>();
			auto materials = vector<shared_ptr<Material>>();

			for (auto &primitive: mesh.primitives) {

				auto element = geometryElementFromGlFTPrimitive(asset, primitive);
				if (element) elements.push_back(element);

				auto material = materialFromGlFTPrimitive(asset, primitive);
				if (material) materials.push_back(material);
			}

			auto geometry = make_shared<Geometry>(elements, materials);
			geometry->name(string(mesh.name));
			_geometries[*meshIndex] = geometry;
			return geometry;
		}
		else {
			return _geometries[*meshIndex];
		}
	}

	return nullptr;
}

shared_ptr<ae::GeometryElement> GlTFImporter::geometryElementFromGlFTPrimitive(fastgltf::Asset& asset,
																			   fastgltf::Primitive& primitive) {

	// TODO: make this not suck

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

		if (auto existing = _materials.find(*materialIndex)
				; existing == _materials.end()) {

			auto &material = asset.materials[*materialIndex];

			shared_ptr<ae::Material> aeMaterial = nullptr;

			// ambient, diffuse

			if (auto &pbrData = material.pbrData
					; pbrData.baseColorTexture) {

				auto baseColorTextureIndex = (*pbrData.baseColorTexture).textureIndex;
				auto &texture = asset.textures[baseColorTextureIndex];

				if (auto aeImage = imageFromGlTFTexture(asset, texture)
						; aeImage) {
					auto aeProperty = materialPropertyFromGlTFTexture(asset, texture);
					aeProperty->contents(aeImage);

					aeMaterial = make_shared<ae::Material>(aeProperty,
														   aeProperty,
														   nullptr);
				}
				else {
					aeMaterial = ae::Material::MissingTextureMaterial();
				}
			}
			else {

				// TODO: enabling this fucks up specular
				// enabing specular below fixes the magenta light's specular reflection,
				// but not the yellow light's specular reflection. (?)

				auto baseColorFactor = pbrData.baseColorFactor;

				auto aeColor = colorFromGlTFColorArray(baseColorFactor);
				auto property = make_shared<MaterialProperty>(aeColor);
				aeMaterial = make_shared<ae::Material>(property, property, nullptr);
			}

			// specular

			auto& specularMaterial = material.specular;
			if (specularMaterial) {

				auto factor = specularMaterial->specularFactor;
				auto& textureInfo = specularMaterial->specularTexture;
				auto colorFactor = specularMaterial->specularColorFactor;
				auto& colorTextureInfo = specularMaterial->specularColorTexture;

				AE_LOG_D("*** [SPECULAR] ***");

				AE_LOG_D("factor: {}", factor);
				AE_LOG_D("textureInfo: {}", textureInfo ? "true" : "false");
				AE_LOG_D("colorFactor: ({}, {}, {})", colorFactor[0], colorFactor[1], colorFactor[2]);
				AE_LOG_D("colorTextureInfo: {}", colorTextureInfo ? "true" : "false");
			}

			if (aeMaterial) {
				aeMaterial->doubleSided(material.doubleSided);
				_materials[*materialIndex] = aeMaterial;
				return aeMaterial;
			}
		}
		else {
			return _materials[*materialIndex];
		}
	}
	else {
		AE_LOG_W("Missing material.");
	}

	return ae::Material::DefaultMaterial();
}

shared_ptr<ae::Image> GlTFImporter::imageFromGlTFTexture(fastgltf::Asset& asset,
														 fastgltf::Texture& texture) {

	if (auto imageIndex = texture.imageIndex) {

		if (auto existing = _images.find(*imageIndex)
				; existing == _images.end()) {

			auto &image = asset.images[*imageIndex];

			shared_ptr<Image> aeImage = nullptr;

			if (auto &dataSource = image.data; holds_alternative<sources::Vector>(dataSource)) { // .gltf
				AE_LOG_D("Creating texture image...");

				auto uint8Vec = get<sources::Vector>(dataSource).bytes;
				auto aeBuffer = make_shared<ae::Buffer>(uint8Vec.data(), uint8Vec.size());
				aeImage = make_shared<ae::Image>(aeBuffer, false);
			}
			else if (holds_alternative<sources::BufferView>(dataSource)) { // .glb

				auto bufferViewIndex = get<sources::BufferView>(dataSource).bufferViewIndex;
				auto &bufferView = asset.bufferViews[bufferViewIndex];

				if (auto byteStride = bufferView.byteStride) { // TODO: is this unpacked for us?

					AE_LOG_W("Texture buffer has stride: {}.  Skipping.", *(bufferView.byteStride));
					aeImage = nullptr;
				}
				else {
					AE_LOG_D("Creating texture image...");

					auto buffer = asset.buffers[bufferView.bufferIndex];
					auto byteOffset = bufferView.byteOffset;
					auto byteLength = bufferView.byteLength;

					if (auto bufferData = buffer.data; holds_alternative<sources::Vector>(bufferData)) {

						auto uint8Vec = get<sources::Vector>(bufferData).bytes;
						auto aeBuffer = make_shared<ae::Buffer>(&uint8Vec[byteOffset], byteLength);
						aeImage = make_shared<ae::Image>(aeBuffer, false);
					}
					else {
						AE_LOG_W("Unexpected texture data.");
						aeImage = nullptr;
					}
				}
			}
			else {
				AE_LOG_W("Unexpected texture data.");
				aeImage = nullptr;
			}

			if (aeImage) _images[*imageIndex] = aeImage;
			return aeImage;
		}
		else {
			return _images[*imageIndex];
		}
	}

	return nullptr;
}

shared_ptr<MaterialProperty> GlTFImporter::materialPropertyFromGlTFTexture(fastgltf::Asset& asset,
																		   fastgltf::Texture& texture) {

	// ! important !
	// don't map these to ae::MaterialProperty.
	// AE uses a 1:1 Image/Color:MaterialProperty relationship whereas
	// glTF uses a 1:N Texture:Sampler relationship.

	auto aeProperty = make_shared<ae::MaterialProperty>();

	if (auto samplerIndex = texture.samplerIndex) {
		auto sampler = asset.samplers[*samplerIndex];

		if (sampler.minFilter) {
			aeProperty->minificationFilter(FILTER_MODE(*sampler.minFilter));
		}
		if (sampler.magFilter) {
			aeProperty->magnificationFilter(FILTER_MODE(*sampler.magFilter));
		}
		aeProperty->wrapS(WRAP_MODE(sampler.wrapS));
		aeProperty->wrapT(WRAP_MODE(sampler.wrapT));
	}

	return aeProperty;
};

shared_ptr<ae::Light> GlTFImporter::lightFromGlTFNode(fastgltf::Asset& asset,
													  fastgltf::Node& node) {

	if (auto lightIndex = node.lightIndex) {

		if (auto existing = _lights.find(*lightIndex)
				; existing == _lights.end()) {

			auto& light = asset.lights[*lightIndex];
			auto& type = light.type;

			if (type == fastgltf::LightType::Point) {

				auto aeLight = make_shared<Light>(LIGHT_TYPE::POINT);

				aeLight->name(string(light.name));
				aeLight->attenuationFactor(0); // temporary
				aeLight->color(colorFromGlTFColorArray(light.color));
				// TODO: range, intensity

				_lights[*lightIndex] = aeLight;
				return aeLight;
			}
			else {

				AE_LOG_W("Unsupported light type: {}",
						 magic_enum::enum_name<fastgltf::LightType>(type));
			}
		}
		else {
			return _lights[*lightIndex];
		}
	}

	return nullptr;
}

shared_ptr<ae::Camera> GlTFImporter::cameraFromGlTFNode(fastgltf::Asset& asset,
														fastgltf::Node& node) {

	if (auto cameraIndex = node.cameraIndex) {

		if (auto existing = _cameras.find(*cameraIndex)
				; existing == _cameras.end()) {

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

				_cameras[*cameraIndex] = aeCamera;
				return aeCamera;
			}
			else if (holds_alternative<fastgltf::Camera::Orthographic>(cameraVar)) {

				AE_LOG_W("Orthographic cameras are not supported.");
			}
		}
		else {
			return _cameras[*cameraIndex];
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

shared_ptr<ae::Color> GlTFImporter::colorFromGlTFColorArray(array<float, 4>& arr) {

	array<float, 3> rgbArray = {arr[0], arr[1], arr[2]};
	return colorFromGlTFColorArray(rgbArray);
}
