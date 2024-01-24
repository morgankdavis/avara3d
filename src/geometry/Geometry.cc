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

//shared_ptr<Geometry> LoadObj(const filesystem::path& path) {
//
//	using namespace rapidobj;
//
//	auto result = ParseFile(path.string(),
//			//MaterialLibrary::SearchPath(path.parent_path()));
//							MaterialLibrary::Default());
//	if (!result.error) {
//
//		Triangulate(result);
//
//		for (auto& mat : result.materials) {
//			AE_LOG_I("mat: {}, diffuse_texname: {}", mat.name, mat.diffuse_texname);
//		}
//
//		auto elements = vector<shared_ptr<GeometryElement>>();
//		auto materials = vector<shared_ptr<ae::Material>>();
//
//		auto numShapes = result.shapes.size();
//		for (size_t s=0; s<numShapes; ++s) {
//
////		auto numShapes = result.shapes.size();
////		if (numShapes >= 1) {
////
////			if (numShapes > 1) {
////				AE_LOG_W("Obj file contains more than one shape.  Discarding shapes at index > 0.");
////			}
//
//			auto& shape = result.shapes[s];
//
//			auto name = shape.name;
//			AE_LOG_I("shape name: {}", name);
//			auto& mesh = shape.mesh;
//
//			auto& attributes = result.attributes;
//
//			auto& positions = attributes.positions;
//			auto& normals = attributes.normals;
//			auto& texCoords = attributes.texcoords;
//
//			AE_LOG_I("positions.size(): {}", positions.size());
//			AE_LOG_I("positions.size()/3: {}", positions.size()/3);
//			AE_LOG_I("normals.size(): {}", normals.size());
//			AE_LOG_I("normals.size()/3: {}", normals.size()/3);
//			AE_LOG_I("texCoords.size(): {}", texCoords.size());
//			AE_LOG_I("texCoords.size()/2: {}", texCoords.size()/2);
//
//			auto verts = vector<Vertex>();
//			auto faces = vector<Face>();
//
//			auto numPositions = positions.size();
//			for (size_t p=0; p<numPositions; p+=3) {
////				Vertex vert = {{positions[p+0], positions[p+1], positions[p+2]},
////							   {normals[p+0], normals[p+1], normals[p+2]},
////							   {texCoords[p+0], texCoords[p+1]}};
////				verts.push_back(vert);
//			}
//
//			auto& indicies = mesh.indices;
//
//			auto numIndicies = indicies.size();
////			size_t idx = 0;
//			for (size_t i=0; i<numIndicies; i+=3) {
//
//				auto p1 = positions[indicies[i+0].position_index];
//				auto p2 = positions[indicies[i+1].position_index];
//				auto p3 = positions[indicies[i+2].position_index];
//
//				Vertex vert = {{p1, p2, p3},
//							   {0, 0, 0},
//							   {0, 0}};
//				verts.push_back(vert);
//
////				Vertex vert = {{indicies[i+0].positions[p+0], positions[p+1], positions[p+2]},
////							   {normals[p+0], normals[p+1], normals[p+2]},
////							   {texCoords[p+0], texCoords[p+1]}};
////				verts.push_back(vert);
//
//
//
////				Face face = {indicies[i+0].position_index,
////							 indicies[i+1].position_index,
////							 indicies[i+2].position_index};
////				faces.push_back(face);
//
//				Face face = {(int)i+0,
//							 (int)i+1,
//							 (int)i+2};
//				faces.push_back(face);
//
//
//			}
//
//			AE_LOG_I("mesh.material_ids.size(): {}", mesh.material_ids.size());
//			AE_LOG_I("mesh.indices.size(): {}", mesh.indices.size());
//			AE_LOG_I("mesh.indices.size()/3: {}", mesh.indices.size()/3);
//
//			for (auto& matID : mesh.material_ids) {
//				AE_LOG_I("Material ID: {}", matID);
//			}
//
//			elements.push_back(make_shared<GeometryElement>(verts, faces));
//
//
//
//			AE_LOG_W("RETURNING FIRST SHAPE");
//
//			auto geometry = make_shared<Geometry>(elements, materials);
//			geometry->name(path.filename().stem().string());
//			return geometry;
//		}
//	}
//	else {
//		AE_LOG_E("Error at line {}: {}", result.error.line, result.error.code.message());
//	}
//
//	return nullptr;
//}

