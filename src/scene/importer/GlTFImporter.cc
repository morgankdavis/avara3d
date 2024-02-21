//
// Created by mkd on 1/30/24.
//

#include "ae/scene/importer/GlTFImporter.h"


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
#include "ae/diagnostic/exception/UnsupportedFormat.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/geometry/Geometry.h"
#include "ae/geometry/GeometryElement.h"
#include "ae/rendering/Light.h"
#include "ae/rendering/camera/Camera.h"
#include "ae/rendering/camera/PerspectiveCamera.h"
#include "ae/rendering/material/Material.h"
#include "ae/rendering/material/Sampler.h"
#include "ae/rendering/material/Texture.h"
#include "ae/scene/Node.h"
#include "ae/scene/Scene.h"


using namespace ae;
using namespace fastgltf;
using namespace glm;
using namespace std;


static fastgltf::Options GlTFOptionsFromImportOptions(SceneImportOptions options);
//static shared_ptr<Geometry> GeometryFromGlFTMeshIndex(fastgltf::Asset& asset,
//													  size_t meshIndex);
static mat4 TransformFromGlFTNode(fastgltf::Node& node);
static shared_ptr<ae::Color> ColorFromGlTFColorArray(array<float, 3>& arr);
static shared_ptr<ae::Color> ColorFromGlTFColorArray(array<float, 4>& arr);


GlTFImporter::GlTFImporter(const filesystem::path& path,
						   SceneImportOptions options):
		_parsed{false},
		_asset{},
		_scene{nullptr},
		_path{path},
		_options{options},
		_cameras{},
		_geometries{},
		_images{},
		_lights{},
		_materials{},
		/*_materialProperties{}*/

		_samplers{},
		_textures{} {

	auto extension = path.extension();
	if (!(extension == ".gltf" || extension == ".glb")) {
		throw UnsupportedFormat(fmt::format("Unsupported format: {}", extension.string()));
	}
}

shared_ptr<ae::Scene> GlTFImporter::scene() {

	if (!_scene) {
		if (parse()) {

			auto startTime = Scene::Time();

			auto aeScene = make_shared<ae::Scene>();

			auto& scenes = _asset.scenes;
			if (!scenes.empty()) {

				if (scenes.size() > 1) {
					AE_LOG_W("Ignoring extra scenes.");
				}

				auto& scene = scenes[_asset.defaultScene ? *_asset.defaultScene : 0];

				auto nodeIndicies = scene.nodeIndices;
				if (!nodeIndicies.empty()) {

					for (auto n: nodeIndicies) {
						visitGlTFNode(_asset, _asset.nodes[n], aeScene->rootNode());
					}

					// TODO: throw out nodes that don't have anything attached to them, or any children?

					AE_LOG_I("Done loading scene.  Time: {}", Scene::Time() - startTime);

					_scene = aeScene;
				}
				else {
					AE_LOG_E("No nodes in scene: {}", scene.name);
				}
			}
			else {
				AE_LOG_E("File contains no scenes!");
			}
		}
	}

	return _scene;
}

shared_ptr<Geometry> GlTFImporter::firstGeometry() {

	// unlike scene(), no need to cache the geometry explicitly
	// because it will quickly be looked up in _geometries

	shared_ptr<Geometry> geometry = nullptr;

	if (parse()) {

		auto startTime = Scene::Time();

		auto& meshes = _asset.meshes;
		if (!meshes.empty()) {

			geometry = geometryFromGlFTMeshIndex(_asset, 0);

			if (geometry) {
				AE_LOG_I("Done loading geometry.  Time: {}", Scene::Time() - startTime);
			}
		}
		else {
			AE_LOG_E("File contains no meshes!");
		}
	}

	return geometry;
}

const filesystem::path& GlTFImporter::path() const {
	return _path;
}

SceneImportOptions GlTFImporter::options() const {
	return _options;
}

