//
//  Mesh.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_MESH_H
#define AVARA3D_MESH_MESH_H

#include <climits>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "a3d/Id.h"
#include "a3d/Math.h"
#include "a3d/mesh/AABB.h"
#include "a3d/util/Bitmask.h"

namespace a3d {

    class Line;
    class Material;
    class MeshElement;
    class Node;
    class Renderer;
    class RenderContext;
    class RenderItem;

    class Mesh {

    public:
        /// Public Types ///

        enum class ImportOptions : uint16_t {
            None            = 0,
            ImportMaterials = 1 << 1, // note maps to SceneImportOptions
            ImportAll       = UINT16_MAX
        };

        /// Public Static Member Functions ///

        // imports the first mesh in the specified file, with no node transforms applied.
        static std::shared_ptr<Mesh> FromFile(const std::filesystem::path& path,
                                              ImportOptions options = ImportOptions::ImportMaterials);

        /// Public Lifecycle Functions ///

        Mesh(const std::string&               name,
             std::unique_ptr<MeshElement>     element,
             const std::shared_ptr<Material>& material);
        Mesh(std::unique_ptr<MeshElement> element, const std::shared_ptr<Material>& material);
        Mesh(const std::string&                            name,
             std::vector<std::unique_ptr<MeshElement>>&    elements,
             const std::vector<std::shared_ptr<Material>>& materials);
        Mesh(std::vector<std::unique_ptr<MeshElement>>&    elements,
             const std::vector<std::shared_ptr<Material>>& materials);
        virtual ~Mesh();

        Mesh(const Mesh&)            = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&&)                                                       = delete;
        Mesh&                                            operator=(Mesh&&) = delete;

        /// Public Member Functions ///

        std::optional<std::string>                       name() const;
        void                                             name(const std::string& name);

        const std::vector<std::unique_ptr<MeshElement>>& elements();
        const std::vector<std::shared_ptr<Material>>&    materials();

        std::shared_ptr<Material>                        firstMaterial() const;
        std::shared_ptr<Material>                        materialNamed(const std::string& name) const;
        void                                             addMaterial(const std::shared_ptr<Material>& material);
        void insertMaterial(const std::shared_ptr<Material>& material, int index);
        void removeMaterial(int index);
        void replaceMaterial(int index, const std::shared_ptr<Material>& replacement);

        /// Internal Types ///

        enum class DirtyMask : uint32_t {
            None = 0,
            // AABB?
            All = UINT_MAX
        };

        /// Internal Member Functions ///

        MeshId      id() const noexcept;

        const AABB& localAABB() const;
        AABB        worldAABB(const math::mat4& worldMat, bool vertfit) const;
        math::vec3  localExtent() const;
        math::vec3  worldExtent(const math::mat4& worldTransform) const;

        void        burnTransform(const math::mat4& transform, bool normals);

        DirtyMask   dirtyMask() const;
        void        dirtyMask(DirtyMask mask);

    protected:
        /// Protected Member Functions ///

        void                                      genLocalAABB();

        /// Protected Member Variables ///

        std::vector<std::unique_ptr<MeshElement>> _elements;
        std::vector<std::shared_ptr<Material>>    _materials;

    private:
        /// Private Lifecycle Functions ///

        Mesh();

        /// Private Member Variables ///

        MeshId                     _id;
        std::optional<std::string> _name;
        AABB                       _localAABB;
        DirtyMask                  _dirtyMask;
    };

    namespace util::bitmask {

        template<>
        struct enable_ops<Mesh::ImportOptions> : std::true_type {};

        template<>
        struct enable_ops<Mesh::DirtyMask> : std::true_type {};

    }
}

#endif /* AVARA3D_MESH_MESH_H */
