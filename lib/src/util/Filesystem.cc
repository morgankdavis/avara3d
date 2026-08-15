//
//  Filesystem.cc
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/util/Filesystem.h"

#include <format>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <random>
#include <system_error>

#ifdef A3D_POSIX
    #include <unistd.h>
#endif

#ifdef A3D_LINUX
    #include <linux/limits.h> // PATH_MAX
#endif

#ifdef A3D_MACOS
    #include <CoreGraphics/CoreGraphics.h> // NEEDED?
    #include <mach-o/dyld.h>
    #include <sys/syslimits.h> // PATH_MAX
#endif

#ifdef A3D_WINDOWS
    #include <windows.h> // MAX_PATH
    #define PATH_MAX MAX_PATH
#endif

#include <magic_enum/magic_enum.hpp>

#include "a3d/CubeImage.h"
#include "a3d/Font.h"
#include "a3d/Image.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/scene/Scene.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;

// *** executable and working directories ***

std::optional<std::filesystem::path> a3d::util::fs::ExecutablePath() {
#if defined(A3D_MACOS)
    char     path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::filesystem::path(path);
    }
#elif defined(A3D_LINUX)
    char          path[PATH_MAX];
    const ssize_t count = readlink("/proc/self/exe", path, PATH_MAX - 1);
    if (count < 0) {
        return std::nullopt;
    }
    path[count] = '\0';
    return std::filesystem::path(path);
#elif defined(A3D_WINDOWS)
    char path[PATH_MAX];
    if (GetModuleFileName(NULL, path, PATH_MAX)) {
        return std::filesystem::path(path);
    }
#endif
    return std::nullopt;
}

std::optional<std::filesystem::path> a3d::util::fs::ExecutableDirectory() {
    auto execPathStr = ExecutablePath();
    if (execPathStr) {
        auto execPath = std::filesystem::path(*execPathStr);
        return execPath.parent_path();
    }
    return std::nullopt;
}

std::optional<std::string> a3d::util::fs::ExecutableName() {
    auto execPathStr = ExecutablePath();
    if (execPathStr) {
        auto execPath = std::filesystem::path(*execPathStr);
        //if (is_regular_file(execPath)) {
        return execPath.filename().string();
        //}
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> a3d::util::fs::CurrentWorkingDirectory() {

    std::error_code error;
    auto            path = std::filesystem::current_path(error);
    if (error) {
        return {};
    }
    return path;
}

// *** search paths ***

vector<std::filesystem::path> a3d::util::fs::BaseSearchPaths() {

    // TODO: allow adding new locations at runtime

    static const vector<std::filesystem::path> basePaths = [] {
        vector<std::filesystem::path>   paths;
        optional<std::filesystem::path> execDir {};
        optional<string>                execName {};

#ifdef A3D_DESKTOP

        execDir = ExecutableDirectory();
        execName = ExecutableName();

        if (execDir && execName) {

            const auto ancestor = [&](unsigned depth) {
                auto path = *execDir;
                while (depth-- > 0) {
                    path = path.parent_path();
                }
                return path;
            };

            const auto addAtDepths = [&](const filesystem::path&    relativePath,
                                         initializer_list<unsigned> depths) {
                for (const auto depth : depths) {
                    paths.push_back(ancestor(depth) / relativePath);
                }
            };

            // program-local data

            paths.push_back(*execDir / "data");

            addAtDepths(filesystem::path("tests") / "sandbox" / *execName / "data", {4});
            addAtDepths(filesystem::path("tests") / *execName / "data", {3});
            addAtDepths(filesystem::path("demos") / *execName / "data", {3});

            paths.push_back(*execDir);

            // shared test/demo data

            addAtDepths(filesystem::path("tests") / "data", {4, 3, 6});
            addAtDepths(filesystem::path("demos") / "data", {4, 3, 6});

            // shared project data

            addAtDepths("data", {1, 4, 3, 6});

            // engine data -- last so applications can override engine resources

            addAtDepths(filesystem::path("lib") / "data", {2, 3, 4});
        }

#elif A3D_WEB

        // program data takes precedence over engine data

        auto path = "/tests/data";
        paths.push_back(path);

        path = "/demos/data";
        paths.push_back(path);

        path = "/data";
        paths.push_back(path);

#endif

        return paths;
    }();

    return basePaths;
}

vector<std::filesystem::path> a3d::util::fs::ShaderSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "shaders");
    }
    return searchPaths;
}

vector<filesystem::path> a3d::util::fs::ShaderIncludeSearchPaths() {
    auto searchPaths = vector<filesystem::path>();
    for (const auto& path : ShaderSearchPaths()) {
        searchPaths.push_back(path / "include");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::fs::SceneSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "scenes");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::fs::ModelSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "models");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::fs::ImageSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "images");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::fs::FontSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "fonts");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::fs::AuxiliarySearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "auxiliary");
    }
    return searchPaths;
}

