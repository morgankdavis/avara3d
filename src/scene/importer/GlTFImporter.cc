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

#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/util.hpp>
#include <magic_enum/magic_enum.hpp>

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/Image.h"
#include "a3d/exception/UnsupportedFormatException.h"
#include "a3d/log/Log.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/IndexTypes.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/chrono.h"
#include "a3d/visual/camera/Camera.h"
#include "a3d/visual/camera/PerspectiveCamera.h"
#include "a3d/visual/light/DirectionalLight.h"
#include "a3d/visual/light/Light.h"
#include "a3d/visual/light/PointLight.h"
#include "a3d/visual/light/SpotLight.h"
#include "a3d/visual/material/Material.h"
#include "a3d/visual/material/Sampler.h"
#include "a3d/visual/material/Texture.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Private Typees ///

//struct ImportedMeshElement final : a3d::MeshElement {
//	ImportedMeshElement() : a3d::MeshElement() {}
//	void finalize() { genLocalAABB(); }
//};

/// Private Static Non-Member Prototypes ///

static fastgltf::Options GlTFOptionsFromImportOptions(Scene::ImportOptions options);
static std::span<const byte> BytesFromDataSource(const fastgltf::DataSource& src);
static std::span<const byte> BytesFromBufferView(const fastgltf::Asset& asset, size_t bufferViewIndex);
static mat4 TransformFromGlTFNode(fastgltf::Node& node);
static shared_ptr<a3d::Color> ColorFromGlTFColorArray(const fastgltf::math::nvec3& v);
static shared_ptr<a3d::Color> ColorFromGlTFColorArray(const fastgltf::math::nvec4& v);
static void ReadIndicesU32(const fastgltf::Asset& asset,
						   const fastgltf::Accessor& idxAccessor,
						   vector<uint32_t>& out);

/// Internal Lifecycle Functions ///

GlTFImporter::GlTFImporter(const filesystem::path& path,
						   Scene::ImportOptions options):
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

			auto startTime = util::chrono::Time();

			auto a3dScene = make_unique<a3d::Scene>();

			auto& scenes = _asset.scenes;
			if (!scenes.empty()) {

				if (scenes.size() > 1) {
					log::w()("Ignoring extra scenes.");
				}

				auto& scene = scenes[_asset.defaultScene ? *_asset.defaultScene : 0];

				a3dScene->name(string(scene.name));

				auto nodeIndicies = scene.nodeIndices;
				if (!nodeIndicies.empty()) {

					for (auto n: nodeIndicies) {
						visitGlTFNode(_asset, _asset.nodes[n], a3dScene->rootNode().get());
					}

					// TODO: throw out nodes that don't have anything attached to them, or any children?

					log::i()("Done loading scene.  Time: {}", util::chrono::Time() - startTime);

					_scene = std::move(a3dScene);
				}
				else {
					log::e()("No nodes in scene: {}", scene.name);
				}
			}
			else {
				log::e()("File contains no scenes!");
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

		auto startTime = util::chrono::Time();

		auto& meshes = _asset.meshes;
		if (!meshes.empty()) {

			mesh = meshFromGlTFMeshIndex(_asset, 0);

			if (mesh) {
				log::i()("Done loading mesh.  Time: {}", util::chrono::Time() - startTime);
			}
		}
		else {
			log::e()("File contains no meshes!");
		}
	}

	return mesh;
}

const filesystem::path& GlTFImporter::path() const {
	return _path;
}

Scene::ImportOptions GlTFImporter::options() const {
	return _options;
}

/// Private Member Functions ///

bool GlTFImporter::parse() {

	using namespace fastgltf;

	if (!_parsed) {

		auto startTime = util::chrono::Time();

		log::i()("Parsing glTF: '{}'...", _path.string());

		auto extensions = Extensions::KHR_lights_punctual
						  | Extensions::KHR_materials_specular
						  | Extensions::KHR_materials_anisotropy
						  | Extensions::KHR_texture_transform;
		auto parser = Parser(extensions);

		auto gltfFile = MappedGltfFile::FromPath(_path);
		if (!bool(gltfFile)) {
			log::e()("Failed to open glTF file: {}", getErrorMessage(gltfFile.error()));
			return false;
		}

		auto directory = _path.parent_path();
		auto options = GlTFOptionsFromImportOptions(_options);
		auto asset = parser.loadGltf(gltfFile.get(), directory, options);
		if (asset.error() != fastgltf::Error::None) {
			log::e()("Failed to load glTF {}", getErrorMessage(asset.error()));
			return false;
		}

		_asset = std::move(asset.get());
		_parsed = true; // <-- IMPORTANT
		log::i()("Parsed glTF. Time: {}", util::chrono::Time() - startTime);
	}

	return true;
}

