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

    /**
     * @brief Owns renderable geometry elements and their materials.
     *
     * A Mesh owns its MeshElement instances and shares ownership of its Material
     * instances. When rendered, elements select materials by element index, wrapping
     * through the material list when fewer materials than elements are present. If
     * no material is available for an element, the renderer uses its default material.
     */
    class Mesh {

    public:
        // [Public Types]

        /** @brief Selects optional resources imported with Mesh::FromFile(). */
        enum class ImportOptions : uint16_t {
            None = 0, ///< Do not import source materials.

            // note maps to SceneImportOptions
            ImportMaterials = 1 << 1,    ///< Import materials referenced by the mesh.
            ImportAll       = UINT16_MAX ///< Enable all supported Mesh import options.
        };

        // [Public Static Member Functions]

        // imports the first mesh in the specified file, with no node transforms applied.
        /**
         * @brief Imports the first mesh in a glTF file without applying scene-node transforms.
         *
         * Mesh geometry is always imported; @p options controls optional associated resources.
         *
         * @param path glTF or GLB file to import.
         * @param options optional resources to import with the mesh.
         * @return The first imported mesh, or nullptr if the file cannot be parsed or contains no meshes.
         * @throws std::runtime_error if @p path has an unsupported file extension.
         */
        static std::shared_ptr<Mesh> FromFile(const std::filesystem::path& path,
                                              ImportOptions options = ImportOptions::ImportMaterials);

        // [Public Lifecycle Functions]

        /**
         * @brief Creates a named Mesh and takes ownership of @p element.
         *
         * A non-null @p material is retained with shared ownership.
         */
        Mesh(const std::string&               name,
             std::unique_ptr<MeshElement>     element,
             const std::shared_ptr<Material>& material);

        /**
         * @brief Creates a Mesh and takes ownership of @p element.
         *
         * A non-null @p material is retained with shared ownership.
         */
        Mesh(std::unique_ptr<MeshElement> element, const std::shared_ptr<Material>& material);

        /**
         * @brief Creates a named Mesh by moving the elements in @p elements into the Mesh.
         *
         * Materials in @p materials are retained with shared ownership.
         */
        Mesh(const std::string&                            name,
             std::vector<std::unique_ptr<MeshElement>>&    elements,
             const std::vector<std::shared_ptr<Material>>& materials);

        /**
         * @brief Creates a Mesh by moving the elements in @p elements into the Mesh.
         *
         * Materials in @p materials are retained with shared ownership.
         */
        Mesh(std::vector<std::unique_ptr<MeshElement>>&    elements,
             const std::vector<std::shared_ptr<Material>>& materials);
        virtual ~Mesh();

        Mesh(const Mesh&)            = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&&)                                                       = delete;
        Mesh&                                            operator=(Mesh&&) = delete;

        // [Public Member Functions]

        /** @brief Returns the optional mesh name. */
        std::optional<std::string>                       name() const;

        /** @brief Sets the mesh name. */
        void                                             name(const std::string& name);

        /** @brief Returns the MeshElement instances owned by this Mesh. */
        const std::vector<std::unique_ptr<MeshElement>>& elements();

        /** @brief Returns the materials retained by this Mesh. */
        const std::vector<std::shared_ptr<Material>>&    materials();

        /** @brief Returns the first material, or nullptr if the material list is empty. */
        std::shared_ptr<Material>                        firstMaterial() const;

        /** @brief Returns the first material named @p name, or nullptr if no material matches. */
        std::shared_ptr<Material>                        materialNamed(const std::string& name) const;

        /** @brief Appends @p material to the material list. */
        void                                             addMaterial(const std::shared_ptr<Material>& material);

        /**
         * @brief Inserts @p material at @p index.
         *
         * @param index insertion position in the range [0, materials().size()].
         */
        void insertMaterial(const std::shared_ptr<Material>& material, int index);

        /**
         * @brief Removes the material at @p index.
         *
         * @param index valid zero-based material index.
         */
        void removeMaterial(int index);

        /**
         * @brief Replaces the material at @p index with @p replacement.
         *
         * @param index valid zero-based material index.
         */
        void replaceMaterial(int index, const std::shared_ptr<Material>& replacement);

        // [Internal Types]

        enum class DirtyMask : uint32_t {
            None = 0,
            // AABB?
            All = UINT_MAX
        };

        // [Internal Member Functions]

        MeshId      id() const noexcept;

        const AABB& localAABB() const;
        AABB        worldAABB(const math::mat4& worldMat, bool vertfit) const;
        math::vec3  localExtent() const;
        math::vec3  worldExtent(const math::mat4& worldTransform) const;

        void        burnTransform(const math::mat4& transform, bool normals);

        DirtyMask   dirtyMask() const;
        void        dirtyMask(DirtyMask mask);

    protected:
        // [Protected Member Functions]

        void                                      genLocalAABB();

        // [Protected Member Variables]

        std::vector<std::unique_ptr<MeshElement>> _elements;
        std::vector<std::shared_ptr<Material>>    _materials;

    private:
        // [Private Lifecycle Functions]

        Mesh();

        // [Private Member Variables]

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

#endif // AVARA3D_MESH_MESH_H