bool GlTFImporter::parse() {

	if (!_parsed) {

		AE_LOG_I("Parsing glTF: '{}'...", _path.string());

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
			AE_LOG_E("Unsupported file extension: {}", extension.string());
		}

		if (auto error = expectedAsset.error(); error == Error::None) {

			auto& asset = expectedAsset.get();

			if (auto& info = asset.assetInfo) {
				AE_LOG_D("Done parsing glTF.  Version: '{}', Copyright: '{}', Generator: '{}'.  Parse time: {}",
						 info->gltfVersion, info->copyright, info->generator, Scene::Time() - startTime);
			}
			else {
				AE_LOG_D("Done parsing glTF.  Time: {}", Scene::Time() - startTime);
			}

			_asset = std::move(expectedAsset.get());
		}
		else {

			AE_LOG_E("Error parsing glTF file: {}", magic_enum::enum_name(error));
			return false;
		}
	}

	return true;
}

void GlTFImporter::visitGlTFNode(fastgltf::Asset& asset,
								 fastgltf::Node& node,
								 shared_ptr<ae::Node> parent) {

	// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?

	auto aeNode = Node::NamedNode(string(node.name));

	aeNode->transform(TransformFromGlFTNode(node));

	if ((_options & SceneImportOptions::ImportGeometries) != SceneImportOptions::None) {
		aeNode->geometry(geometryFromGlFTNode(asset, node));
	}

	if ((_options & SceneImportOptions::ImportLights) != SceneImportOptions::None) {
		aeNode->light(lightFromGlTFNode(asset, node));
	}

	if ((_options & SceneImportOptions::ImportCameras) != SceneImportOptions::None) {
		aeNode->camera(cameraFromGlTFNode(asset, node));
	}

	parent->addChild(aeNode);

	for (auto c: node.children) {
		visitGlTFNode(asset, asset.nodes[c], aeNode);
	}
}

shared_ptr<Geometry> GlTFImporter::geometryFromGlFTNode(fastgltf::Asset& asset,
														fastgltf::Node& node) {
	if (auto meshIndex = node.meshIndex) {
		return geometryFromGlFTMeshIndex(asset, *meshIndex);
	}
	return nullptr;
}

shared_ptr<Geometry> GlTFImporter::geometryFromGlFTMeshIndex(fastgltf::Asset& asset,
															 size_t meshIndex) {

	if (auto existing = _geometries.find(meshIndex)
			; existing == _geometries.end()) {

		auto& mesh = asset.meshes[meshIndex];

		auto elements = vector<shared_ptr<GeometryElement>>();
		auto materials = vector<shared_ptr<Material>>();

		for (auto& primitive: mesh.primitives) {

			auto element = geometryElementFromGlFTPrimitive(asset, primitive);
			if (element) elements.push_back(element);

			// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?
			auto material = ((_options & SceneImportOptions::ImportMaterials) != SceneImportOptions::None)
							? materialFromGlFTPrimitive(asset, primitive)
							: Material::DefaultMaterial();
			if (material) materials.push_back(material);
		}

		auto geometry = make_shared<Geometry>(elements, materials);
		geometry->name(string(mesh.name));
		_geometries[meshIndex] = geometry;
		return geometry;
	}
	else {
		return _geometries[meshIndex];
	}

	return nullptr;
}

shared_ptr<ae::GeometryElement> GlTFImporter::geometryElementFromGlFTPrimitive(fastgltf::Asset& asset,
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
							AE_LOG_W("Unsupported position buffer data.");
						}
					}
					else {
						AE_LOG_W("Unsupported position accessor component type: {}",
								 magic_enum::enum_name(componentType));
					}
				}
				else {
					AE_LOG_W("Unsupported position accessor type: {}",
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
							AE_LOG_W("Unsupported normals buffer data.");
						}
					}
					else {
						AE_LOG_W("Unsupported normals accessor component type: {}",
								 magic_enum::enum_name(componentType));
					}
				}
				else {
					AE_LOG_W("Unsupported normals accessor type: {}",
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
							AE_LOG_W("Unsupported texcoords buffer data.");
						}
					}
					else {
						AE_LOG_W("Unsupported texcoords accessor component type: {}",
								 magic_enum::enum_name(componentType));
					}
				}
				else {
					AE_LOG_W("Unsupported texcoords accessor type: {}",
							 magic_enum::enum_name(type));
				}
			}

			return make_shared<GeometryElement>(verts, faces);
		}
		else {
			AE_LOG_E("Missing vertex indicies.");
		}
	}
	else {
		AE_LOG_W("Unsupported primitive type: {}",
				 magic_enum::enum_name(primitive.type));
	}

	return nullptr;
}

