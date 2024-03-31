//
// Created by mkd on 1/30/24.
//

#include "a3d/scene/importer/GlTFImporter.h"


#include <filesystem>
#include <variant>

#include "fastgltf/parser.hpp"
#include "fastgltf/tools.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/util.hpp"
#include "fmt/format.h"
#include "glm/gtc/type_ptr.hpp"
#include "magic_enum.hpp"

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/Image.h"
#include "a3d/Types.h"
#include "a3d/diagnostic/exception/UnsupportedFormat.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/Light.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/camera/PerspectiveCamera.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/rendering/material/Sampler.h"
#include "a3d/rendering/material/Texture.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace fastgltf;
using namespace glm;
using namespace std;


static fastgltf::Options GlTFOptionsFromImportOptions(SceneImportOptions options);
static mat4 TransformFromGlTFNode(fastgltf::Node& node);
static shared_ptr<a3d::Color> ColorFromGlTFColorArray(array<float, 3>& arr);
static shared_ptr<a3d::Color> ColorFromGlTFColorArray(array<float, 4>& arr);


GlTFImporter::GlTFImporter(const filesystem::path& path,
						   SceneImportOptions options):
		_parsed{false},
		_asset{},
		_scene{},
		_path{path},
		_options{options},
		_cameras{},
		_meshes{},
		_images{},
		_lights{},
		_materials{},
		_textures{},
		_samplers{} {

	auto extension = path.extension();
	if (!(extension == ".gltf" || extension == ".glb")) {
		throw UnsupportedFormat(fmt::format("Unsupported format: {}", extension.string()));
	}
}

unique_ptr<a3d::Scene> GlTFImporter::scene() {

	if (!_scene) {
		if (parse()) {

			auto startTime = Scene::Time();

			auto a3dScene = make_unique<a3d::Scene>();

			auto& scenes = _asset.scenes;
			if (!scenes.empty()) {

				if (scenes.size() > 1) {
					A3D_LOG_W("Ignoring extra scenes.");
				}

				auto& scene = scenes[_asset.defaultScene ? *_asset.defaultScene : 0];

				a3dScene->name(string(scene.name));

				auto nodeIndicies = scene.nodeIndices;
				if (!nodeIndicies.empty()) {

					for (auto n: nodeIndicies) {
						visitGlTFNode(_asset, _asset.nodes[n], a3dScene->rootNode().get());
					}

					// TODO: throw out nodes that don't have anything attached to them, or any children?

					A3D_LOG_I("Done loading scene.  Time: {}", Scene::Time() - startTime);

					_scene = std::move(a3dScene);
				}
				else {
					A3D_LOG_E("No nodes in scene: {}", scene.name);
				}
			}
			else {
				A3D_LOG_E("File contains no scenes!");
			}
		}
	}

	return std::move(_scene);
}

shared_ptr<a3d::Mesh> GlTFImporter::firstMesh() {

	// unlike scene(), no need to cache the mesh explicitly
	// because it will quickly be looked up in _meshes

	shared_ptr<Mesh> mesh = nullptr;

	if (parse()) {

		auto startTime = Scene::Time();

		auto& meshes = _asset.meshes;
		if (!meshes.empty()) {

			mesh = meshFromGlTFMeshIndex(_asset, 0);

			if (mesh) {
				A3D_LOG_I("Done loading mesh.  Time: {}", Scene::Time() - startTime);
			}
		}
		else {
			A3D_LOG_E("File contains no meshes!");
		}
	}

	return mesh;
}

const filesystem::path& GlTFImporter::path() const {
	return _path;
}

SceneImportOptions GlTFImporter::options() const {
	return _options;
}

