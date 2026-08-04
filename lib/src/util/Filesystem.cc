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
#include <memory>
#include <random>

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

std::optional<std::filesystem::path> a3d::util::filesystem::ExecutablePath() {
#if defined(A3D_MACOS)
    char     path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::filesystem::path(path);
    }
#elif defined(A3D_LINUX)
    // https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
    char    path[PATH_MAX];
    ssize_t count = std::min(size_t(readlink("/proc/self/exe", path, PATH_MAX)), size_t(PATH_MAX - 1));
    if (count >= 0) {
        path[count] = '\0';
        return std::filesystem::path(path);
    }
#elif defined(A3D_WINDOWS)
    char path[PATH_MAX];
    if (GetModuleFileName(NULL, path, PATH_MAX)) {
        return std::filesystem::path(path);
    }
#endif
    return std::nullopt;
}

std::optional<std::filesystem::path> a3d::util::filesystem::ExecutableDirectory() {
    auto execPathStr = ExecutablePath();
    if (execPathStr) {
        auto execPath = std::filesystem::path(*execPathStr);
        return execPath.parent_path();
    }
    return std::nullopt;
}

std::optional<std::string> a3d::util::filesystem::ExecutableName() {
    auto execPathStr = ExecutablePath();
    if (execPathStr) {
        auto execPath = std::filesystem::path(*execPathStr);
        //if (is_regular_file(execPath)) {
        return execPath.filename().string();
        //}
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> a3d::util::filesystem::CurrentWorkingDirectory() {
#ifdef A3D_POSIX
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd))) {
        return std::filesystem::path(cwd);
    }
#else
    char path[PATH_MAX];
    if (GetModuleFileName(NULL, path, PATH_MAX)) {
        return std::filesystem::path(path);
    }
#endif
    return std::nullopt;
}

// *** search paths ***

vector<std::filesystem::path> a3d::util::filesystem::BaseSearchPaths() {
    // build a list of common directories where "shader", "scene", "images", "fonts", etc
    // subdirectories may live.
    // clients will use this to append those subdirectory names to search for specific resources.
    // clients should first check "local" locations first, then "engine" locations.
    // EDIT: this is GROSS. do something else.

    static vector<std::filesystem::path>   basePaths;
    static optional<std::filesystem::path> execDir {};
    static optional<string>                execName = {};

    static bool initd = false;
    if (!initd) {
        basePaths = vector<std::filesystem::path>();

#ifdef A3D_DESKTOP

        execDir = ExecutableDirectory();

        if (execDir) {
            execName = ExecutableName();

            auto path = (*execDir) / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path().parent_path().parent_path() / "tests" / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path().parent_path() / "tests" / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path().parent_path().parent_path() / "demos" / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path().parent_path() / "demos" / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path() / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path().parent_path().parent_path() / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path().parent_path() / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path() / "lib" / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path().parent_path() / "lib" / "data";
            basePaths.push_back(path);

            path =
                (*execDir).parent_path().parent_path().parent_path().parent_path().parent_path().parent_path()
                / "tests" / "data";
            basePaths.push_back(path);

            path =
                (*execDir).parent_path().parent_path().parent_path().parent_path().parent_path().parent_path()
                / "demos" / "data";
            basePaths.push_back(path);

            path =
                (*execDir).parent_path().parent_path().parent_path().parent_path().parent_path().parent_path()
                / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path().parent_path() / "tests" / (*execName) / "data";
            basePaths.push_back(path);

            path = (*execDir).parent_path().parent_path().parent_path() / "demos" / (*execName) / "data";
            basePaths.push_back(path);

            path = (*execDir);
            basePaths.push_back(path);
        }

#elif A3D_WEB

        auto path = "/data";
        basePaths.push_back(path);

        path = "/tests/data";
        basePaths.push_back(path);

        path = "/demos/data";
        basePaths.push_back(path);

#endif

        initd = true;
    }

    return basePaths;
}

