//
//  Mesh.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/Mesh.h"

#include <utility>

#include "a3d/Color.h"
#include "a3d/IdGenerator.h"
#include "a3d/Image.h"
#include "a3d/log/Log.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/scene/importer/GlTFImporter.h"
#include "a3d/visual/material/Material.h"
#include "a3d/util/Chrono.h"
using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Static Member Functions ///

shared_ptr<Mesh> Mesh::FromFile(const filesystem::path& path, ImportOptions options) {
    auto optsUnderlying = static_cast<underlying_type<ImportOptions>::type>(options);
    auto sceneOpts = Scene::ImportOptions(optsUnderlying) | Scene::ImportOptions::ImportMeshes;
    auto mesh = GlTFImporter(path, sceneOpts).firstMesh();
    return mesh;
}

/// Public Lifecycle Functions ///

Mesh::Mesh(const string& name, unique_ptr<MeshElement> element, const shared_ptr<Material>& material):
    Mesh {std::move(element), material} {

    _name = name;
}

Mesh::Mesh(unique_ptr<MeshElement> element, const shared_ptr<Material>& material):
    Mesh {} {

    if (element) {
        _elements.push_back(std::move(element));
    }
    if (material) {
        _materials.push_back(material);
    }

    genLocalAABB();
}

Mesh::Mesh(const string&                       name,
           vector<unique_ptr<MeshElement>>&    elements,
           const vector<shared_ptr<Material>>& materials):
    Mesh {elements, materials} {

    _name = name;
}

Mesh::Mesh(vector<unique_ptr<MeshElement>>& elements, const vector<shared_ptr<Material>>& materials):
    Mesh {} {

    _elements = vector<unique_ptr<MeshElement>>();
    _elements.reserve(elements.size());
    _elements.insert(_elements.end(), std::make_move_iterator(elements.begin()),
                     std::make_move_iterator(elements.end()));
    _materials = materials;

    genLocalAABB();
}

Mesh::~Mesh() {

    if (_name != nullopt) {
        log::d()("Destroying Mesh '{}' ({:p})", *_name, static_cast<void*>(this));
    }
    else {
        log::d()("Destroying Mesh {:p}", static_cast<void*>(this));
    }
}

/// Public Member Functions ///

optional<string> Mesh::name() const {
    return _name;
}

void Mesh::name(const string& name) {
    _name = name;
}

const vector<unique_ptr<MeshElement>>& Mesh::elements() {
    return _elements;
}

const vector<shared_ptr<Material>>& Mesh::materials() {
    return _materials;
}

shared_ptr<Material> Mesh::firstMaterial() const {
    if (!_materials.empty()) {
        return _materials[0];
    }
    return nullptr;
}

shared_ptr<Material> Mesh::materialNamed(const string& name) const {
    for (const auto& material : _materials) {
        if (material->name() == name) {
            return material;
        }
    }
    return nullptr;
}

void Mesh::addMaterial(const shared_ptr<Material>& material) {
    _materials.push_back(material);
}

void Mesh::insertMaterial(const shared_ptr<Material>& material, int index) {
    _materials.insert(_materials.begin() + index, material);
}

void Mesh::removeMaterial(int index) {
    //	if (_materials.size() >= index-1) { // fails when index==0
    _materials.erase(_materials.begin() + index);
    //	}
}

void Mesh::replaceMaterial(int index, const shared_ptr<Material>& replacement) {
    removeMaterial(index);
    insertMaterial(replacement, index);
}

/// Internal Member Functions ///

MeshId Mesh::id() const noexcept {
    return _id;
}

const AABB& Mesh::localAABB() const {
    return _localAABB;
}

AABB Mesh::worldAABB(const math::mat4& worldTransform, bool vertfit) const {

    // TODO: consolidate (MeshElement has the same function)

    if (vertfit) {
        static const float maxFloat = math::F32_MAX;
        static const float minFloat = math::F32_LOWEST;
        AABB               out = {{maxFloat, maxFloat, maxFloat}, {minFloat, minFloat, minFloat}};

        for (const auto& e : _elements) {
            AABB ea = e->worldAABB(worldTransform, true);
            out.min = min(out.min, ea.min);
            out.max = max(out.max, ea.max);
        }
        return out;
    }
    // fit over OBB - looser - fast!
    else {

        auto localAABB = Mesh::localAABB();

        const vec3 c = (localAABB.min + localAABB.max) / 2.0f; // local center
        const vec3 e = (localAABB.max - localAABB.min) / 2.0f; // local half extents

        const vec3 C = vec3 {worldTransform * vec4(c, 1.0f)}; // world center

        // linear part (rotation/scale/shear)
        const mat3 L = math::mat3 {worldTransform};

        const mat3 A = math::abs(L);

        const vec3 E = A * e; // world half extents

        return AABB {C - E, C + E};
    }
}

vec3 Mesh::localExtent() const {
    auto aabb = localAABB();
    return aabb.max - aabb.min;
}

vec3 Mesh::worldExtent(const mat4& worldTransform) const {
    auto aabb = worldAABB(worldTransform, true);
    return aabb.max - aabb.min;
}

void Mesh::burnTransform(const mat4& transform, bool normals) {
    for (auto& element : elements()) {
        element->burnTransform(transform, normals);
    }

    genLocalAABB();
}

Mesh::DirtyMask Mesh::dirtyMask() const {
    return _dirtyMask;
}

void Mesh::dirtyMask(DirtyMask mask) {
    _dirtyMask = mask;
}

// Protected Member Functions ///

void Mesh::genLocalAABB() {

    static const float maxFloat = math::F32_MAX;
    static const float minFloat = math::F32_LOWEST;
    AABB               aabb = {{maxFloat, maxFloat, maxFloat}, {minFloat, minFloat, minFloat}};

    for (const auto& element : _elements) {
        auto elementAABB = element->localAABB();
        aabb.min = min(aabb.min, elementAABB.min);
        aabb.max = max(aabb.max, elementAABB.max);
    }

    _localAABB = aabb;
}

/// Private Lifecycle Functions ///

Mesh::Mesh():
    _id {IdGenerator<MeshId>::next()},
    _name {},
    _elements {},
    _materials {},
    _dirtyMask {DirtyMask::All} {}