// OG
//shared_ptr<ae::Material> GlTFImporter::materialFromGlFTPrimitive(fastgltf::Asset& asset,
//																 fastgltf::Primitive& primitive) {
//
//	if (auto materialIndex = primitive.materialIndex) {
//
//		//if (auto existing = _materials.find(*materialIndex) ; existing == _materials.end()) {
//		if (_materials.find(*materialIndex) == _materials.end()) { // TODO: DO FOR ALL
//
//			auto& material = asset.materials[*materialIndex];
//
//			shared_ptr<ae::Material> aeMaterial = nullptr;
//
//			// ambient, diffuse
//
//			if (auto& pbrData = material.pbrData ; pbrData.baseColorTexture) {
//
//				auto baseColorTextureIndex = (*pbrData.baseColorTexture).textureIndex;
//				auto& texture = asset.textures[baseColorTextureIndex];
//
//				if (auto aeImage = imageFromGlTFTexture(asset, texture) ; aeImage) {
//					auto aeProperty = materialPropertyFromGlTFTexture(asset, texture);
//					aeProperty->contents(aeImage);
//
//					aeMaterial = make_shared<ae::Material>(nullptr, aeProperty, nullptr);
//				}
//				else {
//					aeMaterial = ae::Material::MissingTextureMaterial();
//				}
//			}
//			else {
//
//				// TODO: enabling this fucks up specular
//				// enabing specular below fixes the magenta light's specular reflection,
//				// but not the yellow light's specular reflection. (?)
//
//				auto baseColorFactor = pbrData.baseColorFactor;
//
//				auto aeColor = ColorFromGlTFColorArray(baseColorFactor);
//				auto property = make_shared<Material::Property>(aeColor);
//				aeMaterial = make_shared<ae::Material>(nullptr, property, nullptr);
//			}
//
//			// specular
//
//			if (auto& specularMaterial = material.specular; specularMaterial) {
//
//				// TODO: this needs work.  and more testing.
//
//				auto factor = specularMaterial->specularFactor;
//				auto& textureInfo = specularMaterial->specularTexture;
//				auto colorFactor = specularMaterial->specularColorFactor;
//				auto& colorTextureInfo = specularMaterial->specularColorTexture;
//
////				AE_LOG_D("*** [SPECULAR] ***");
////				AE_LOG_D("factor: {}", factor);
////				AE_LOG_D("textureInfo: {}", textureInfo ? "true" : "false");
////				AE_LOG_D("colorFactor: ({}, {}, {})", colorFactor[0], colorFactor[1], colorFactor[2]);
////				AE_LOG_D("colorTextureInfo: {}", colorTextureInfo ? "true" : "false");
//
//				// if it has a 'texture' map, use it (only contains alpha)
//				// if it has no map, but a 'factor' use solid white, with an intentify of the factor.
//
//				shared_ptr<ae::Material::Property> aeProperty = nullptr;
//
//				if (textureInfo) {
//					auto texture = asset.textures[(*textureInfo).textureIndex];
//
//					if (auto aeImage = imageFromGlTFTexture(asset, texture) ; aeImage) {
//						aeProperty = materialPropertyFromGlTFTexture(asset, texture);
//						aeProperty->contents(aeImage);
//					}
//				}
//
//				if (!aeProperty && (factor > 0)) {
//					auto factorColor = make_shared<Color>(factor);
//					aeProperty = make_shared<ae::Material::Property>(factorColor);
//				}
//
//				if (aeProperty) {
//					if (!aeMaterial) {
//						aeMaterial = make_shared<ae::Material>(nullptr, nullptr, aeProperty);
//					}
//					else {
//						aeMaterial->specular(aeProperty);
//					}
//				}
//			}
//
//			if (aeMaterial) {
//
//				aeMaterial->locksAmbientWithDiffuse(true);
//
//				aeMaterial->doubleSided(material.doubleSided);
//
//				if (auto& anisotropy = material.anisotropy; anisotropy) {
//					// error to draw attention -- at time of initial glTF integration
//					// we don't have an example file with KHR_materials_anisotropy
//					auto strength = anisotropy->anisotropyStrength;
//					AE_LOG_E("anisotropyStrength: {}", strength);
//					aeMaterial->diffuse()->maxAnisotropy(strength);
//				}
//
//				// specularExponent?
//
//				// transform -- scale
//
//				// emissiveFactor/emissiveTexture?
//
//				_materials[*materialIndex] = aeMaterial;
//				return aeMaterial;
//			}
//		}
//		else {
//			return _materials[*materialIndex];
//		}
//	}
//	else {
//		AE_LOG_W("Missing material.");
//	}
//
//	return ae::Material::DefaultMaterial();
//}

