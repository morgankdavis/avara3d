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

namespace a3d::util::filesystem {

    // *** executable and working directories ***

#ifndef A3D_ANDROID
    std::optional<std::filesystem::path> ExecutablePath();
    std::optional<std::filesystem::path> ExecutableDirectory();
    std::optional<std::string>           ExecutableName();
    std::optional<std::filesystem::path> CurrentWorkingDirectory();
#endif

    // *** search paths ***

#ifndef A3D_ANDROID
    std::vector<std::filesystem::path>   BaseSearchPaths();
    std::vector<std::filesystem::path>   ShaderSearchPaths();
    std::vector<std::filesystem::path>   SceneSearchPaths();
    std::vector<std::filesystem::path>   ModelSearchPaths();
    std::vector<std::filesystem::path>   ImageSearchPaths();
    std::vector<std::filesystem::path>   FontSearchPaths();
    std::vector<std::filesystem::path>   AuxiliarySearchPaths();
    std::optional<std::filesystem::path> SearchInPaths(const std::string&                        filename,
                                                       const std::vector<std::filesystem::path>& paths);
#endif

    // *** binary and text files ***

#ifdef A3D_ANDROID
    std::optional<std::filesystem::path> InternalFilesDirectory();
    std::optional<std::string>           TextAsset(const std::string& relPath);
    std::shared_ptr<Buffer>              BinaryAsset(const std::string& relPath);
#else
    std::optional<std::string> TextFile(const std::filesystem::path& path);
#endif

    // *** shaders ***

    std::optional<std::string>      ShaderSource(const std::string& name, ShaderType type);

    // *** fonts ***

//		std::unique_ptr<a3d::Font> 		FontNamed(const std::string& filename);
    std::unique_ptr<a3d::Font>      FontNamed(const std::string& name, const std::string& type);

    // ***  images ***

    std::unique_ptr<a3d::Image>     ImageNamed(const std::string& name,
                                               bool               flipHorizontal = true,
                                               bool               flipVertical   = false);
    std::unique_ptr<a3d::Image>     ImageNamed(const std::string& name,
                                               const std::string& type,
                                               bool               flipHorizontal = true,
                                               bool               flipVertical   = false);
    std::unique_ptr<a3d::CubeImage> CubeImageNamed(const std::string& name);
    std::unique_ptr<a3d::CubeImage> CubeImageNamed(const std::string& name, const std::string& type);

    // *** scenes ***

#ifndef A3D_ANDROID
    std::unique_ptr<a3d::Scene> SceneNamed(const std::string&   name,
                                           Scene::ImportOptions options = Scene::ImportOptions::ImportAll);
    std::unique_ptr<a3d::Scene> SceneNamed(const std::string&   name,
                                           const std::string&   type,
                                           Scene::ImportOptions options = Scene::ImportOptions::ImportAll);
    std::shared_ptr<a3d::Mesh>  MeshNamed(const std::string&  name,
                                          Mesh::ImportOptions options = Mesh::ImportOptions::ImportMaterials);
    std::shared_ptr<a3d::Mesh>  MeshNamed(const std::string&  name,
                                          const std::string&  type,
                                          Mesh::ImportOptions options = Mesh::ImportOptions::ImportMaterials);

    // *** aux ***

    std::optional<std::filesystem::path> AuxiliaryFilePath(const std::string& name, const std::string& type);
#endif

}

#endif //AVARA3D_UTIL_FILESYSTEM_H