vector<std::filesystem::path> a3d::util::filesystem::ShaderSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "shaders");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::filesystem::SceneSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "scenes");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::filesystem::ModelSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "models");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::filesystem::ImageSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "images");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::filesystem::FontSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "fonts");
    }
    return searchPaths;
}

vector<std::filesystem::path> a3d::util::filesystem::AuxiliarySearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (auto& path : BaseSearchPaths()) {
        searchPaths.push_back(path / "auxiliary");
    }
    return searchPaths;
}

std::optional<std::filesystem::path> a3d::util::filesystem::SearchInPaths(const string& filename,
                                                                          const vector<std::filesystem::path>&
                                                                              paths) {
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

std::optional<string> a3d::util::filesystem::TextFile(const std::filesystem::path& path) {
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

std::optional<std::string> a3d::util::filesystem::ShaderSource(const string& name, ShaderType type) {
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

// *** fonts ***

//unique_ptr<a3d::Font> a3d::util::filesystem::FontNamed(const string& filename) {
//	auto path = filesystem::path(filename);
//	return FontNamed(path.stem(), path.extension());
//}

unique_ptr<Font> a3d::util::filesystem::FontNamed(const string& name, const string& type) {
    auto path = SearchInPaths((name + "." + type), FontSearchPaths());
    if (path) {
        log::t()("Found font at path: {}", (*path).string());
        return make_unique<Font>(*path);
    }
    return nullptr;
}

// ***  images ***

unique_ptr<Image> a3d::util::filesystem::ImageNamed(const string& name,
                                                    bool          flipHorizontal,
                                                    bool          flipVertical) {

    return ImageNamed(name, "png", flipHorizontal, flipVertical);
}

unique_ptr<Image> a3d::util::filesystem::ImageNamed(const string& name,
                                                    const string& type,
                                                    bool          flipHorizontal,
                                                    bool          flipVertical) {
    auto path = SearchInPaths((name + "." + type), ImageSearchPaths());
    if (path) {
        log::t()("Found image at path: {}", (*path).string());
        return make_unique<Image>(*path, flipHorizontal, flipVertical);
    }
    return nullptr;
}

unique_ptr<CubeImage> a3d::util::filesystem::CubeImageNamed(const string& name) {
    return CubeImageNamed(name, "png");
}

unique_ptr<CubeImage> a3d::util::filesystem::CubeImageNamed(const string& name, const string& type) {

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

unique_ptr<Scene> a3d::util::filesystem::SceneNamed(const string& name, Scene::ImportOptions options) {

    return SceneNamed(name, "gltf", options);
}

unique_ptr<Scene> a3d::util::filesystem::SceneNamed(const string&        name,
                                                    const string&        type,
                                                    Scene::ImportOptions options) {

    auto path = SearchInPaths((name + "." + type), SceneSearchPaths());
    if (path) {
        log::t()("Found scene at path: {}", (*path).string());
        return Scene::FromFile(*path, options);
    }
    return nullptr;
}

shared_ptr<Mesh> a3d::util::filesystem::MeshNamed(const string& name, Mesh::ImportOptions options) {

    return MeshNamed(name, "gltf", options);
}

shared_ptr<Mesh> a3d::util::filesystem::MeshNamed(const string&       name,
                                                  const string&       type,
                                                  Mesh::ImportOptions options) {

    auto path = SearchInPaths((name + "." + type), ModelSearchPaths());
    if (path) {
        log::t()("Found scene at path: {}", (*path).string());
        return Mesh::FromFile(*path, options);
    }
    return nullptr;
}

// *** other ***

optional<filesystem::path> a3d::util::filesystem::AuxiliaryFilePath(const string& name, const string& type) {

    auto path = SearchInPaths((name + "." + type), AuxiliarySearchPaths());
    if (path) {
        log::t()("Found aux file at path: {}", (*path).string());
        return *path;
    }
    return nullopt;
}