bool GlTFImporter::parse() {

	if (!_parsed) {

		A3D_LOG_I("Parsing glTF: '{}'...", _path.string());

		auto startTime = Scene::Time();

		auto extensions = Extensions::KHR_lights_punctual
						  | Extensions::KHR_materials_specular
						  | Extensions::KHR_materials_anisotropy
						  | Extensions::KHR_texture_transform;
		auto parser = Parser(extensions);

		auto extension = _path.extension();
		auto directory = _path.parent_path();

		auto options = GlTFOptionsFromImportOptions(_options);

		GltfDataBuffer data;
		data.loadFromFile(_path);

		auto expectedAsset = Expected<Asset>(Error::None);

		if (extension == ".gltf") {
			expectedAsset = parser.loadGLTF(&data, directory, options);
		}
		else if (extension == ".glb") {
			expectedAsset = parser.loadBinaryGLTF(&data, directory, options);
		}
		else {
			A3D_LOG_E("Unsupported file extension: {}", extension.string());
		}

		if (auto error = expectedAsset.error(); error == Error::None) {

			auto& asset = expectedAsset.get();

			if (auto& info = asset.assetInfo) {
				A3D_LOG_D("Done parsing glTF.  Version: '{}', Copyright: '{}', Generator: '{}'.  Parse time: {}",
						  info->gltfVersion, info->copyright, info->generator, Scene::Time() - startTime);
			}
			else {
				A3D_LOG_D("Done parsing glTF.  Time: {}", Scene::Time() - startTime);
			}

			_asset = std::move(expectedAsset.get());
		}
		else {

			A3D_LOG_E("Error parsing glTF file: {}", magic_enum::enum_name(error));
			return false;
		}
	}

	return true;
}

void GlTFImporter::visitGlTFNode(fastgltf::Asset& asset,
								 fastgltf::Node& node,
								 a3d::Node* parent) {

	// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?

	auto a3dNode = Node::NamedNode(string(node.name));

	a3dNode->transform(TransformFromGlTFNode(node));

	if ((_options & SceneImportOptions::ImportMeshes) != SceneImportOptions::None) {
		a3dNode->mesh(meshFromGlTFNode(asset, node));
	}

	if ((_options & SceneImportOptions::ImportLights) != SceneImportOptions::None) {
		a3dNode->light(lightFromGlTFNode(asset, node));
	}

	if ((_options & SceneImportOptions::ImportCameras) != SceneImportOptions::None) {
		a3dNode->camera(cameraFromGlTFNode(asset, node));
	}

//	auto nodePtr = a3dNode.get();
//	parent->addChild(std::move(a3dNode));

	for (auto c: node.children) {
		visitGlTFNode(asset, asset.nodes[c], a3dNode.get());
	}

	parent->addChild(a3dNode);
}

shared_ptr<a3d::Mesh> GlTFImporter::meshFromGlTFNode(fastgltf::Asset& asset,
													 fastgltf::Node& node) {
	if (auto meshIndex = node.meshIndex) {
		return meshFromGlTFMeshIndex(asset, *meshIndex);
	}
	return nullptr;
}

shared_ptr<a3d::Mesh> GlTFImporter::meshFromGlTFMeshIndex(fastgltf::Asset& asset,
														  std::size_t meshIndex) {

	if (_meshes.find(meshIndex) == _meshes.end()) {

		auto& mesh = asset.meshes[meshIndex];

		auto elements = vector<unique_ptr<MeshElement>>();
		auto materials = vector<shared_ptr<Material>>();

		for (auto& primitive: mesh.primitives) {

			auto element = meshElementFromGlTFPrimitive(asset, primitive);
			if (element) elements.push_back(std::move(element));

			// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?
			auto material = ((_options & SceneImportOptions::ImportMaterials) != SceneImportOptions::None)
							? materialFromGlTFPrimitive(asset, primitive)
							: Material::DefaultMaterial();
//							: make_shared<Material>();
			if (material) materials.push_back(material);
		}

		auto a3dMesh = make_shared<a3d::Mesh>(string(mesh.name), elements, materials);
		_meshes[meshIndex] = a3dMesh;
		return a3dMesh;
	}
	else {
		return _meshes[meshIndex];
	}

	return nullptr;
}

