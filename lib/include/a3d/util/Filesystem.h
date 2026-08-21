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
#include <optional>

#include "a3d/mesh/Mesh.h"
#include "a3d/render/backend/ShaderType.h"
#include "a3d/scene/Scene.h"

namespace a3d {

    class CubeImage;
    class Font;
    class Image;
    class Mesh;
    class Scene;

}

namespace a3d::util::fs {

    // [Process Paths]

    std::optional<std::filesystem::path> ExecutablePath();
    std::optional<std::filesystem::path> ExecutableDirectory();
    std::optional<std::string>           ExecutableName();
    std::optional<std::filesystem::path> CurrentWorkingDirectory();

    // [Images]

    std::unique_ptr<Image>               ImageAt(const std::filesystem::path& resourcePath,
                                                 bool                         flipVertical   = true,
                                                 bool                         flipHorizontal = false);
    std::unique_ptr<CubeImage>           CubeImageAt(const std::filesystem::path& baseFilename);

    // [Scenes]

    std::unique_ptr<Scene>     SceneAt(const std::filesystem::path& resourcePath,
                                       Scene::ImportOptions         options = Scene::ImportOptions::ImportAll);

    // [Meshes]

    std::shared_ptr<Mesh>      MeshAt(const std::filesystem::path& resourcePath,
                                      Mesh::ImportOptions          options = Mesh::ImportOptions::ImportMaterials);

    // [Text]

    std::optional<std::string> TextAt(const std::filesystem::path& resourcePath);

    // [Fonts]

    std::unique_ptr<Font>      FontAt(const std::filesystem::path& resourcePath);

    // [Auxiliary]

    std::optional<std::filesystem::path> AuxiliaryFileAt(const std::filesystem::path& resourcePath);

}

#endif // AVARA3D_UTIL_FILESYSTEM_H
