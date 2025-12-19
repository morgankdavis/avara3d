//
//  GlTFImporter.cc
//  avara3d
//
//  Created by Morgan Davis on 1/30/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/scene/importer/GlTFImporter.h"

#include <filesystem>
#include <format>
#include <utility>
#include <variant>

// TODO: clean up
#include <fastgltf/core.hpp>
#include "fastgltf/math.hpp"
//#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include "fastgltf/util.hpp"

#include "magic_enum/magic_enum.hpp"

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/Image.h"
#include "a3d/Types.h"
#include "a3d/diagnostic/exception/UnsupportedFormatException.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/camera/PerspectiveCamera.h"
#include "a3d/rendering/light/DirectionalLight.h"
#include "a3d/rendering/light/Light.h"
#include "a3d/rendering/light/PointLight.h"
#include "a3d/rendering/light/SpotLight.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/rendering/material/Sampler.h"
#include "a3d/rendering/material/Texture.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/Utilities.h"

using namespace a3d;
using namespace fastgltf;
using namespace a3d::math;
using namespace std;

/// Private Static Non-Member Prototypes ///

static fastgltf::Options GlTFOptionsFromImportOptions(SceneImportOptions options);
static std::span<const byte> BytesFromDataSource(const DataSource& src);
static std::span<const byte> BytesFromBufferView(const Asset& asset, size_t bufferViewIndex);
static mat4 TransformFromGlTFNode(fastgltf::Node& node);
//static shared_ptr<a3d::Color> ColorFromGlTFColorArray(array<float, 3>& arr);
//static shared_ptr<a3d::Color> ColorFromGlTFColorArray(array<float, 4>& arr);
// NEW for fastgltf 0.9:
static shared_ptr<a3d::Color> ColorFromGlTFColorArray(const fastgltf::math::nvec3& v);
static shared_ptr<a3d::Color> ColorFromGlTFColorArray(const fastgltf::math::nvec4& v);

/// Internal Lifecycle Functions ///

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
		throw UnsupportedFormatException(std::format("Unsupported format: {}", extension.string()));
	}
}

/// Internal Member Functions ///