unique_ptr<a3d::MeshElement> GlTFImporter::meshElementFromGlTFPrimitive(fastgltf::Asset& asset,
																		fastgltf::Primitive& primitive) {

	// TODO: make this suck less

	if (primitive.type == PrimitiveType::Triangles) {

		if (auto indiciesAccessorIndex = primitive.indicesAccessor) {
			auto& indiciesAccessor = asset.accessors[*indiciesAccessorIndex];

			vector<Vertex> verts;
			vector<Face> faces;

			// faces

			{
				vector<uint32_t> indices;
				indices.resize(indiciesAccessor.count);

				iterateAccessorWithIndex<uint32_t>(
						asset,
						indiciesAccessor,
						[&](uint32_t index, size_t idx) {
							indices[idx] = index;
						});

				for (size_t i = 0; i < indices.size(); i += 3) {
					faces.push_back( {unsigned(indices[i + 0]),
									  unsigned(indices[i + 1]),
									  unsigned(indices[i + 2])} );
				}
			}

			// positions

			{
				auto attrib = primitive.findAttribute("POSITION");

				auto accessorIndex = attrib->second;
				auto accessor = asset.accessors[accessorIndex];

				auto type = accessor.type;
				if (type == AccessorType::Vec3) {

					auto componentType = accessor.componentType;
					if (componentType == ComponentType::Float) {

						auto& bufferView = asset.bufferViews[*accessor.bufferViewIndex];
						auto& buffer = asset.buffers[bufferView.bufferIndex];
						auto& bufferData = buffer.data;

						if (auto vec = std::get_if<sources::Vector>(&bufferData)) {

							auto offset = bufferView.byteOffset + accessor.byteOffset;
							auto length = bufferView.byteLength;
							auto elementByteSize = getElementByteSize(type, componentType);
							auto numElements = length / elementByteSize;

							auto ptr = reinterpret_cast<uint8_t*>(&vec->bytes[offset]);
							auto stride = bufferView.byteStride
										  ? *(bufferView.byteStride)
										  : sizeof(vec3);

							for (size_t i = 0; i < numElements; ++i) {
								auto v3Ptr = reinterpret_cast<vec3*>(ptr);
								verts.push_back({*v3Ptr, {}, {}});
								ptr += stride;
							}
						}
						else {
							A3D_LOG_W("Unsupported position buffer data.");
						}
					}
					else {
						A3D_LOG_W("Unsupported position accessor component type: {}",
								  magic_enum::enum_name(componentType));
					}
				}
				else {
					A3D_LOG_W("Unsupported position accessor type: {}",
							  magic_enum::enum_name(type));
				}
			}

			// normals

			{
				auto attrib = primitive.findAttribute("NORMAL");

				auto accessorIndex = attrib->second;
				auto accessor = asset.accessors[accessorIndex];

				auto type = accessor.type;
				if (type == AccessorType::Vec3) {

					auto componentType = accessor.componentType;
					if (componentType == ComponentType::Float) {

						auto& bufferView = asset.bufferViews[*accessor.bufferViewIndex];
						auto& buffer = asset.buffers[bufferView.bufferIndex];
						auto& bufferData = buffer.data;

						if (auto vec = std::get_if<sources::Vector>(&bufferData)) {

							auto offset = bufferView.byteOffset + accessor.byteOffset;
							auto length = bufferView.byteLength;
							auto elementByteSize = getElementByteSize(type, componentType);
							auto numElements = length / elementByteSize;

							auto ptr = reinterpret_cast<uint8_t*>(&vec->bytes[offset]);
							auto stride = bufferView.byteStride
										  ? *(bufferView.byteStride)
										  : sizeof(vec3);

							for (size_t i = 0; i < numElements; ++i) {
								auto v3Ptr = reinterpret_cast<vec3*>(ptr);
								verts[i].normal = *v3Ptr;
								ptr += stride;
							}
						}
						else {
							A3D_LOG_W("Unsupported normals buffer data.");
						}
					}
					else {
						A3D_LOG_W("Unsupported normals accessor component type: {}",
								  magic_enum::enum_name(componentType));
					}
				}
				else {
					A3D_LOG_W("Unsupported normals accessor type: {}",
							  magic_enum::enum_name(type));
				}
			}

			// texture coordinates

			{
				auto attrib = primitive.findAttribute("TEXCOORD_0");

				auto accessorIndex = attrib->second;
				auto accessor = asset.accessors[accessorIndex];

				auto type = accessor.type;
				if (type == AccessorType::Vec2) {

					auto componentType = accessor.componentType;
					if (componentType == ComponentType::Float) {

						auto& bufferView = asset.bufferViews[*accessor.bufferViewIndex];
						auto& buffer = asset.buffers[bufferView.bufferIndex];
						auto& bufferData = buffer.data;

						if (auto vec = std::get_if<sources::Vector>(&bufferData)) {

							auto offset = bufferView.byteOffset + accessor.byteOffset;
							auto length = bufferView.byteLength;
							auto elementByteSize = getElementByteSize(type, componentType);
							auto numElements = length / elementByteSize;

							auto ptr = reinterpret_cast<uint8_t*>(&vec->bytes[offset]);
							auto stride = bufferView.byteStride
										  ? *(bufferView.byteStride)
										  : sizeof(vec2);

							for (size_t i = 0; i < numElements; ++i) {
								auto v2Ptr = reinterpret_cast<vec2*>(ptr);
								verts[i].texCoord = *v2Ptr;
								ptr += stride;
							}
						}
						else {
							A3D_LOG_W("Unsupported texcoords buffer data.");
						}
					}
					else {
						A3D_LOG_W("Unsupported texcoords accessor component type: {}",
								  magic_enum::enum_name(componentType));
					}
				}
				else {
					A3D_LOG_W("Unsupported texcoords accessor type: {}",
							  magic_enum::enum_name(type));
				}
			}

			return make_unique<MeshElement>(verts, faces);
		}
		else {
			A3D_LOG_E("Missing vertex indicies.");
		}
	}
	else {
		A3D_LOG_W("Unsupported primitive type: {}",
				  magic_enum::enum_name(primitive.type));
	}

	return nullptr;
}

