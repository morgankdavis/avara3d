//
//  Filesystem.h
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_FILESYSTEM_H
#define AVARA3D_UTIL_FILESYSTEM_H

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include "a3d/mesh/Mesh.h"
#include "a3d/scene/Scene.h"

namespace a3d {

    class CubeImage;
    class Font;
    class Image;

}

namespace a3d::util::fs {

    // [Public Process Path Functions]

    /** @brief Returns the path to the current executable, or std::nullopt if unavailable. */
    std::optional<std::filesystem::path> ExecutablePath();

    /** @brief Returns the directory containing the current executable, or std::nullopt if unavailable. */
    std::optional<std::filesystem::path> ExecutableDirectory();

    /** @brief Returns the current executable filename without its extension, or std::nullopt if unavailable. */
    std::optional<std::string>           ExecutableName();

    /** @brief Returns the current working directory, or std::nullopt if unavailable. */
    std::optional<std::filesystem::path> CurrentWorkingDirectory();

    // [Public Image Functions]

    /** @brief Loads an image resource, optionally flipping it vertically or horizontally; returns nullptr if not found. */
    std::unique_ptr<Image>               ImageAt(const std::filesystem::path& resourcePath,
                                                 bool                         flipVertical   = true,
                                                 bool                         flipHorizontal = false);

    /**
     * @brief Loads a cubemap from six image resources derived from @p baseFilename.
     *
     * Face filenames append _x_pos, _x_neg, _y_pos, _y_neg, _z_pos, or _z_neg before the extension.
     *
     * @throws std::invalid_argument if a face cannot be loaded or the face formats are incompatible.
     */
    std::unique_ptr<CubeImage>           CubeImageAt(const std::filesystem::path& baseFilename);

    // [Public Scene Functions]

    /** @brief Loads a scene resource with @p options; returns nullptr if not found. */
    std::unique_ptr<Scene>               SceneAt(const std::filesystem::path& resourcePath,
                                                 Scene::ImportOptions         options = Scene::ImportOptions::ImportAll);

    // [Public Mesh Functions]

    /** @brief Loads a mesh resource with @p options; returns nullptr if not found. */
    std::shared_ptr<Mesh>                MeshAt(const std::filesystem::path& resourcePath,
                                                Mesh::ImportOptions          options = Mesh::ImportOptions::ImportMaterials);

    // [Public Text Functions]

    /** @brief Loads a text resource; returns std::nullopt if it is not found or cannot be opened. */
    std::optional<std::string>           TextAt(const std::filesystem::path& resourcePath);

    // [Public Font Functions]

    /** @brief Loads a font resource; returns nullptr if not found. */
    std::unique_ptr<Font>                FontAt(const std::filesystem::path& resourcePath);

    // [Public Auxiliary Functions]

    /** @brief Returns the resolved path to an auxiliary resource, or std::nullopt if not found. */
    std::optional<std::filesystem::path> AuxiliaryFileAt(const std::filesystem::path& resourcePath);

}

#endif // AVARA3D_UTIL_FILESYSTEM_H