shared_ptr<Geometry> LoadObj(const filesystem::path& path) {

	using namespace rapidobj;

	auto result = ParseFile(path.string());
	// TODO: maybe allow missing textures?

	if (!result.error) {

		Triangulate(result);

		auto elements = vector<shared_ptr<GeometryElement>>();
		auto materials = vector<shared_ptr<ae::Material>>();

		auto& attributes = result.attributes;
		auto& positions = attributes.positions;
		auto& normals = attributes.normals;
		auto& texCoords = attributes.texcoords;
		auto& shapes = result.shapes;

		for (auto s=0; s<shapes.size(); ++s) {

			auto verts = vector<Vertex>();
			auto faces = vector<Face>();

			auto& shape = result.shapes[s];
			auto& mesh = shape.mesh;

			size_t index_offset = 0;
			for (auto f=0; f<shape.mesh.num_face_vertices.size(); ++f) {

				auto fv = shape.mesh.num_face_vertices[f];

				// loop over vertices in the face.
				for (auto v=0; v<fv; ++v) {

					// access to vertex
					auto idx = shape.mesh.indices[index_offset + v];

					auto vx = positions[3 * idx.position_index + 0];
					auto vy = positions[3 * idx.position_index + 1];
					auto vz = positions[3 * idx.position_index + 2];

					Vertex vert = {{vx, vy, vz}, {}, {}};

					// negative = no normal data
					if (idx.normal_index >= 0) {
						auto nx = normals[3 * idx.normal_index + 0];
						auto ny = normals[3 * idx.normal_index + 1];
						auto nz = normals[3 * idx.normal_index + 2];

						vert.normal = {nx, ny, nz};
					}

					// negative = no texcoord data
					if (idx.texcoord_index >= 0) {
						auto tu = texCoords[2 * idx.texcoord_index + 0];
						auto tv = texCoords[2 * idx.texcoord_index + 1];

						vert.texCoord = {tu, tv};
					}

					verts.push_back(vert);

					// optional: vertex colors
					// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
				}
				index_offset += fv;

				Face face = {3*f+0, 3*f+1, 3*f+2};
				faces.push_back(face);

				// per-face material
//				auto objMaterial = objShapes[s].mesh.material_ids[f];
			}


			elements.push_back(make_shared<GeometryElement>(verts, faces));

			AE_LOG_W("RETURNING FIRST SHAPE");

			auto geometry = make_shared<Geometry>(elements, materials);
			geometry->name(path.filename().stem().string());
			return geometry;
		}





//		auto numShapes = result.shapes.size();
//		for (size_t s=0; s<numShapes; ++s) {
//
//			auto& shape = result.shapes[s];
//
//			auto verts = vector<Vertex>();
//			auto faces = vector<Face>();
//
//			auto name = shape.name;
//			AE_LOG_I("name: {}", name);
//
//			auto& mesh = shape.mesh;
//
//			auto& indicies = mesh.indices;
//			auto numIndicies = indicies.size();
//			auto numFaces = mesh.num_face_vertices.size();
//			AE_LOG_I("numIndicies: {}", numIndicies);
//			AE_LOG_I("numIndicies/3: {}", numIndicies/3);
//			AE_LOG_I("numFaces: {}", numFaces);
//
//			for (size_t i=0; i<numIndicies; i+=3) {
//
//				auto px = positions[indicies[i+0].position_index];
//				auto py = positions[indicies[i+1].position_index];
//				auto pz = positions[indicies[i+2].position_index];
//
//				// TODO: check normal_index != -1
//				auto nx = normals[indicies[i+0].normal_index];
//				auto ny = normals[indicies[i+1].normal_index];
//				auto nz = normals[indicies[i+2].normal_index];
//
//				// TODO: check texcoord_index != -1
//				auto tu = texCoords[indicies[i+0].texcoord_index];
//				auto tv = texCoords[indicies[i+1].texcoord_index];
//
//				Vertex vertex = {{px, py, pz},
//								 {nx, ny, nz},
//								 {tu, tv}};
//				verts.push_back(vertex);
//
////				Face face = {(int)i+0,
////							 (int)i+1,
////							 (int)i+2};
////				faces.push_back(face);
//			}
//
//			elements.push_back(make_shared<GeometryElement>(verts, faces));
//
//
//
//			AE_LOG_W("RETURNING FIRST SHAPE");
//
//			auto geometry = make_shared<Geometry>(elements, materials);
//			geometry->name(path.filename().stem().string());
//			return geometry;
//		}
	}
	else {
		AE_LOG_E("Error at line {}: {}", result.error.line, result.error.code.message());
	}

	return nullptr;
}