shared_ptr<ae::Material> GlTFImporter::materialFromGlFTPrimitive(fastgltf::Asset& asset,
																 fastgltf::Primitive& primitive) {

	if (auto materialIndex = primitive.materialIndex) {

		//if (auto existing = _materials.find(*materialIndex) ; existing == _materials.end()) {
		if (_materials.find(*materialIndex) == _materials.end()) { // TODO: DO FOR ALL

			auto& material = asset.materials[*materialIndex];

			shared_ptr<ae::Material> aeMaterial = nullptr;

			// ambient, diffuse

			if (auto& pbrData = material.pbrData ; pbrData.baseColorTexture) {

				auto baseColorTextureIndex = (*pbrData.baseColorTexture).textureIndex;
				if (auto aeTexture = textureFromGlTFTextureIndex(asset, baseColorTextureIndex)
						; aeTexture) {
					aeMaterial = make_shared<ae::Material>(monostate{}, aeTexture, monostate{});
				}
				else {
					aeMaterial = ae::Material::MissingTextureMaterial();
				}

//				auto baseColorTextureIndex = (*pbrData.baseColorTexture).textureIndex;
//				auto& texture = asset.textures[baseColorTextureIndex];
//
//				if (auto aeImage = imageFromGlTFTexture(asset, texture) ; aeImage) {
//					auto aeProperty = materialPropertyFromGlTFTexture(asset, texture);
//					aeProperty->contents(aeImage);
//
//					aeMaterial = make_shared<ae::Material>(nullptr, aeProperty, nullptr);
//				}
//				else {
//					aeMaterial = ae::Material::MissingTextureMaterial();
//				}
			}
			else {

				auto baseColorFactor = pbrData.baseColorFactor;

				auto aeColor = ColorFromGlTFColorArray(baseColorFactor);
				auto aeProperty = Material::Property(aeColor);
				aeMaterial = make_shared<ae::Material>(monostate{}, aeProperty, monostate{});

				// enabing specular below fixes the magenta light's specular reflection,
				// but not the yellow light's specular reflection. (?)

//				auto baseColorFactor = pbrData.baseColorFactor;
//
//				auto aeColor = ColorFromGlTFColorArray(baseColorFactor);
//				auto property = make_shared<Material::Property>(aeColor);
//				aeMaterial = make_shared<ae::Material>(nullptr, property, nullptr);
			}

			// specular

			if (auto& specularMaterial = material.specular; specularMaterial) {

				// TODO: this needs work.  and more testing.

				auto factor = specularMaterial->specularFactor;
				auto& textureInfo = specularMaterial->specularTexture;
				auto colorFactor = specularMaterial->specularColorFactor;
				auto& colorTextureInfo = specularMaterial->specularColorTexture;

//				AE_LOG_D("*** [SPECULAR] ***");
//				AE_LOG_D("factor: {}", factor);
//				AE_LOG_D("textureInfo: {}", textureInfo ? "true" : "false");
//				AE_LOG_D("colorFactor: ({}, {}, {})", colorFactor[0], colorFactor[1], colorFactor[2]);
//				AE_LOG_D("colorTextureInfo: {}", colorTextureInfo ? "true" : "false");

				// if it has a 'texture' map, use it (only contains alpha)
				// if it has no map, but a 'factor' use solid white, with an intentify of the factor.

				ae::Material::Property aeProperty = monostate{};

				if (textureInfo) {

					auto specularTextureIndex = (*textureInfo).textureIndex;
					if (auto aeTexture = textureFromGlTFTextureIndex(asset, specularTextureIndex)
							; aeTexture) {
						aeProperty = aeTexture;
					}
					else {
						aeProperty = ae::Material::MissingTextureProperty();
					}

//					auto texture = asset.textures[(*textureInfo).textureIndex];
//
//					if (auto aeImage = imageFromGlTFTexture(asset, texture) ; aeImage) {
//						aeProperty = materialPropertyFromGlTFTexture(asset, texture);
//						aeProperty->contents(aeImage);
//					}
				}

				if (holds_alternative<monostate>(aeProperty) && (factor > 0)) {
					auto factorColor = make_shared<Color>(factor);
					aeProperty = ae::Material::Property(factorColor);
				}

				if (!holds_alternative<monostate>(aeProperty)) {
					if (!aeMaterial) {
						aeMaterial = make_shared<ae::Material>(monostate{}, monostate{}, aeProperty);
					}
					else {
						aeMaterial->specular(aeProperty);
					}
				}
			}

			if (aeMaterial) {

				aeMaterial->locksAmbientWithDiffuse(true);

				aeMaterial->doubleSided(material.doubleSided);

				if (auto& anisotropy = material.anisotropy; anisotropy) {

					// log error to draw attention -- at time of initial glTF integration
					// we don't have an example file with KHR_materials_anisotropy
					auto strength = anisotropy->anisotropyStrength;
					AE_LOG_E("anisotropyStrength: {}", strength);
//					aeMaterial->maxAnisotropy(strength);

					// heh
					Material::Property properties[] = { aeMaterial->ambient(),
														aeMaterial->diffuse(),
														aeMaterial->specular(),
														aeMaterial->emission() };
					for (int p=0; p<4; ++p) {
						auto property = properties[p];
						if (holds_alternative<shared_ptr<Texture>>(property)) {
							auto texture = get<shared_ptr<Texture>>(property);
							texture->sampler()->maxAnisotropy(strength);
						}
					}
				}

				// specularExponent?

				// transform -- scale

				// emissiveFactor/emissiveTexture?

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

			auto& image = asset.images[*imageIndex];

			shared_ptr<Image> aeImage = nullptr;

			if (auto &dataSource = image.data; holds_alternative<sources::Vector>(dataSource)) { // .gltf
				AE_LOG_D("Creating texture image...");

				auto uint8Vec = get<sources::Vector>(dataSource).bytes;
				auto aeBuffer = make_shared<ae::Buffer>(uint8Vec.data(), uint8Vec.size());
				aeImage = make_shared<ae::Image>(aeBuffer, false);
			}
			else if (holds_alternative<sources::BufferView>(dataSource)) { // .glb

				auto bufferViewIndex = get<sources::BufferView>(dataSource).bufferViewIndex;
				auto& bufferView = asset.bufferViews[bufferViewIndex];

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

//shared_ptr<Material::Property> GlTFImporter::materialPropertyFromGlTFTexture(fastgltf::Asset& asset,
//																			 fastgltf::Texture& texture) {
//
//	// ! important !
//	// don't map these to ae::MaterialProperty.
//	// ae uses a 1:1 Image/Color:MaterialProperty relationship whereas
//	// glTF uses a 1:N Texture:Sampler relationship.
//
//	auto aeProperty = make_shared<ae::Material::Property>();
//
//	if (auto samplerIndex = texture.samplerIndex) {
//		auto sampler = asset.samplers[*samplerIndex];
//
//		if (sampler.minFilter) {
//			aeProperty->minificationFilter(FilterMode(*sampler.minFilter));
//		}
//		if (sampler.magFilter) {
//			aeProperty->magnificationFilter(FilterMode(*sampler.magFilter));
//		}
//		aeProperty->wrapS(WrapMode(sampler.wrapS));
//		aeProperty->wrapT(WrapMode(sampler.wrapT));
//	}
//
//	return aeProperty;
//};

shared_ptr<ae::Light> GlTFImporter::lightFromGlTFNode(fastgltf::Asset& asset,
													  fastgltf::Node& node) {

	if (auto lightIndex = node.lightIndex) {

		if (auto existing = _lights.find(*lightIndex)
				; existing == _lights.end()) {

			auto& light = asset.lights[*lightIndex];
			auto& type = light.type;

			if (type == fastgltf::LightType::Point) {

				auto aeLight = make_shared<Light>(LightType::Point);

				aeLight->name(string(light.name));
				aeLight->attenuationFactor(0); // temporary
				aeLight->color(ColorFromGlTFColorArray(light.color));
				// TODO: range, intensity

				_lights[*lightIndex] = aeLight;
				return aeLight;
			}
			else {

				AE_LOG_W("Unsupported light type: {}",
						 magic_enum::enum_name(type));
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
															   (perspective.zfar
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









shared_ptr<ae::Texture> GlTFImporter::textureFromGlTFTextureIndex(fastgltf::Asset& asset,
																  std::size_t textureIndex) {

//	if (auto existing = _textures.find(textureIndex)
//			; existing == _textures.end()) {
//
//		auto& mesh = asset.meshes[meshIndex];
//
//		auto elements = vector<shared_ptr<GeometryElement>>();
//		auto materials = vector<shared_ptr<Material>>();
//
//		for (auto& primitive: mesh.primitives) {
//
//			auto element = geometryElementFromGlFTPrimitive(asset, primitive);
//			if (element) elements.push_back(element);
//
//			// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?
//			auto material = ((_options & SceneImportOptions::ImportMaterials) != SceneImportOptions::None)
//							? materialFromGlFTPrimitive(asset, primitive)
//							: Material::DefaultMaterial();
//			if (material) materials.push_back(material);
//		}
//
//		auto geometry = make_shared<Geometry>(elements, materials);
//		geometry->name(string(mesh.name));
//		_geometries[meshIndex] = geometry;
//		return geometry;
//	}
//	else {
//		return _geometries[meshIndex];
//	}
//
//	return nullptr;



//	if (auto cameraIndex = sampler.) {

		if (auto existing = _textures.find(textureIndex)
				; existing == _textures.end()) {

			auto& texture = asset.textures[textureIndex];




//			auto baseColorTextureIndex = (*pbrData.baseColorTexture).textureIndex;
//			auto& texture = asset.textures[baseColorTextureIndex];

			if (auto aeImage = imageFromGlTFTexture(asset, texture) ; aeImage) {
				auto aeSampler = samplerFromGlTFTexture(asset, texture);
//				aeProperty->contents(aeImage);

//				aeMaterial = make_shared<ae::Material>(nullptr, aeProperty, nullptr);

				auto aeTexture = make_shared<ae::Texture>(make_shared<ae::Sampler>(), aeImage);
				_textures[textureIndex] = aeTexture;
				return aeTexture;
			}
//			else {
////				aeMaterial = ae::Material::MissingTextureMaterial();
//			}
		}
		else {
			return _textures[textureIndex];
		}
//	}

	return nullptr;
}

shared_ptr<ae::Sampler> GlTFImporter::samplerFromGlTFTexture(fastgltf::Asset& asset,
															 fastgltf::Texture& texture) {

	if (auto samplerIndex = texture.samplerIndex) {

		if (auto existing = _samplers.find(*samplerIndex)
				; existing == _samplers.end()) {

			auto& sampler = asset.samplers[*samplerIndex];

			auto aeSampler = make_shared<ae::Sampler>();

			if (sampler.minFilter) {
				aeSampler->minificationFilter(FilterMode(*sampler.minFilter));
			}
			if (sampler.magFilter) {
				aeSampler->magnificationFilter(FilterMode(*sampler.magFilter));
			}
			aeSampler->wrapS(WrapMode(sampler.wrapS));
			aeSampler->wrapT(WrapMode(sampler.wrapT));

			return aeSampler;
		}
		else {
			return _samplers[*samplerIndex];
		}
	}

	return nullptr;
}

fastgltf::Options GlTFOptionsFromImportOptions(SceneImportOptions options) {

	auto gltfOptions = Options::None;

	// TODO: macro instead of != SCENE_IMPORT_OPTIONS::NONE ?

	if ((options & SceneImportOptions::ImportGeometries) != SceneImportOptions::None) {
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

shared_ptr<ae::Color> ColorFromGlTFColorArray(array<float, 3>& arr) {

	return make_shared<Color>(arr[0], arr[1], arr[2]);
}

shared_ptr<ae::Color> ColorFromGlTFColorArray(array<float, 4>& arr) {

	array<float, 3> rgbArray = {arr[0], arr[1], arr[2]};
	return ColorFromGlTFColorArray(rgbArray);
}