std::optional<std::filesystem::path> a3d::util::fs::SearchInPaths(const string&                        filename,
                                                                  const vector<std::filesystem::path>& paths) {
    // for (size_t i = 0; i < paths.size(); ++i) {
    // 	log::i()("  [{}] '{}'", i, paths[i].string());
    // }

    for (auto& searchPath : paths) {
        if (std::filesystem::is_directory(searchPath)) {
            log::t()("Searching for '{}' in '{}'", filename, searchPath.string());
            auto path = searchPath / filename;
            if (std::filesystem::is_regular_file(path)) {
                //log::d()("Found '{}' at '{}'", searchPath.string(), filename);
                return path;
            }
        }
        // else {
        // 	log::w()("Search path is not a directory: '{}'", searchPath.string());
        // }
    }
    log::w()("'{}' not found.", filename);
    return std::nullopt;
}

// *** binary and text files ***

std::optional<string> a3d::util::fs::TextFile(const std::filesystem::path& path) {
    string   line;
    string   source = "";
    ifstream infile;
    infile.open(path.string());
    if (infile.is_open()) {
        while (!infile.eof()) {
            getline(infile, line);
            source += line;
            source += "\n";
        }
        infile.close();
        return source;
    }
    return std::nullopt;
}

// *** shaders ***

std::optional<std::string> a3d::util::fs::ShaderSource(const string& name, ShaderType type) {
    std::optional<string> rawSource = std::nullopt;
    auto                  extension = "";
    switch (type) {
        case ShaderType::Vertex:
            extension = "vert";
            break;
        case ShaderType::Fragment:
            extension = "frag";
            break;
    }
    auto path = SearchInPaths((name + "." + extension), ShaderSearchPaths());
    if (path) {
        log::t()("Found shader at path: {}", (*path).string());
        rawSource = TextFile(*path);
    }
    return rawSource;
}

optional<string> a3d::util::fs::ShaderIncludeSource(const string& filename) {
    const filesystem::path includePath {filename};
    if (includePath.empty() || includePath.has_root_path()) {
        return nullopt;
    }
    for (const auto& component : includePath) {
        if (component == "..") {
            return nullopt;
        }
    }
    const auto path = SearchInPaths(filename, ShaderIncludeSearchPaths());
    if (!path) {
        return nullopt;
    }
    log::t()("Found shader include at path: {}", path->string());
    return TextFile(*path);
}

// *** fonts ***

//unique_ptr<a3d::Font> a3d::util::filesystem::FontNamed(const string& filename) {
//	auto path = filesystem::path(filename);
//	return FontNamed(path.stem(), path.extension());
//}

unique_ptr<Font> a3d::util::fs::FontNamed(const string& name, const string& type) {
    auto path = SearchInPaths((name + "." + type), FontSearchPaths());
    if (path) {
        log::t()("Found font at path: {}", (*path).string());
        return make_unique<Font>(*path);
    }
    return nullptr;
}

// ***  images ***

unique_ptr<Image> a3d::util::fs::ImageNamed(const string& name,
                                            const string& type,
                                            bool          flipVertical,
                                            bool          flipHorizontal) {

    auto path = SearchInPaths((name + "." + type), ImageSearchPaths());
    if (path) {
        log::t()("Found image at path: {}", (*path).string());
        return make_unique<Image>(*path, flipVertical, flipHorizontal);
    }
    return nullptr;
}

unique_ptr<CubeImage> a3d::util::fs::CubeImageNamed(const string& name) {
    return CubeImageNamed(name, "png");
}

unique_ptr<CubeImage> a3d::util::fs::CubeImageNamed(const string& name, const string& type) {

    // panorama to cubemap: https://jaxry.github.io/panorama-to-cubemap/

    return make_unique<CubeImage>(std::array<unique_ptr<Image>,
                                             6> {ImageNamed(name + "_xpos", type, false, true),
                                                 ImageNamed(name + "_xneg", type, false, true),
                                                 ImageNamed(name + "_ypos", type, true, false),
                                                 ImageNamed(name + "_yneg", type, true, false),
                                                 ImageNamed(name + "_zpos", type, false, true),
                                                 ImageNamed(name + "_zneg", type, false, true)});
}

// *** scenes ***

unique_ptr<Scene> a3d::util::fs::SceneNamed(const string& name, Scene::ImportOptions options) {

    return SceneNamed(name, "gltf", options);
}

unique_ptr<Scene> a3d::util::fs::SceneNamed(const string&        name,
                                            const string&        type,
                                            Scene::ImportOptions options) {

    auto path = SearchInPaths((name + "." + type), SceneSearchPaths());
    if (path) {
        log::t()("Found scene at path: {}", (*path).string());
        return Scene::FromFile(*path, options);
    }
    return nullptr;
}

shared_ptr<Mesh> a3d::util::fs::MeshNamed(const string& name, Mesh::ImportOptions options) {

    return MeshNamed(name, "gltf", options);
}

shared_ptr<Mesh> a3d::util::fs::MeshNamed(const string& name, const string& type, Mesh::ImportOptions options) {

    auto path = SearchInPaths((name + "." + type), ModelSearchPaths());
    if (path) {
        log::t()("Found scene at path: {}", (*path).string());
        return Mesh::FromFile(*path, options);
    }
    return nullptr;
}

// *** other ***

optional<filesystem::path> a3d::util::fs::AuxiliaryFilePath(const string& name, const string& type) {

    auto path = SearchInPaths((name + "." + type), AuxiliarySearchPaths());
    if (path) {
        log::t()("Found aux file at path: {}", (*path).string());
        return *path;
    }
    return nullopt;
}