shared_ptr<a3d::Material> GlTFImporter::materialFromGlTFPrimitive(fastgltf::Asset& asset,
																  fastgltf::Primitive& primitive) {

	if (auto materialIndex = primitive.materialIndex) {

		if (_materials.find(*materialIndex) == _materials.end()) {

			auto& material = asset.materials[*materialIndex];

			shared_ptr<a3d::Material> a3dMaterial = nullptr;

			// ambient, diffuse

			if (auto& pbrData = material.pbrData ; pbrData.baseColorTexture) {

				auto baseColorTextureIndex = (*pbrData.baseColorTexture).textureIndex;
				if (auto a3dTexture = textureFromGlTFTextureIndex(asset, baseColorTextureIndex)
						; a3dTexture) {
					a3dMaterial = make_shared<a3d::Material>(monostate{}, a3dTexture, monostate{});
				}
				else {
					a3dMaterial = a3d::Material::MissingTextureMaterial();
				}
			}
			else {

				auto baseColorFactor = pbrData.baseColorFactor;

				auto a3dColor = ColorFromGlTFColorArray(baseColorFactor);
				auto a3dProperty = MaterialProperty(a3dColor);
				a3dMaterial = make_shared<a3d::Material>(monostate{}, a3dProperty, monostate{});
			}

			// specular

			if (auto& specularMaterial = material.specular; specularMaterial) {

				// TODO: this needs work.  and more testing.

				auto factor = specularMaterial->specularFactor;
				auto& textureInfo = specularMaterial->specularTexture;
				auto colorFactor = specularMaterial->specularColorFactor;
				auto& colorTextureInfo = specularMaterial->specularColorTexture;

//				A3D_LOG_D("*** [SPECULAR] ***");
//				A3D_LOG_D("factor: {}", factor);
//				A3D_LOG_D("textureInfo: {}", textureInfo ? "true" : "false");
//				A3D_LOG_D("colorFactor: ({}, {}, {})", colorFactor[0], colorFactor[1], colorFactor[2]);
//				A3D_LOG_D("colorTextureInfo: {}", colorTextureInfo ? "true" : "false");

				// if it has a 'texture' map, use it (only contains alpha)
				// if it has no map, but a 'factor' use solid white, with an intentify of the factor.

				MaterialProperty a3dProperty = monostate{};

				if (textureInfo) {

					auto specularTextureIndex = (*textureInfo).textureIndex;
					if (auto a3dTexture = textureFromGlTFTextureIndex(asset, specularTextureIndex)
							; a3dTexture) {
						a3dProperty = a3dTexture;
					}
					else {
						a3dProperty = a3d::Material::MissingTextureProperty();
					}
				}

				if (holds_alternative<monostate>(a3dProperty) && (factor > 0)) {
					auto factorColor = make_shared<Color>(factor);
					a3dProperty = MaterialProperty(factorColor);
				}

				if (!holds_alternative<monostate>(a3dProperty)) {
					if (!a3dMaterial) {
						a3dMaterial = make_shared<a3d::Material>(monostate{}, monostate{}, a3dProperty);
					}
					else {
						a3dMaterial->specular(a3dProperty);
					}
				}
			}

			if (a3dMaterial) {

				a3dMaterial->locksAmbientWithDiffuse(true);

				a3dMaterial->doubleSided(material.doubleSided);

				if (auto& anisotropy = material.anisotropy; anisotropy) {

					// log error to draw attention -- at time of initial glTF integration
					// we don't have an example file with KHR_materials_anisotropy
					auto strength = anisotropy->anisotropyStrength;
					A3D_LOG_E("anisotropyStrength: {}", strength);
//					a3dMaterial->maxAnisotropy(strength);

					for (auto [property, type] : a3dMaterial->properties()) {
						if (holds_alternative<shared_ptr<Texture>>(*property)) {
							auto texture = get<shared_ptr<Texture>>(*property);
							texture->sampler()->maxAnisotropy(strength);
						}
					}
				}

				// specularExponent?

				// transform -- scale

				// emissiveFactor/emissiveTexture?

				_materials[*materialIndex] = a3dMaterial;
				return a3dMaterial;
			}
		}
		else {
			return _materials[*materialIndex];
		}
	}
	else {
		A3D_LOG_W("Missing material.");
	}

//	return make_shared<Material>();
	return a3d::Material::DefaultMaterial();
}