//
// close -- diagnostic
//
//shared_ptr<Geometry> LoadObj(const filesystem::path& path) {
//
//	using namespace rapidobj;
//
//	auto result = ParseFile(path.string(),
//							//MaterialLibrary::SearchPath(path.parent_path()));
//							MaterialLibrary::Default());
//	if (!result.error) {
//
//		Triangulate(result);
//
//		for (auto& mat : result.materials) {
//			AE_LOG_I("mat: {}, diffuse_texname: {}", mat.name, mat.diffuse_texname);
//		}
//
//		auto elements = vector<shared_ptr<GeometryElement>>();
//		auto materials = vector<shared_ptr<ae::Material>>();
//
//		auto numShapes = result.shapes.size();
//		for (size_t s=0; s<numShapes; ++s) {
//
////		auto numShapes = result.shapes.size();
////		if (numShapes >= 1) {
////
////			if (numShapes > 1) {
////				AE_LOG_W("Obj file contains more than one shape.  Discarding shapes at index > 0.");
////			}
//
//			auto& shape = result.shapes[s];
//
//			auto name = shape.name;
//			AE_LOG_I("shape name: {}", name);
//			auto& mesh = shape.mesh;
//
//			auto& attributes = result.attributes;
//
//			auto& positions = attributes.positions;
//			auto& normals = attributes.normals;
//			auto& texCoords = attributes.texcoords;
//
//			AE_LOG_I("positions.size(): {}", positions.size());
//			AE_LOG_I("positions.size()/3: {}", positions.size()/3);
//			AE_LOG_I("normals.size(): {}", normals.size());
//			AE_LOG_I("normals.size()/3: {}", normals.size()/3);
//			AE_LOG_I("texCoords.size(): {}", texCoords.size());
//			AE_LOG_I("texCoords.size()/2: {}", texCoords.size()/2);
//
//			auto verts = vector<Vertex>();
//			auto faces = vector<Face>();
//
//			auto numPositions = positions.size();
//			for (size_t p=0; p<numPositions; p+=3) {
//				Vertex vert = {{positions[p+0], positions[p+1], positions[p+2]},
//							   {normals[p+0], normals[p+1], normals[p+2]},
//							   {texCoords[p+0], texCoords[p+1]}};
//				verts.push_back(vert);
//			}
//
//			auto& indicies = mesh.indices;
//
//			auto numIndicies = indicies.size();
//			for (size_t i=0; i<numIndicies; i+=3) {
//				Face face = {indicies[i+0].position_index,
//							 indicies[i+1].position_index,
//							 indicies[i+2].position_index};
//				faces.push_back(face);
//			}
//
//			AE_LOG_I("mesh.material_ids.size(): {}", mesh.material_ids.size());
//			AE_LOG_I("mesh.indices.size(): {}", mesh.indices.size());
//			AE_LOG_I("mesh.indices.size()/3: {}", mesh.indices.size()/3);
//
//			for (auto& matID : mesh.material_ids) {
//				AE_LOG_I("Material ID: {}", matID);
//			}
//
//			elements.push_back(make_shared<GeometryElement>(verts, faces));
//
//
//
//			AE_LOG_W("RETURNING FIRST SHAPE");
//
////			auto geometry = make_shared<Geometry>(elements, materials);
////			geometry->name(path.filename().stem().string());
////			return geometry;
//		}
//	}
//	else {
//		AE_LOG_E("Error at line {}: {}", result.error.line, result.error.code.message());
//	}
//
//	return nullptr;
//}

//shared_ptr<Geometry> LoadObj_tiny(const filesystem::path& path) {
//
//	using namespace tinyobj;
//
//	attrib_t objAttrib;
//	vector<shape_t> objShapes;
//	vector<material_t> objMaterials;
//	string err;
//
//	if (tinyobj::LoadObj(&objAttrib, &objShapes, &objMaterials,
//						 &err, path.string().c_str())) {
//
//		// TODO: check using triangles
//
//		if (!err.empty()) {
//			AE_LOG_W(err);
//		}
//
//		// loop over shapes
//		for (auto s=0; s<objShapes.size(); ++s) {
//
//			auto shape = objShapes[s];
//			auto& mesh = shape.mesh;
//
//			// loop over faces (polygon)
//			size_t index_offset = 0;
//			for (auto f=0; f<shape.mesh.num_face_vertices.size(); ++f) {
//
//				auto fv = shape.mesh.num_face_vertices[f];
//
//				// loop over vertices in the face.
//				for (auto v=0; v<fv; ++v) {
//
//					// access to vertex
//					auto idx = shape.mesh.indices[index_offset + v];
//
//					auto vx = objAttrib.vertices[3 * idx.vertex_index + 0];
//					auto vy = objAttrib.vertices[3 * idx.vertex_index + 1];
//					auto vz = objAttrib.vertices[3 * idx.vertex_index + 2];
//
//					// negative = no normal data
//					if (idx.normal_index >= 0) {
//						auto nx = objAttrib.normals[3 * idx.normal_index + 0];
//						auto ny = objAttrib.normals[3 * idx.normal_index + 1];
//						auto nz = objAttrib.normals[3 * idx.normal_index + 2];
//					}
//
//					// negative = no texcoord data
//					if (idx.texcoord_index >= 0) {
//						auto tx = objAttrib.texcoords[2 * idx.texcoord_index + 0];
//						auto ty = objAttrib.texcoords[2 * idx.texcoord_index + 1];
//					}
//
//					// optional: vertex colors
//					// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
//					// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
//					// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
//				}
//				index_offset += fv;
//
//				// per-face material
//				auto objMaterial = objShapes[s].mesh.material_ids[f];
//			}
//
////			auto numFaces = shape.mesh.indices.size();
////
////			for (size_t f = 0; f < numFaces; ++f) {
////				auto index = shape.mesh.indices[f];
////				faces.push_back(Face{index.vertex_index});
////			}
//		}
//	}
//	else {
//		AE_LOG_E("Error loading obj file: {}", err);
//	}
//
//	return nullptr;
//}
