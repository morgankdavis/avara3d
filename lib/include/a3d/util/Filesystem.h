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
#include <vector>

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

    // [Shaders]

    std::optional<std::string>           ShaderSource(const std::string& name, ShaderType type);
    std::optional<std::string>           ShaderIncludeSource(const std::string& filename);

    // [Fonts]

    std::unique_ptr<a3d::Font>           FontNamed(const std::filesystem::path& filename);

    // [Images]

    std::unique_ptr<a3d::Image>          ImageNamed(const std::filesystem::path& filename,
                                                    bool                         flipVertical   = true,
                                                    bool                         flipHorizontal = false);
    std::unique_ptr<a3d::CubeImage>      CubeImageNamed(const std::filesystem::path& filename);

    // [Scenes]

    std::unique_ptr<a3d::Scene> SceneNamed(const std::filesystem::path& filename,
                                           Scene::ImportOptions options = Scene::ImportOptions::ImportAll);

    // [Meshes]

    std::shared_ptr<a3d::Mesh>  MeshNamed(const std::filesystem::path& filename,
                                          Mesh::ImportOptions options = Mesh::ImportOptions::ImportMaterials);

    // [Auxiliary]

    std::optional<std::filesystem::path> AuxiliaryFilePath(const std::filesystem::path& filename);

}

#endif //AVARA3D_UTIL_FILESYSTEM_H