void GlTFImporter::visitGlTFNode(fastgltf::Asset& asset,
								 fastgltf::Node& node,
								 a3d::Node* parent) {

	// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?

	auto a3dNode = Node::NamedNode(string(node.name));

	a3dNode->transform(TransformFromGlTFNode(node));

	if ((_options & Scene::ImportOptions::ImportMeshes) != Scene::ImportOptions::None) {
		a3dNode->mesh(meshFromGlTFNode(asset, node));
	}

	if ((_options & Scene::ImportOptions::ImportLights) != Scene::ImportOptions::None) {
		a3dNode->light(lightFromGlTFNode(asset, node));
	}

	// (during .6.1 -> .9 migration): this was disabled... why...?
	if ((_options & Scene::ImportOptions::ImportCameras) != Scene::ImportOptions::None) {
		a3dNode->camera(cameraFromGlTFNode(asset, node));
	}

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
														  size_t meshIndex) {

	if (_meshes.find(meshIndex) == _meshes.end()) {

		auto& mesh = asset.meshes[meshIndex];

		log::d()("Importing mesh '{}'...", mesh.name);

		auto elements = vector<unique_ptr<MeshElement>>();
		auto materials = vector<shared_ptr<Material>>();

		for (auto& primitive: mesh.primitives) {

			auto element = meshElementFromGlTFPrimitive(asset, primitive);
			if (element) elements.push_back(std::move(element));

			// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?
			auto material = ((_options & Scene::ImportOptions::ImportMaterials) != Scene::ImportOptions::None)
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
}

unique_ptr<a3d::MeshElement> GlTFImporter::meshElementFromGlTFPrimitive(
		fastgltf::Asset& asset,
		fastgltf::Primitive& primitive) {

	using namespace fastgltf;

	if (primitive.type != PrimitiveType::Triangles) {
		log::w()("Unsupported primitive type: {}", magic_enum::enum_name(primitive.type));
		return nullptr;
	}

	auto posAttr = primitive.findAttribute("POSITION");
	if (!posAttr) {
		log::e()("Primitive has no POSITION attribute.");
		return nullptr;
	}

	const size_t posAccessorIndex = posAttr->accessorIndex;
	const auto& posAccessor = asset.accessors[posAccessorIndex];

	const size_t vertexCount = posAccessor.count;
	if (vertexCount == 0) {
		log::w()("Primitive has 0 vertices.");
		return nullptr;
	}

	auto to_vec3 = [](const fastgltf::math::fvec3& v) -> a3d::math::vec3 {
		return { v[0], v[1], v[2] };
	};

	vector<VertexPNT> verts(vertexCount);

	// positions
	iterateAccessorWithIndex<fastgltf::math::fvec3>(
			asset, posAccessor,
			[&](fastgltf::math::fvec3 v, size_t i) {
				if (i < verts.size()) verts[i].position = to_vec3(v);
			}
	);

	// normals
	if (auto nAttr = primitive.findAttribute("NORMAL"); nAttr) {
		const auto& nAccessor = asset.accessors[nAttr->accessorIndex];
		iterateAccessorWithIndex<fastgltf::math::fvec3>(
				asset, nAccessor,
				[&](fastgltf::math::fvec3 n, size_t i) {
					if (i < verts.size()) verts[i].normal = to_vec3(n);
				}
		);
	}

	// texcoords
	if (auto uvAttr = primitive.findAttribute("TEXCOORD_0"); uvAttr) {
		const auto& uvAccessor = asset.accessors[uvAttr->accessorIndex];

		using fastgltf::AccessorType;
		using fastgltf::math::fvec2;
		using fastgltf::math::fvec3;
		using fastgltf::math::fvec4;

		switch (uvAccessor.type) {
			case AccessorType::Vec2: {
				iterateAccessorWithIndex<fvec2>(asset, uvAccessor,
												[&](fvec2 uv, size_t i) {
													if (i < verts.size()) {
														verts[i].texCoord = { uv[0], uv[1] };
													}
												});
				break;
			}
			case AccessorType::Vec3: {
				// non-compliant glTF...
				log::w()("TEXCOORD_0 is VEC3; truncating to (u,v).");
				iterateAccessorWithIndex<fvec3>(asset, uvAccessor,
												[&](fvec3 uvw, size_t i) {
													if (i < verts.size()) {
														verts[i].texCoord = { uvw[0], uvw[1] };
													}
												});
				break;
			}
			case AccessorType::Vec4: {
				// non-compliant glTF...
				log::w()("TEXCOORD_0 is VEC4; truncating to (u,v).");
				iterateAccessorWithIndex<fvec4>(asset, uvAccessor,
												[&](fvec4 uvzw, size_t i) {
													if (i < verts.size()) {
														verts[i].texCoord = { uvzw[0], uvzw[1] };
													}
												});
				break;
			}
			default: {
				log::w()("TEXCOORD_0 has unexpected accessor.type={} (expected VEC2). Skipping UVs.",
						 magic_enum::enum_name(uvAccessor.type));
				break;
			}
		}
	}

	// indices (read U32 then optionally down-pack to U16)
	vector<uint32_t> rawIndices;
	if (primitive.indicesAccessor) {
		const auto& idxAccessor = asset.accessors[*primitive.indicesAccessor];
		ReadIndicesU32(asset, idxAccessor, rawIndices);
		if (rawIndices.empty()) {
			log::w()("Primitive indices accessor present but unreadable: skipping primitive.");
			return nullptr;
		}
	}
	else {
		// glTF "non-indexed": treat as sequential indexing
		rawIndices.resize(vertexCount);
		for (size_t i = 0; i < vertexCount; ++i) {
			rawIndices[i] = static_cast<uint32_t>(i);
		}
	}

	if (rawIndices.size() < 3) {
		log::w()("Primitive has fewer than 3 indices: cannot form triangles.");
		return nullptr;
	}
	if ((rawIndices.size() % 3) != 0) {
		log::w()("Index count {} is not divisible by 3: truncating.", rawIndices.size());
	}

	const size_t triIndexCount = (rawIndices.size() / 3) * 3;

	// validate triangles and build a clean index list
	vector<uint32_t> cleanIndices;
	cleanIndices.reserve(triIndexCount);

	for (size_t i = 0; i < triIndexCount; i += 3) {
		const uint32_t a = rawIndices[i + 0];
		const uint32_t b = rawIndices[i + 1];
		const uint32_t c = rawIndices[i + 2];

		// don’t create garbage faces if a file is malformed
		if (a >= vertexCount || b >= vertexCount || c >= vertexCount) {
			log::w()("Triangle index out of range ({} {} {}) for vertexCount: {}. Skipping triangle.",
					 a, b, c, vertexCount);
			continue;
		}

		cleanIndices.push_back(a);
		cleanIndices.push_back(b);
		cleanIndices.push_back(c);
	}

	if (cleanIndices.size() < 3) {
		log::w()("Primitive produced 0 valid triangles after validation.");
		return nullptr;
	}

	// choose smallest index format that can represent max index
	uint32_t maxIndex = 0;
	for (uint32_t v : cleanIndices) {
		if (v > maxIndex) maxIndex = v;
	}

	IndexFormat indexFormat = (maxIndex <= 0xFFFFu) ? IndexFormat::U16 : IndexFormat::U32;

	// vertex bytes
	const auto vb = std::as_bytes(std::span<const VertexPNT>(verts.data(), verts.size()));

	if (indexFormat == IndexFormat::U16) {
		vector<uint16_t> idx16;
		idx16.reserve(cleanIndices.size());
		for (uint32_t v : cleanIndices) {
			// guaranteed safe by maxIndex check
			idx16.push_back(static_cast<uint16_t>(v));
		}

		const auto ib = std::as_bytes(std::span<const uint16_t>(idx16.data(), idx16.size()));

		return make_unique<MeshElement>(
				VertexLayout::PNT,
				vb,
				static_cast<uint32_t>(verts.size()),
				static_cast<uint16_t>(sizeof(VertexPNT)),
				PrimitiveTopology::Triangles,
				IndexFormat::U16,
				ib,
				static_cast<uint32_t>(idx16.size()));
	}
	else {
		const auto ib = std::as_bytes(std::span<const uint32_t>(cleanIndices.data(), cleanIndices.size()));

		return make_unique<MeshElement>(
				VertexLayout::PNT,
				vb,
				static_cast<uint32_t>(verts.size()),
				static_cast<uint16_t>(sizeof(VertexPNT)),
				PrimitiveTopology::Triangles,
				IndexFormat::U32,
				ib,
				static_cast<uint32_t>(cleanIndices.size()));
	}
}

shared_ptr<a3d::Material> GlTFImporter::materialFromGlTFPrimitive(fastgltf::Asset& asset,
																  fastgltf::Primitive& primitive) {

	if (auto materialIndex = primitive.materialIndex) {

		if (_materials.find(*materialIndex) == _materials.end()) {

			auto& material = asset.materials[*materialIndex];

			log::d()("Importing material '{}'...", material.name);

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
				auto a3dProperty = Material::Property(a3dColor);
				a3dMaterial = make_shared<a3d::Material>(monostate{}, a3dProperty, monostate{});
			}

			// specular

			if (auto& specularMaterial = material.specular; specularMaterial) {

				// TODO: this needs work.  and more testing.

				auto factor = specularMaterial->specularFactor;
				auto& textureInfo = specularMaterial->specularTexture;
				auto colorFactor = specularMaterial->specularColorFactor;
				auto& colorTextureInfo = specularMaterial->specularColorTexture;

//				log::d()("*** [SPECULAR] ***");
//				log::d()("factor: {}", factor);
//				log::d()("textureInfo: {}", textureInfo ? "true" : "false");
//				log::d()("colorFactor: ({}, {}, {})", colorFactor[0], colorFactor[1], colorFactor[2]);
//				log::d()("colorTextureInfo: {}", colorTextureInfo ? "true" : "false");

				// if it has a 'texture' map, use it (only contains alpha)
				// if it has no map, but a 'factor' use solid white, with an intentify of the factor.

				Material::Property a3dProperty = monostate{};

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
					a3dProperty = Material::Property(factorColor);
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
					log::i()("anisotropyStrength: {}", strength);

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
		log::w()("Missing material.");
	}

	return a3d::Material::DefaultMaterial();
}

shared_ptr<a3d::Texture> GlTFImporter::textureFromGlTFTextureIndex(fastgltf::Asset& asset,
																   std::size_t textureIndex) {

	if (_textures.find(textureIndex) == _textures.end()) {

		auto& texture = asset.textures[textureIndex];

		log::d()("Importing texture '{}'...", texture.name);

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

			log::d()("Importing sampler '{}'...", sampler.name);

			auto a3dSampler = make_shared<a3d::Sampler>();

			if (sampler.minFilter) {
				a3dSampler->minificationFilter(Sampler::FilterMode(*sampler.minFilter));
			}
			if (sampler.magFilter) {
				a3dSampler->magnificationFilter(Sampler::FilterMode(*sampler.magFilter));
			}
			a3dSampler->wrapS(Sampler::WrapMode(sampler.wrapS));
			a3dSampler->wrapT(Sampler::WrapMode(sampler.wrapT));

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

			log::d()("Importing image '{}'...", image.name);

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
							log::w()("Unexpected/empty BufferView image data.");
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
						// but keep this log because it indicates you didn’t load image bytes.
						log::w()("Image is a URI -- is Options::LoadExternalImages enabled?");
						return nullptr;
					},
					[&](const auto&) -> std::shared_ptr<a3d::Image> {
						log::w()("Unsupported image DataSource.");
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

			log::d()("Importing light '{}'...", light.name);

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

			log::d()("Importing camera '{}'...", name);

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
				log::w()("Orthographic cameras are not supported.");
			}
		}
		else {
			return _cameras[*cameraIndex];
		}
	}

	return nullptr;
}

/// Private Static Non-Member Functions ///

fastgltf::Options GlTFOptionsFromImportOptions(Scene::ImportOptions options) {

	using namespace fastgltf;
	using ImportOptions = a3d::Scene::ImportOptions;

	auto gltfOptions = Options::None;

	// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?

	if ((options & ImportOptions::ImportMeshes) != ImportOptions::None) {
		gltfOptions |= Options::LoadExternalBuffers
					   | Options::GenerateMeshIndices;
	}

	if ((options & ImportOptions::ImportMaterials) != ImportOptions::None) {
		gltfOptions |= Options::LoadExternalBuffers
					   | Options::LoadExternalImages;
	}

	if ((options & ImportOptions::ImportLights) != ImportOptions::None) {

	}

	if ((options & ImportOptions::ImportCameras) != ImportOptions::None) {

	}

	return gltfOptions;
}

static std::span<const byte> BytesFromDataSource(const fastgltf::DataSource& src) {
	return std::visit(fastgltf::visitor {
			[](const fastgltf::sources::Vector& v) -> std::span<const byte> {
				return { v.bytes.data(), v.bytes.size() };
			},
			[](const fastgltf::sources::Array& a) -> std::span<const byte> {
				return { a.bytes.data(), a.bytes.size() };
			},
			[](const fastgltf::sources::ByteView& bv) -> std::span<const byte> {
				// In newer fastgltf, ByteView is commonly used by mapped-file loaders.
				// Most builds expose it as a (ptr,len) pair or span-like member; adjust field names if needed.
				return { bv.bytes.data(), bv.bytes.size() };
			},
			[](const auto&) -> std::span<const byte> {
				return {};
			}
	}, src);
}

static std::span<const byte> BytesFromBufferView(const fastgltf::Asset& asset, size_t bufferViewIndex) {
	const auto& bv = asset.bufferViews[bufferViewIndex];
	const auto& buf = asset.buffers[bv.bufferIndex];

	auto base = BytesFromDataSource(buf.data);
	if (base.empty()) return {};

	const std::size_t begin = bv.byteOffset;
	const std::size_t len   = bv.byteLength;

	if (begin + len > base.size()) return {};
	return base.subspan(begin, len);
}

mat4 TransformFromGlTFNode(fastgltf::Node& node) {

	const fastgltf::math::fmat4x4 m = fastgltf::getTransformMatrix(node);
	return a3d::math::make_mat4(&m[0][0]);
}

shared_ptr<a3d::Color> ColorFromGlTFColorArray(const fastgltf::math::nvec3& v) {
	return make_shared<Color>(vec3{v[0], v[1], v[2]});
}

shared_ptr<a3d::Color> ColorFromGlTFColorArray(const fastgltf::math::nvec4& v) {
	return make_shared<Color>(vec3{v[0], v[1], v[2]});
}

void ReadIndicesU32(const fastgltf::Asset& asset,
					const fastgltf::Accessor& idxAccessor,
					std::vector<uint32_t>& out) {

	using fastgltf::ComponentType;

	out.assign(idxAccessor.count, 0);

	switch (idxAccessor.componentType) {
		case ComponentType::UnsignedByte: {
			fastgltf::iterateAccessorWithIndex<uint8_t>(asset, idxAccessor,
														[&](uint8_t v, size_t i) { if (i < out.size()) out[i] = (uint32_t)v; });
			break;
		}
		case ComponentType::UnsignedShort: {
			fastgltf::iterateAccessorWithIndex<uint16_t>(asset, idxAccessor,
														 [&](uint16_t v, size_t i) { if (i < out.size()) out[i] = (uint32_t)v; });
			break;
		}
		case ComponentType::UnsignedInt: {
			fastgltf::iterateAccessorWithIndex<uint32_t>(asset, idxAccessor,
														 [&](uint32_t v, size_t i) { if (i < out.size()) out[i] = v; });
			break;
		}
		default:
			log::w()("Unsupported index componentType: {} (expected U8/U16/U32).",
					 magic_enum::enum_name(idxAccessor.componentType));
			out.clear();
			break;
	}
}