shared_ptr<a3d::Texture> GlTFImporter::textureFromGlTFTextureIndex(fastgltf::Asset& asset,
																   std::size_t textureIndex) {

	if (_textures.find(textureIndex) == _textures.end()) {

		auto& texture = asset.textures[textureIndex];

		if (auto a3dImage = imageFromGlTFTexture(asset, texture) ; a3dImage) {

			auto a3dSampler = samplerFromGlTFTexture(asset, texture);

			auto a3dTexture = make_shared<a3d::Texture>(a3dImage, make_shared<a3d::Sampler>());
			_textures[textureIndex] = a3dTexture;
			return a3dTexture;
		}
	}
	else {
		return _textures[textureIndex];
	}

	return nullptr;
}

shared_ptr<a3d::Sampler> GlTFImporter::samplerFromGlTFTexture(fastgltf::Asset& asset,
															  fastgltf::Texture& texture) {

	if (auto samplerIndex = texture.samplerIndex) {

		if (_samplers.find(*samplerIndex) == _samplers.end()) {

			auto& sampler = asset.samplers[*samplerIndex];

			auto a3dSampler = make_shared<a3d::Sampler>();

			if (sampler.minFilter) {
				a3dSampler->minificationFilter(FilterMode(*sampler.minFilter));
			}
			if (sampler.magFilter) {
				a3dSampler->magnificationFilter(FilterMode(*sampler.magFilter));
			}
			a3dSampler->wrapS(WrapMode(sampler.wrapS));
			a3dSampler->wrapT(WrapMode(sampler.wrapT));

			return a3dSampler;
		}
		else {
			return _samplers[*samplerIndex];
		}
	}

	return nullptr;
}

shared_ptr<a3d::Image> GlTFImporter::imageFromGlTFTexture(fastgltf::Asset& asset,
														  fastgltf::Texture& texture) {

	if (auto imageIndex = texture.imageIndex) {

		if (_images.find(*imageIndex) == _images.end()) {

			auto& image = asset.images[*imageIndex];

			shared_ptr<Image> a3dImage = nullptr;

			if (auto &dataSource = image.data; holds_alternative<sources::Vector>(dataSource)) { // .gltf
				A3D_LOG_D("Creating texture image...");

				auto uint8Vec = get<sources::Vector>(dataSource).bytes;
				auto a3dBuffer = make_unique<a3d::Buffer>(reinterpret_cast<std::byte*>(uint8Vec.data()), uint8Vec.size());
				a3dImage = make_shared<a3d::Image>(std::move(a3dBuffer), false);
			}
			else if (holds_alternative<sources::BufferView>(dataSource)) { // .glb

				auto bufferViewIndex = get<sources::BufferView>(dataSource).bufferViewIndex;
				auto& bufferView = asset.bufferViews[bufferViewIndex];

				if (auto byteStride = bufferView.byteStride) { // TODO: is this unpacked for us?

					A3D_LOG_W("Texture buffer has stride: {}.  Skipping.", *(bufferView.byteStride));
					a3dImage = nullptr;
				}
				else {
					A3D_LOG_D("Creating texture image...");

					auto buffer = asset.buffers[bufferView.bufferIndex];
					auto byteOffset = bufferView.byteOffset;
					auto byteLength = bufferView.byteLength;

					if (auto bufferData = buffer.data; holds_alternative<sources::Vector>(bufferData)) {

						auto uint8Vec = get<sources::Vector>(bufferData).bytes;
						auto a3dBuffer = make_unique<a3d::Buffer>(reinterpret_cast<std::byte*>(&uint8Vec[byteOffset]), byteLength);
						a3dImage = make_shared<a3d::Image>(std::move(a3dBuffer), false);
					}
					else {
						A3D_LOG_W("Unexpected texture data.");
						a3dImage = nullptr;
					}
				}
			}
			else {
				A3D_LOG_W("Unexpected texture data.");
				a3dImage = nullptr;
			}

			if (a3dImage) _images[*imageIndex] = a3dImage;
			return a3dImage;
		}
		else {
			return _images[*imageIndex];
		}
	}

	return nullptr;
}

