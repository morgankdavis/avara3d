//
//  Geometry.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "ae/geometry/Geometry.h"

#include "glm/gtx/transform.hpp"
#include "rapidobj.hpp"

#include "ae/Color.h"
#include "ae/Image.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/diagnostic/exceptions/UnsupportedFormat.h"
#include "ae/geometry/GeometryElement.h"
#include "ae/rendering/Renderer.h"
#include "ae/rendering/materials/Material.h"
#include "ae/rendering/materials/MaterialProperty.h"
#include "ae/scene/Node.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Private Static Prototypes
 *********************************************************************************************/

static shared_ptr<Geometry> LoadObj(const filesystem::path& path);
static shared_ptr<Material> AEMaterialFromROMaterials(map<int32_t, shared_ptr<ae::Material>>& aeMaterialsMap,
													  const rapidobj::Materials& roMaterials,
													  int32_t roMaterialID,
													  filesystem::path& textureDir);

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<Geometry> Geometry::FromFile(const filesystem::path& path) {

	auto extension = path.extension();
	if (extension == ".obj") {
		return LoadObj(path);;
	}
	else {
		throw UnsupportedFormat(fmt::format("Unsupported geometry format: {}", extension.string()));
	}

	return nullptr;
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Geometry::Geometry():
		_name(nullopt),
		_elements(vector<shared_ptr<GeometryElement>>()),
		_materials(vector<shared_ptr<Material>>()),
//	_node({}),
		_dirtyMask(GEOMETRY_DIRTY_MASK::ALL) {

}

Geometry::Geometry(const shared_ptr<GeometryElement> element,
				   const shared_ptr<Material> material):
		Geometry() {

	_elements.emplace_back(element);
	_materials.emplace_back(material);
}

Geometry::Geometry(const vector<shared_ptr<GeometryElement>> elements,
				   const vector<shared_ptr<Material>> materials):
		Geometry() {

	_elements.insert(_elements.begin(), elements.begin(), elements.end());
	_materials.insert(_materials.begin(), materials.begin(), materials.end());
}

Geometry::~Geometry() {
	AE_LOG_D("Destroying Geometry {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

optional<string> Geometry::name() const {
	return _name;
}

void Geometry::name(const string& name) {
	_name = name;
}

const vector<shared_ptr<GeometryElement>>& Geometry::elements() {
	return _elements;
}

const vector<shared_ptr<Material>>& Geometry::materials() {
	return _materials;
}

shared_ptr<Material> Geometry::firstMaterial() const {
	if (!_materials.empty()) {
		return _materials[0];
	}
	return nullptr;
}

shared_ptr<Material> Geometry::materialNamed(const string& name) const {
	for (auto material : _materials) {
		auto matName = material->name();
		if (matName) {
			if (!((*matName) == name)) {
				return material;
			}
		}
	}
	return nullptr;
}

void Geometry::addMaterial(const shared_ptr<Material> material) {
	_materials.emplace_back(material);
}

void Geometry::insertMaterial(const shared_ptr<Material> material, int index) {
	_materials.insert(_materials.begin()+index, material);
}

void Geometry::removeMaterial(int index) {
	if (_materials.size() >= index-1) {
		_materials.erase(_materials.begin()+index);
	}
}

void Geometry::replaceMaterial(int index, const shared_ptr<Material> replacement) {
	removeMaterial(index);
	insertMaterial(replacement, index);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void Geometry::burnTransform(const mat4& transform, bool normals) {
	for (auto element : elements()) {
		element->burnTransform(transform, normals);
	}
	//node()->lock()->transform(mat4(1.0));
}

void Geometry::draw(Renderer& renderer,
					const mat4& modelMat,
					const mat4& viewMat,
					const mat4& projectionMat,
					const DEBUG_OPTIONS& debugOptions,
					Stats& stats) {
	
	// forces Renderer to re-create AABB linesets next time they're turned on.
	// this seems like hacky way to do it.
	if (!DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
		_dirtyMask = GEOMETRY_DIRTY_MASK_ADD(_dirtyMask, GEOMETRY_DIRTY_MASK::EXTENT);
	}
	
	renderer.render(shared_from_this(),
					modelMat, viewMat, projectionMat,
					debugOptions, stats);
	
	for (unsigned e=0; e<_elements.size(); ++e) {
		
		shared_ptr<GeometryElement> element = _elements[e];
		shared_ptr<Material> material = nullptr;
		if (_materials.size() > e) {
			material = _materials[e];
		}
		else {
			material = Material::DefaultMaterial();
		}
		
		element->draw(renderer,
					  *material,
					  modelMat,
					  viewMat,
					  projectionMat,
					  debugOptions,
					  stats);

		++stats.geometries;
	}	
}

AABB Geometry::aabb(const shared_ptr<Node> convertToNode) const {

	static const float maxFloat = numeric_limits<float>::max();
	static const float minFloat = numeric_limits<float>::min();

	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	for (const auto& element : _elements) {
		auto elementAABB = element->aabb(convertToNode);
			aabb.min.x = std::min(aabb.min.x, elementAABB.min.x);
			aabb.max.x = std::max(aabb.max.x, elementAABB.max.x);
			aabb.min.y = std::min(aabb.min.y, elementAABB.min.y);
			aabb.max.y = std::max(aabb.max.y, elementAABB.max.y);
			aabb.min.z = std::min(aabb.min.z, elementAABB.min.z);
			aabb.max.z = std::max(aabb.max.z, elementAABB.max.z);
	}

	return aabb;
}

vec3 Geometry::extent(const shared_ptr<Node> convertToNode) const {
	auto aabb = Geometry::aabb(convertToNode);
	return {aabb.max.x - aabb.min.x,
			aabb.max.y - aabb.min.y,
			aabb.max.z - aabb.min.z};
}

//void Geometry::attachedToNode(shared_ptr<Node> node) {
//	_node = node;
//}
//
//weak_ptr<Node> Geometry::node() const {
//	return _node;
//}

GEOMETRY_DIRTY_MASK Geometry::dirtyMask() const {
	return _dirtyMask;
}

void Geometry::dirtyMask(GEOMETRY_DIRTY_MASK mask) {
	_dirtyMask = mask;
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

shared_ptr<Geometry> LoadObj(const filesystem::path& path) {

	using namespace rapidobj;

	AE_LOG_I("Loading Wavefront Obj at: {}", path.string());

	// example from tinyobjloader:
	// https://github.com/tinyobjloader/tinyobjloader#example-code-deprecated-api

	auto result = ParseFile(path.string(),
							MaterialLibrary::Default(Load::Optional));
//	auto result = ParseFile(path.string());

	if (!result.error) {

		Triangulate(result);

		auto& attributes = result.attributes;
		auto& positions = attributes.positions;
		auto& normals = attributes.normals;
		auto& texCoords = attributes.texcoords;
		auto& shapes = result.shapes;
		auto& materials = result.materials;

		auto aeElements = vector<shared_ptr<GeometryElement>>();
		auto aeMaterials = vector<shared_ptr<ae::Material>>();

		auto aeMaterialsMap = map<int32_t, shared_ptr<ae::Material>>();
		auto textureDir = path.parent_path();

		auto numShapes = shapes.size();
		AE_LOG_D("numShapes: {}", numShapes);
		for (auto s=0; s<numShapes; ++s) {

			auto& shape = result.shapes[s];
			auto& mesh = shape.mesh;

			auto verts = vector<Vertex>();
			auto faces = vector<Face>();

			size_t indexOffset = 0;
			int32_t materialID = -1;
			int32_t prevMaterialID = -1;
			size_t numFaces = shape.mesh.num_face_vertices.size();
			size_t faceIndexOffset = 0;
			AE_LOG_D("shape[{}], numFaces: {}", s, numFaces);
			for (auto f=0; f<numFaces; ++f) {

				auto baseFaceIndex = (size_t)((3 * f) - faceIndexOffset);

				materialID = shape.mesh.material_ids[f];
//				AE_LOG_D("materialID: {}", materialID);

				if (prevMaterialID == -1) {
//					AE_LOG_D("Initializing materialID to {}.", materialID);
					prevMaterialID = materialID;
				}

				// whenever the material ID changes, start a new GeometryElement
				if (materialID != prevMaterialID) {
					AE_LOG_D("New materialID -- adding GeometryElement.");

					aeElements.push_back(make_shared<GeometryElement>(verts, faces));
					auto aeMaterial = AEMaterialFromROMaterials(aeMaterialsMap,
																materials,
																prevMaterialID,
																textureDir);
					if (aeMaterial) aeMaterials.push_back(aeMaterial);

					verts.clear();
					faces.clear();

					// since we are logging off an element early, we have to keep
					// a negative offset for future face indicies.
					faceIndexOffset += baseFaceIndex;
					baseFaceIndex -= faceIndexOffset;
				}

				//AE_LOG_D("Adding vertex for face {}, material {}", f, materialID);
				// vertices in the face (shape is triangulated above)
				for (auto v=0; v<3; ++v) {

					// access to vertex
					auto index = shape.mesh.indices[indexOffset + v];

					auto vx = positions[3 * index.position_index + 0];
					auto vy = positions[3 * index.position_index + 1];
					auto vz = positions[3 * index.position_index + 2];
					Vertex vert = {{vx, vy, vz}, {}, {}};

					// negative = no normals
					if (index.normal_index >= 0) {
						auto nx = normals[3 * index.normal_index + 0];
						auto ny = normals[3 * index.normal_index + 1];
						auto nz = normals[3 * index.normal_index + 2];
						vert.normal = {nx, ny, nz};
					}

					// negative = no texcoords
					if (index.texcoord_index >= 0) {
						auto tu = texCoords[2 * index.texcoord_index + 0];
						auto tv = texCoords[2 * index.texcoord_index + 1];
						vert.texCoord = {tu, tv};
					}

					verts.push_back(vert);

				} // verticies

				//AE_LOG_D("Adding face for face {}, material {}", f, materialID);
				Face face = {(int)baseFaceIndex + 0,
							 (int)baseFaceIndex + 1,
							 (int)baseFaceIndex + 2};
				faces.push_back(face);

				prevMaterialID = materialID;
				indexOffset += 3;

			} // faces

			// add the last element (materialID didn't change at the end of the face list)
			AE_LOG_D("Adding last GeometryElement.  verts: {}, faces: {}",
					 verts.size(), faces.size());
			aeElements.push_back(make_shared<GeometryElement>(verts, faces));
			auto aeMaterial = AEMaterialFromROMaterials(aeMaterialsMap,
														materials,
														materialID,
														textureDir);
			if (aeMaterial) aeMaterials.push_back(aeMaterial);

		} // shapes

		AE_LOG_D("Adding geometry with {} elements, {} materials.",
				 aeElements.size(), aeMaterials.size());
		auto geometry = make_shared<Geometry>(aeElements, aeMaterials);
		geometry->name(path.filename().stem().string());
		return geometry;
	}
	else {
		AE_LOG_E("Error at line {}: {}",
				 result.error.line_num, result.error.code.message());
	}

	return nullptr;
}

shared_ptr<Material> AEMaterialFromROMaterials(map<int32_t, shared_ptr<ae::Material>>& aeMaterialsMap,
											   const rapidobj::Materials& roMaterials,
											   int32_t roMaterialID,
											   filesystem::path& textureDir) {
	AE_LOG_D("roMaterialID: {}", roMaterialID);

	if (roMaterialID == -1) {
		return nullptr;
	}
	else if (roMaterials.size() == 0) {
		AE_LOG_W("Missing materials.");
		return nullptr;
	}
	else if (auto existing = aeMaterialsMap.find(roMaterialID); existing != aeMaterialsMap.end()) {
		return existing->second;
	}
	else {

		auto aeMaterial = make_shared<Material>();

		auto& roMaterial = roMaterials[roMaterialID];

		// TODO: generalize this

		{ // ambient

			auto filename = roMaterial.ambient_texname;
			if (!filename.empty()) {
				auto image = make_shared<Image>(textureDir / filename);
				auto property = make_shared<MaterialProperty>(image);
				aeMaterial->ambient(property);
			}
			else {
				auto roColor = roMaterial.ambient;
				auto aeColor = make_shared<Color>(roColor[0], roColor[1], roColor[2]);
				auto property = make_shared<MaterialProperty>(aeColor);
				aeMaterial->ambient(property);
			}
		}

		{ // diffuse

			auto filename = roMaterial.diffuse_texname;
			if (!filename.empty()) {
				auto image = make_shared<Image>(textureDir / filename);
				auto property = make_shared<MaterialProperty>(image);
				aeMaterial->diffuse(property);
			}
			else {
				auto roColor = roMaterial.diffuse;
				auto aeColor = make_shared<Color>(roColor[0], roColor[1], roColor[2]);
				auto property = make_shared<MaterialProperty>(aeColor);
				aeMaterial->diffuse(property);
			}
		}

		{ // specular

			auto filename = roMaterial.specular_texname;
			if (!filename.empty()) {
				auto image = make_shared<Image>(textureDir / filename);
				auto property = make_shared<MaterialProperty>(image);
				aeMaterial->specular(property);
			}
			else {
				auto roColor = roMaterial.specular;
				auto aeColor = make_shared<Color>(roColor[0], roColor[1], roColor[2]);
				auto property = make_shared<MaterialProperty>(aeColor);
				aeMaterial->specular(property);
			}
		}

		{ // emissive

		}

		{ // specular exponant

		}

		{ // double-sided

		}

		{ // fill mode

		}

		{ // scale

		}

		{ // blend function

		}

		aeMaterialsMap[roMaterialID] = aeMaterial;

		return aeMaterial;
	}
}
