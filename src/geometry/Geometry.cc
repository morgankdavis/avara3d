//
//  Geometry.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "ae/geometry/Geometry.h"

#include "glm/gtx/transform.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "tiny_obj_loader.h"

#include "ae/diagnostic/logging/Logger.h"
#include "ae/diagnostic/exceptions/UnsupportedFormat.h"
#include "ae/geometry/GeometryElement.h"
#include "ae/rendering/Renderer.h"
#include "ae/rendering/materials/Material.h"
#include "ae/scene/Node.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Private Static Prototypes
 *********************************************************************************************/

static shared_ptr<Geometry> LoadObj(const filesystem::path& path);

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

	using namespace tinyobj;

	attrib_t objAttrib;
	vector<shape_t> objShapes;
	vector<material_t> objMaterials;
	string err;

	if (LoadObj(&objAttrib, &objShapes, &objMaterials, &err, path.c_str())) {

		if (!err.empty()) {
			AE_LOG_W(err);
		}

		auto elements = vector<shared_ptr<GeometryElement>>();
		auto materials = vector<shared_ptr<Material>>();

		// loop over shapes
		for (auto s=0; s<objShapes.size(); ++s) {

			auto shape = objShapes[s];

			auto verts = vector<Vertex>();
			auto faces = vector<Face>();

			// loop over faces (polygon)
			size_t index_offset = 0;
			for (auto f=0; f<shape.mesh.num_face_vertices.size(); ++f) {

				auto fv = shape.mesh.num_face_vertices[f];

				// loop over vertices in the face.
				for (auto v=0; v<fv; ++v) {

					Vertex vert;

					// access to vertex
					auto idx = shape.mesh.indices[index_offset + v];
					Face face = {static_cast<int>(3 * idx.vertex_index + 0),
								 static_cast<int>(3 * idx.vertex_index + 1),
								 static_cast<int>(3 * idx.vertex_index + 2)};
					faces.push_back(face);

					auto vx = objAttrib.vertices[face.a];
					auto vy = objAttrib.vertices[face.b];
					auto vz = objAttrib.vertices[face.c];
//					auto vx = objAttrib.vertices[3 * idx.vertex_index + 0];
//					auto vy = objAttrib.vertices[3 * idx.vertex_index + 1];
//					auto vz = objAttrib.vertices[3 * idx.vertex_index + 2];
					vert.position = {vx, vy, vz};

					// negative = no normal data
					if (idx.normal_index >= 0) {
						auto nx = objAttrib.normals[3 * idx.normal_index + 0];
						auto ny = objAttrib.normals[3 * idx.normal_index + 1];
						auto nz = objAttrib.normals[3 * idx.normal_index + 2];
						vert.normal = {nx, ny, nz};
					}

					// negative = no texcoord data
					if (idx.texcoord_index >= 0) {
						auto tx = objAttrib.texcoords[2 * idx.texcoord_index + 0];
						auto ty = objAttrib.texcoords[2 * idx.texcoord_index + 1];
						vert.textureCoordinate = {tx, ty};
					}

					verts.push_back(vert);

					// optional: vertex colors
					// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
				}
				index_offset += fv;

				// per-face material
				auto objMaterial = objShapes[s].mesh.material_ids[f];
			}

//			auto numFaces = shape.mesh.indices.size();
//
//			for (size_t f = 0; f < numFaces; ++f) {
//				auto index = shape.mesh.indices[f];
//				faces.push_back(Face{index.vertex_index});
//			}


			elements.push_back(make_shared<GeometryElement>(verts, faces));
		}

		auto geometry = make_shared<Geometry>(elements, materials);
		geometry->name(path.filename().stem());
		return geometry;
	}
	else {
		AE_LOG_E("Error loading obj file: {}", err);
	}

	return nullptr;
}