shared_ptr<a3d::Light> GlTFImporter::lightFromGlTFNode(fastgltf::Asset& asset,
													   fastgltf::Node& node) {

	if (auto lightIndex = node.lightIndex) {

		if (_lights.find(*lightIndex) == _lights.end()) {

			auto& light = asset.lights[*lightIndex];
			auto& type = light.type;

			if (type == fastgltf::LightType::Point) {

				auto a3dLight = make_shared<Light>(LightType::Point);

				a3dLight->name(string(light.name));
				a3dLight->attenuationFactor(0); // temporary
				a3dLight->color(ColorFromGlTFColorArray(light.color));
				// TODO: range, intensity

				_lights[*lightIndex] = a3dLight;
				return a3dLight;
			}
			else {

				A3D_LOG_W("Unsupported light type: {}",
						  magic_enum::enum_name(type));
			}
		}
		else {
			return _lights[*lightIndex];
		}
	}

	return nullptr;
}

shared_ptr<a3d::Camera> GlTFImporter::cameraFromGlTFNode(fastgltf::Asset& asset,
														 fastgltf::Node& node) {

	if (auto cameraIndex = node.cameraIndex) {

		if (_cameras.find(*cameraIndex) == _cameras.end()) {

			auto& camera = asset.cameras[*cameraIndex];

			auto cameraVar = camera.camera;
			if (holds_alternative<fastgltf::Camera::Perspective>(cameraVar)) {

				auto perspective = get<fastgltf::Camera::Perspective>(cameraVar);

				auto a3dCamera = make_shared<PerspectiveCamera>(string(camera.name),
																perspective.znear,
																(perspective.zfar
																 ? *perspective.zfar
																 : 1000000), // cheating
																perspective.yfov);

				if (auto ratio = perspective.aspectRatio) {
					a3dCamera->aspectRatio(*ratio);
				}

				_cameras[*cameraIndex] = a3dCamera;
				return a3dCamera;
			}
			else if (holds_alternative<fastgltf::Camera::Orthographic>(cameraVar)) {

				A3D_LOG_W("Orthographic cameras are not supported.");
			}
		}
		else {
			return _cameras[*cameraIndex];
		}
	}

	return nullptr;
}

fastgltf::Options GlTFOptionsFromImportOptions(SceneImportOptions options) {

	auto gltfOptions = Options::None;

	// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?

	if ((options & SceneImportOptions::ImportMeshes) != SceneImportOptions::None) {
		gltfOptions |= Options::LoadGLBBuffers
					   | Options::LoadExternalBuffers
					   | Options::GenerateMeshIndices;
	}

	if ((options & SceneImportOptions::ImportMaterials) != SceneImportOptions::None) {
		gltfOptions |= Options::LoadGLBBuffers
					   | Options::LoadExternalBuffers
					   | Options::LoadExternalImages;
	}

	if ((options & SceneImportOptions::ImportLights) != SceneImportOptions::None) {

	}

	if ((options & SceneImportOptions::ImportCameras) != SceneImportOptions::None) {

	}

	return gltfOptions;
}

mat4 TransformFromGlTFNode(fastgltf::Node& node) {

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
		A3D_LOG_W("No transform associated with node: {}", node.name);
	}

	return mat4(1.0);
}

shared_ptr<a3d::Color> ColorFromGlTFColorArray(array<float, 3>& arr) {

	return make_shared<Color>(arr[0], arr[1], arr[2]);
}

shared_ptr<a3d::Color> ColorFromGlTFColorArray(array<float, 4>& arr) {

	array<float, 3> rgbArray = {arr[0], arr[1], arr[2]};
	return ColorFromGlTFColorArray(rgbArray);
}