unique_ptr<a3d::Scene> GlTFImporter::scene() {

	if (!_scene) {
		if (parse()) {

			auto startTime = utils::chrono::Time();

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

					A3D_LOG_I("Done loading scene.  Time: {}", utils::chrono::Time() - startTime);

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

		auto startTime = utils::chrono::Time();

		auto& meshes = _asset.meshes;
		if (!meshes.empty()) {

			mesh = meshFromGlTFMeshIndex(_asset, 0);

			if (mesh) {
				A3D_LOG_I("Done loading mesh.  Time: {}", utils::chrono::Time() - startTime);
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

/// Private Member Functions ///

bool GlTFImporter::parse() {

	if (!_parsed) {

		auto startTime = utils::chrono::Time();

		A3D_LOG_I("Parsing glTF: '{}'...", _path.string());

		auto extensions = Extensions::KHR_lights_punctual
						  | Extensions::KHR_materials_specular
						  | Extensions::KHR_materials_anisotropy
						  | Extensions::KHR_texture_transform;
		auto parser = Parser(extensions);

//		auto extension = _path.extension();
		auto directory = _path.parent_path();

		auto options = GlTFOptionsFromImportOptions(_options);

		auto gltfFile = fastgltf::MappedGltfFile::FromPath(_path);
		if (!bool(gltfFile)) {
			std::cerr << "Failed to open glTF file: " << fastgltf::getErrorMessage(gltfFile.error()) << '\n';
			return false;
		}

		auto asset = parser.loadGltf(gltfFile.get(), directory, options);
		if (asset.error() != fastgltf::Error::None) {
			std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(asset.error()) << '\n';
			return false;
		}

		_asset = std::move(asset.get());


//		GltfDataBuffer data;
//		data.loadFromFile(_path);
//
//		auto expectedAsset = Expected<Asset>(Error::None);
//
//		if (extension == ".gltf") {
//			expectedAsset = parser.loadGLTF(&data, directory, options);
//		}
//		else if (extension == ".glb") {
//			expectedAsset = parser.loadBinaryGLTF(&data, directory, options);
//		}
//		else {
//			A3D_LOG_E("Unsupported file extension: {}", extension.string());
//		}
//
//		if (auto error = expectedAsset.error(); error == Error::None) {
//
//			auto& asset = expectedAsset.get();
//
//			if (auto& info = asset.assetInfo) {
//				A3D_LOG_D("Done parsing glTF.  Version: '{}', Copyright: '{}', Generator: '{}'.  Parse time: {}",
//						  info->gltfVersion, info->copyright, info->generator, utils::chrono::Time() - startTime);
//			}
//			else {
//				A3D_LOG_D("Done parsing glTF.  Time: {}", utils::chrono::Time() - startTime);
//			}
//
//			_asset = std::move(expectedAsset.get());
//		}
//		else {
//
//			A3D_LOG_E("Error parsing glTF file: {}", magic_enum::enum_name(error));
//			return false;
//		}
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

	//if ((_options & SceneImportOptions::ImportCameras) != SceneImportOptions::None) {
		a3dNode->camera(cameraFromGlTFNode(asset, node));
	//}

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

		A3D_LOG_D("Importing mesh '{}'...", mesh.name);

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

std::unique_ptr<a3d::MeshElement> GlTFImporter::meshElementFromGlTFPrimitive(
		fastgltf::Asset& asset,
		fastgltf::Primitive& primitive)
{
	using namespace fastgltf;

	if (primitive.type != PrimitiveType::Triangles) {
		A3D_LOG_W("Unsupported primitive type: {}", magic_enum::enum_name(primitive.type));
		return nullptr;
	}

	auto posAttr = primitive.findAttribute("POSITION");
	if (!posAttr) {
		A3D_LOG_E("Primitive has no POSITION attribute.");
		return nullptr;
	}

	const std::size_t posAccessorIndex = posAttr->accessorIndex;
	const auto& posAccessor = asset.accessors[posAccessorIndex];

	const std::size_t vertexCount = posAccessor.count;
	if (vertexCount == 0) {
		A3D_LOG_W("Primitive has 0 vertices.");
		return nullptr;
	}

	auto to_vec3 = [](const fastgltf::math::fvec3& v) -> a3d::math::vec3 {
		return { v[0], v[1], v[2] };
	};
	auto to_vec2 = [](const fastgltf::math::fvec2& v) -> a3d::math::vec2 {
		return { v[0], v[1] };
	};

	std::vector<Vertex> verts(vertexCount);

	// Positions
	iterateAccessorWithIndex<fastgltf::math::fvec3>(
			asset, posAccessor,
			[&](fastgltf::math::fvec3 v, std::size_t i) {
				if (i < verts.size()) verts[i].position = to_vec3(v);
			}
	);

	// Normals (optional)
	if (auto nAttr = primitive.findAttribute("NORMAL"); nAttr) {
		const auto& nAccessor = asset.accessors[nAttr->accessorIndex];
		iterateAccessorWithIndex<fastgltf::math::fvec3>(
				asset, nAccessor,
				[&](fastgltf::math::fvec3 n, std::size_t i) {
					if (i < verts.size()) verts[i].normal = to_vec3(n);
				}
		);
	}

	// Texcoords (optional)
	if (auto uvAttr = primitive.findAttribute("TEXCOORD_0"); uvAttr) {
		const auto& uvAccessor = asset.accessors[uvAttr->accessorIndex];
		iterateAccessorWithIndex<fastgltf::math::fvec2>(
				asset, uvAccessor,
				[&](fastgltf::math::fvec2 uv, std::size_t i) {
					if (i < verts.size()) verts[i].texCoord = to_vec2(uv);
				}
		);
	}

	// Indices (optional)
	std::vector<uint32_t> indices;
	if (primitive.indicesAccessor) {
		const auto& idxAccessor = asset.accessors[*primitive.indicesAccessor];
		indices.resize(idxAccessor.count);
		iterateAccessorWithIndex<uint32_t>(
				asset, idxAccessor,
				[&](uint32_t idx, std::size_t i) {
					if (i < indices.size()) indices[i] = idx;
				}
		);
	} else {
		indices.resize(vertexCount);
		for (std::size_t i = 0; i < vertexCount; ++i)
			indices[i] = static_cast<uint32_t>(i);
	}

	if (indices.size() < 3) {
		A3D_LOG_W("Primitive has fewer than 3 indices; cannot form triangles.");
		return nullptr;
	}
	if (indices.size() % 3 != 0) {
		A3D_LOG_W("Index count {} is not divisible by 3; truncating.", indices.size());
	}

	const std::size_t triIndexCount = (indices.size() / 3) * 3;
	std::vector<Face> faces;
	faces.reserve(triIndexCount / 3);

	for (std::size_t i = 0; i < triIndexCount; i += 3) {
		faces.push_back({
								static_cast<unsigned>(indices[i + 0]),
								static_cast<unsigned>(indices[i + 1]),
								static_cast<unsigned>(indices[i + 2]),
						});
	}

	return std::make_unique<MeshElement>(verts, faces);
}

//unique_ptr<a3d::MeshElement> GlTFImporter::meshElementFromGlTFPrimitive(fastgltf::Asset& asset,
//																		fastgltf::Primitive& primitive) {
//
//	// TODO: make this suck less
//
//	if (primitive.type == PrimitiveType::Triangles) {
//
//		if (auto indiciesAccessorIndex = primitive.indicesAccessor) {
//			auto& indiciesAccessor = asset.accessors[*indiciesAccessorIndex];
//
//			vector<Vertex> verts;
//			vector<Face> faces;
//
//			// faces
//
//			{
//				vector<uint32_t> indices;
//				indices.resize(indiciesAccessor.count);
//
//				iterateAccessorWithIndex<uint32_t>(
//						asset,
//						indiciesAccessor,
//						[&](uint32_t index, size_t idx) {
//							indices[idx] = index;
//						});
//
//				for (size_t i = 0; i < indices.size(); i += 3) {
//					faces.push_back( {unsigned(indices[i + 0]),
//									  unsigned(indices[i + 1]),
//									  unsigned(indices[i + 2])} );
//				}
//			}
//
//			// positions
//
//			{
//				auto attrib = primitive.findAttribute("POSITION");
//
//				auto accessorIndex = attrib->second;
//				auto accessor = asset.accessors[accessorIndex];
//
//				auto type = accessor.type;
//				if (type == AccessorType::Vec3) {
//
//					auto componentType = accessor.componentType;
//					if (componentType == ComponentType::Float) {
//
//						auto& bufferView = asset.bufferViews[*accessor.bufferViewIndex];
//						auto& buffer = asset.buffers[bufferView.bufferIndex];
//						auto& bufferData = buffer.data;
//
//						if (auto vec = std::get_if<sources::Vector>(&bufferData)) {
//
//							auto offset = bufferView.byteOffset + accessor.byteOffset;
//							auto length = bufferView.byteLength;
//							auto elementByteSize = getElementByteSize(type, componentType);
//							auto numElements = length / elementByteSize;
//
//							auto ptr = reinterpret_cast<uint8_t*>(&vec->bytes[offset]);
//							auto stride = bufferView.byteStride
//										  ? *(bufferView.byteStride)
//										  : sizeof(vec3);
//
//							for (size_t i = 0; i < numElements; ++i) {
//								auto v3Ptr = reinterpret_cast<vec3*>(ptr);
//								verts.push_back({*v3Ptr, {}, {}});
//								ptr += stride;
//							}
//						}
//						else {
//							A3D_LOG_W("Unsupported position buffer data.");
//						}
//					}
//					else {
//						A3D_LOG_W("Unsupported position accessor component type: {}",
//								  magic_enum::enum_name(componentType));
//					}
//				}
//				else {
//					A3D_LOG_W("Unsupported position accessor type: {}",
//							  magic_enum::enum_name(type));
//				}
//			}
//
//			// normals
//
//			{
//				auto attrib = primitive.findAttribute("NORMAL");
//
//				auto accessorIndex = attrib->second;
//				auto accessor = asset.accessors[accessorIndex];
//
//				auto type = accessor.type;
//				if (type == AccessorType::Vec3) {
//
//					auto componentType = accessor.componentType;
//					if (componentType == ComponentType::Float) {
//
//						auto& bufferView = asset.bufferViews[*accessor.bufferViewIndex];
//						auto& buffer = asset.buffers[bufferView.bufferIndex];
//						auto& bufferData = buffer.data;
//
//						if (auto vec = std::get_if<sources::Vector>(&bufferData)) {
//
//							auto offset = bufferView.byteOffset + accessor.byteOffset;
//							auto length = bufferView.byteLength;
//							auto elementByteSize = getElementByteSize(type, componentType);
//							auto numElements = length / elementByteSize;
//
//							auto ptr = reinterpret_cast<uint8_t*>(&vec->bytes[offset]);
//							auto stride = bufferView.byteStride
//										  ? *(bufferView.byteStride)
//										  : sizeof(vec3);
//
//							for (size_t i = 0; i < numElements; ++i) {
//								auto v3Ptr = reinterpret_cast<vec3*>(ptr);
//								verts[i].normal = *v3Ptr;
//								ptr += stride;
//							}
//						}
//						else {
//							A3D_LOG_W("Unsupported normals buffer data.");
//						}
//					}
//					else {
//						A3D_LOG_W("Unsupported normals accessor component type: {}",
//								  magic_enum::enum_name(componentType));
//					}
//				}
//				else {
//					A3D_LOG_W("Unsupported normals accessor type: {}",
//							  magic_enum::enum_name(type));
//				}
//			}
//
//			// texture coordinates
//
//			{
//				auto attrib = primitive.findAttribute("TEXCOORD_0");
//
//				auto accessorIndex = attrib->second;
//				auto accessor = asset.accessors[accessorIndex];
//
//				auto type = accessor.type;
//				if (type == AccessorType::Vec2) {
//
//					auto componentType = accessor.componentType;
//					if (componentType == ComponentType::Float) {
//
//						auto& bufferView = asset.bufferViews[*accessor.bufferViewIndex];
//						auto& buffer = asset.buffers[bufferView.bufferIndex];
//						auto& bufferData = buffer.data;
//
//						if (auto vec = std::get_if<sources::Vector>(&bufferData)) {
//
//							auto offset = bufferView.byteOffset + accessor.byteOffset;
//							auto length = bufferView.byteLength;
//							auto elementByteSize = getElementByteSize(type, componentType);
//							auto numElements = length / elementByteSize;
//
//							auto ptr = reinterpret_cast<uint8_t*>(&vec->bytes[offset]);
//							auto stride = bufferView.byteStride
//										  ? *(bufferView.byteStride)
//										  : sizeof(vec2);
//
//							for (size_t i = 0; i < numElements; ++i) {
//								auto v2Ptr = reinterpret_cast<vec2*>(ptr);
//								verts[i].texCoord = *v2Ptr;
//								ptr += stride;
//							}
//						}
//						else {
//							A3D_LOG_W("Unsupported texcoords buffer data.");
//						}
//					}
//					else {
//						A3D_LOG_W("Unsupported texcoords accessor component type: {}",
//								  magic_enum::enum_name(componentType));
//					}
//				}
//				else {
//					A3D_LOG_W("Unsupported texcoords accessor type: {}",
//							  magic_enum::enum_name(type));
//				}
//			}
//
//			return make_unique<MeshElement>(verts, faces);
//		}
//		else {
//			A3D_LOG_E("Missing vertex indicies.");
//		}
//	}
//	else {
//		A3D_LOG_W("Unsupported primitive type: {}",
//				  magic_enum::enum_name(primitive.type));
//	}
//
//	return nullptr;
//}

shared_ptr<a3d::Material> GlTFImporter::materialFromGlTFPrimitive(fastgltf::Asset& asset,
																  fastgltf::Primitive& primitive) {

	if (auto materialIndex = primitive.materialIndex) {

		if (_materials.find(*materialIndex) == _materials.end()) {

			auto& material = asset.materials[*materialIndex];

			A3D_LOG_D("Importing material '{}'...", material.name);

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
					A3D_LOG_I("anisotropyStrength: {}", strength);

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

	return a3d::Material::DefaultMaterial();
}

shared_ptr<a3d::Texture> GlTFImporter::textureFromGlTFTextureIndex(fastgltf::Asset& asset,
																   std::size_t textureIndex) {

	if (_textures.find(textureIndex) == _textures.end()) {

		auto& texture = asset.textures[textureIndex];

		A3D_LOG_D("Importing texture '{}'...", texture.name);

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

			A3D_LOG_D("Importing sampler '{}'...", sampler.name);

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
			auto &dataSource = image.data;

			A3D_LOG_D("Importing image '{}'...", image.name);

//			auto a3dImage = std::visit([&asset](auto&& dataSource) -> shared_ptr<Image> {
//
//				using T = std::decay_t<decltype(dataSource)>;
//
//				if constexpr (std::is_same_v<T, sources::Vector>) { // .gltf
//
//					A3D_LOG_D("Creating texture image...");
//
//					auto uint8Vec = dataSource.bytes;
//					auto a3dBuffer = make_unique<a3d::Buffer>(reinterpret_cast<std::byte*>(uint8Vec.data()), uint8Vec.size());
//					return make_shared<a3d::Image>(std::move(a3dBuffer), false);
//				}
//				else if constexpr (std::is_same_v<T, sources::BufferView>) { // .glb
//
//					auto bufferViewIndex = dataSource.bufferViewIndex;
//					auto& bufferView = asset.bufferViews[bufferViewIndex];
//
//					if (auto byteStride = bufferView.byteStride) { // TODO: is this unpacked for us?
//
//						A3D_LOG_W("Texture buffer has stride: {}.  Skipping.", *(bufferView.byteStride));
//						return nullptr;
//					}
//					else {
//						A3D_LOG_D("Creating texture image...");
//
//						auto buffer = asset.buffers[bufferView.bufferIndex];
//						auto byteOffset = bufferView.byteOffset;
//						auto byteLength = bufferView.byteLength;
//
//						if (auto uint8Vec = std::get_if<sources::Vector>(&buffer.data))
//						{
//							auto a3dBuffer = make_unique<a3d::Buffer>(reinterpret_cast<std::byte*>(&uint8Vec[byteOffset]), byteLength);
//							return make_shared<a3d::Image>(std::move(a3dBuffer), false);
//						}
//						else {
//							A3D_LOG_W("Unexpected texture data.");
//							return nullptr;
//						}
//
////						if (auto bufferData = buffer.data; holds_alternative<sources::Vector>(bufferData)) {
////
////							auto uint8Vec = get<sources::Vector>(bufferData).bytes;
////							auto a3dBuffer = make_unique<a3d::Buffer>(reinterpret_cast<std::byte*>(&uint8Vec[byteOffset]), byteLength);
////							return make_shared<a3d::Image>(std::move(a3dBuffer), false);
////						}
////						else {
////							A3D_LOG_W("Unexpected texture data.");
////							return nullptr;
////						}
//					}
//				}
//				else if constexpr (std::is_same_v<T, std::monostate>) {
//
//					A3D_LOG_W("Unexpected texture data.");
//					return nullptr;
//				}
//
//				return nullptr;
//
//			}, dataSource);

			auto a3dImage = std::visit(fastgltf::visitor{
					[&](const fastgltf::sources::Vector& v) -> std::shared_ptr<a3d::Image> {
						auto bytes = std::span<const std::byte>(v.bytes.data(), v.bytes.size());
						auto a3dBuffer = std::make_unique<a3d::Buffer>(
								const_cast<std::byte*>(bytes.data()), bytes.size()); // ideally make Buffer accept const
						return std::make_shared<a3d::Image>(std::move(a3dBuffer), false);
					},
					[&](const fastgltf::sources::BufferView& bvSrc) -> std::shared_ptr<a3d::Image> {
						auto bytes = BytesFromBufferView(asset, bvSrc.bufferViewIndex);
						if (bytes.empty()) {
							A3D_LOG_W("Unexpected/empty BufferView image data.");
							return nullptr;
						}

						auto a3dBuffer = std::make_unique<a3d::Buffer>(
								const_cast<std::byte*>(bytes.data()), bytes.size());
						return std::make_shared<a3d::Image>(std::move(a3dBuffer), false);
					},
					[&](const fastgltf::sources::Array& a) -> std::shared_ptr<a3d::Image> {
						auto bytes = std::span<const std::byte>(a.bytes.data(), a.bytes.size());
						auto a3dBuffer = std::make_unique<a3d::Buffer>(
								const_cast<std::byte*>(bytes.data()), bytes.size());
						return std::make_shared<a3d::Image>(std::move(a3dBuffer), false);
					},
					[&](const fastgltf::sources::ByteView& bv) -> std::shared_ptr<a3d::Image> {
						auto bytes = std::span<const std::byte>(bv.bytes.data(), bv.bytes.size());
						auto a3dBuffer = std::make_unique<a3d::Buffer>(
								const_cast<std::byte*>(bytes.data()), bytes.size());
						return std::make_shared<a3d::Image>(std::move(a3dBuffer), false);
					},
					[&](const fastgltf::sources::URI&) -> std::shared_ptr<a3d::Image> {
						// If you pass Options::LoadExternalImages, you usually won't see URI here,
						// but keep this log because it indicates you didn’t load image bytes.
						A3D_LOG_W("Image is still a URI; enable Options::LoadExternalImages (and provide base dir).");
						return nullptr;
					},
					[&](const auto&) -> std::shared_ptr<a3d::Image> {
						A3D_LOG_W("Unsupported image DataSource.");
						return nullptr;
					}
			}, image.data);

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

			A3D_LOG_D("Importing light '{}'...", light.name);

			if (type == fastgltf::LightType::Directional) {

				auto a3dLight = make_shared<DirectionalLight>(string(light.name));
				a3dLight->color(ColorFromGlTFColorArray(light.color));
				// TODO: range, intensity?
				_lights[*lightIndex] = a3dLight;
				return a3dLight;
			}
			else if (type == fastgltf::LightType::Point) {

				auto a3dLight = make_shared<PointLight>(string(light.name));
				a3dLight->color(ColorFromGlTFColorArray(light.color));
				a3dLight->constantAttenuation(1.0); // temporary?
				// TODO: range, intensity?
				_lights[*lightIndex] = a3dLight;
				return a3dLight;
			}
			else if (type == fastgltf::LightType::Spot) {

				auto a3dLight = make_shared<SpotLight>(string(light.name));
				a3dLight->color(ColorFromGlTFColorArray(light.color));
				a3dLight->constantAttenuation(1.0); // temporary?
				a3dLight->innerAngle(light.innerConeAngle.value());
				a3dLight->outerAngle(light.outerConeAngle.value());
				// TODO: range, intensity?
				_lights[*lightIndex] = a3dLight;
				return a3dLight;
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
			auto name = string(camera.name);

			A3D_LOG_D("Importing camera '{}'...", name);

			if (auto persCamera = std::get_if<fastgltf::Camera::Perspective>(&camera.camera))
			{
				auto a3dCamera = make_shared<PerspectiveCamera>(name,
																persCamera->znear,
																(persCamera->zfar
																 ? *persCamera->zfar
																 : 1000000), // cheating
																persCamera->yfov);

				if (auto ratio = persCamera->aspectRatio) {
					a3dCamera->aspectRatio(*ratio);
				}

				_cameras[*cameraIndex] = a3dCamera;
				return a3dCamera;
			}
			else {
				A3D_LOG_W("Orthographic cameras are not supported.");
			}
		}
		else {
			return _cameras[*cameraIndex];
		}
	}

	return nullptr;
}

/// Private Static Non-Member Functions ///

fastgltf::Options GlTFOptionsFromImportOptions(SceneImportOptions options) {

	auto gltfOptions = Options::None;

	// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?

	if ((options & SceneImportOptions::ImportMeshes) != SceneImportOptions::None) {
		gltfOptions |= Options::LoadExternalBuffers
					   | Options::GenerateMeshIndices;
	}

	if ((options & SceneImportOptions::ImportMaterials) != SceneImportOptions::None) {
		gltfOptions |= Options::LoadExternalBuffers
					   | Options::LoadExternalImages;
	}

	if ((options & SceneImportOptions::ImportLights) != SceneImportOptions::None) {

	}

	if ((options & SceneImportOptions::ImportCameras) != SceneImportOptions::None) {

	}

	return gltfOptions;
}

static std::span<const byte> BytesFromDataSource(const DataSource& src) {
	return std::visit(visitor {
			[](const sources::Vector& v) -> std::span<const byte> {
				return { v.bytes.data(), v.bytes.size() };
			},
			[](const fastgltf::sources::Array& a) -> std::span<const byte> {
				return { a.bytes.data(), a.bytes.size() };
			},
			[](const sources::ByteView& bv) -> std::span<const byte> {
				// In newer fastgltf, ByteView is commonly used by mapped-file loaders.
				// Most builds expose it as a (ptr,len) pair or span-like member; adjust field names if needed.
				return { bv.bytes.data(), bv.bytes.size() };
			},
			[](const auto&) -> std::span<const byte> {
				return {};
			}
	}, src);
}

static std::span<const byte> BytesFromBufferView(const Asset& asset, size_t bufferViewIndex) {
	const auto& bv = asset.bufferViews[bufferViewIndex];
	const auto& buf = asset.buffers[bv.bufferIndex];

	auto base = BytesFromDataSource(buf.data);
	if (base.empty()) return {};

	const std::size_t begin = bv.byteOffset;
	const std::size_t len   = bv.byteLength;

	if (begin + len > base.size()) return {};
	return base.subspan(begin, len);
}


//mat4 TransformFromGlTFNode(fastgltf::Node& node) {
//
//	auto transform = node.transform;
//
//
//	return std::visit([&node](auto&& transform) -> mat4 {
//
//		using T = std::decay_t<decltype(transform)>;
//
//		if constexpr (std::is_same_v<T, fastgltf::Node::TRS>) {
//
//			//auto trs = get<fastgltf::Node::TRS>(transform);
//			const auto& id4 = mat4(1.0);
//
//			auto t = translate(id4, { transform.translation[0],
//										   transform.translation[1],
//										   transform.translation[2] });
//
//			auto r = mat4_cast(quat{ transform.rotation[3],
//										  transform.rotation[0],
//										  transform.rotation[1],
//										  transform.rotation[2] });
//
//			auto s = scale(id4, { transform.scale[0],
//									   transform.scale[1],
//									   transform.scale[2] });
//
//			return t * r * s;
//		}
//		else if constexpr (std::is_same_v<T, fastgltf::Node::TransformMatrix>) {
//
//			//auto matrix = get<fastgltf::Node::TransformMatrix>(transform);
//			return make_mat4(&transform[0]);
//		}
//		else {
//			A3D_LOG_W("No transform associated with node: {}", node.name);
//		}
//
//		return mat4(1.0);
//
//	}, transform);
//
//
////	if (holds_alternative<fastgltf::Node::TRS>(transform)) {
////
////		auto trs = get<fastgltf::Node::TRS>(transform);
////		const auto& id4 = mat4(1.0);
////
////		auto t = translate(id4, { trs.translation[0],
////									   trs.translation[1],
////									   trs.translation[2] });
////
////		auto r = mat4_cast(quat{ trs.rotation[3],
////									  trs.rotation[0],
////									  trs.rotation[1],
////									  trs.rotation[2] });
////
////		auto s = scale(id4, { trs.scale[0],
////								   trs.scale[1],
////								   trs.scale[2] });
////
////		return t * r * s;
////	}
////	else if (holds_alternative<fastgltf::Node::TransformMatrix>(transform)) {
////
////		auto matrix = get<fastgltf::Node::TransformMatrix>(transform);
////		return make_mat4(&matrix[0]);
////	}
////	else {
////		A3D_LOG_W("No transform associated with node: {}", node.name);
////	}
////
////	return mat4(1.0);
//}

mat4 TransformFromGlTFNode(fastgltf::Node& node) {

	// fastgltf 0.9: node.transform is not the old nested TRS/TransformMatrix types.
	// Use the helper that returns a matrix for either representation.
	const fastgltf::math::fmat4x4 m = fastgltf::getTransformMatrix(node);

	// Convert fastgltf matrix to your mat4.
	// This matches your old make_mat4(&matrix[0]) pattern, just with 2D indexing.
	return make_mat4(&m[0][0]);
}

//shared_ptr<a3d::Color> ColorFromGlTFColorArray(array<float, 3>& arr) {
//
//	return make_shared<Color>(vec3{arr[0], arr[1], arr[2]});
//}
//
//shared_ptr<a3d::Color> ColorFromGlTFColorArray(array<float, 4>& arr) {
//
//	array<float, 3> rgbArray = {arr[0], arr[1], arr[2]};
//	return ColorFromGlTFColorArray(rgbArray);
//}

shared_ptr<a3d::Color> ColorFromGlTFColorArray(const std::array<float, 3>& arr) {
	return make_shared<Color>(vec3{arr[0], arr[1], arr[2]});
}

shared_ptr<a3d::Color> ColorFromGlTFColorArray(const std::array<float, 4>& arr) {
	return make_shared<Color>(vec3{arr[0], arr[1], arr[2]});
}

// fastgltf 0.9 math vectors:
shared_ptr<a3d::Color> ColorFromGlTFColorArray(const fastgltf::math::nvec3& v) {
	return make_shared<Color>(vec3{v[0], v[1], v[2]});
}

shared_ptr<a3d::Color> ColorFromGlTFColorArray(const fastgltf::math::nvec4& v) {
	return make_shared<Color>(vec3{v[0], v[1], v[2]});
}
