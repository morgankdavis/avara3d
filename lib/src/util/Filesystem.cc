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
#include <string_view>
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

#include "a3d/CubeImage.h"
#include "a3d/Font.h"
#include "a3d/Image.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/scene/Scene.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;

// [Private Search Paths Prototypes]

static const std::vector<std::filesystem::path>& ResourceSearchPaths();
static std::vector<std::filesystem::path>        ShaderSearchPaths();
static std::vector<std::filesystem::path>        SceneSearchPaths();
static std::vector<std::filesystem::path>        ModelSearchPaths();
static std::vector<std::filesystem::path>        ImageSearchPaths();
static std::vector<std::filesystem::path>        FontSearchPaths();
static std::vector<std::filesystem::path>        AuxiliarySearchPaths();
static std::optional<std::filesystem::path>      SearchInPaths(const std::filesystem::path& resourcePath,
                                                               const std::vector<std::filesystem::path>& paths);

// [Process]

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
        return {};
    }
    path[count] = '\0';
    return std::filesystem::path(path);
#elif defined(A3D_WINDOWS)
    char path[PATH_MAX];
    if (GetModuleFileName(NULL, path, PATH_MAX)) {
        return std::filesystem::path(path);
    }
#endif
    return {};
}

optional<filesystem::path> a3d::util::fs::ExecutableDirectory() {

    auto execPath = ExecutablePath();
    if (execPath) {
        return execPath->parent_path();
    }

    return nullopt;
}

optional<string> a3d::util::fs::ExecutableName() {

    auto execPath = ExecutablePath();
    if (execPath) {
        return execPath->stem().string();
    }
    return nullopt;
}

std::optional<std::filesystem::path> a3d::util::fs::CurrentWorkingDirectory() {

    std::error_code error;
    auto            path = std::filesystem::current_path(error);
    if (error) {
        return {};
    }
    return path;
}

// [Images]

unique_ptr<Image> a3d::util::fs::ImageAt(const filesystem::path& resourcePath,
                                         bool                    flipVertical,
                                         bool                    flipHorizontal) {

    auto path = SearchInPaths(resourcePath, ImageSearchPaths());
    if (path) {
        log::t()("Found image at path: {}", path->string());
        return make_unique<Image>(*path, flipVertical, flipHorizontal);
    }

    return nullptr;
}

unique_ptr<CubeImage> a3d::util::fs::CubeImageAt(const filesystem::path& baseFilename) {

    const auto faceFilename = [&](string_view face) {
        return baseFilename.parent_path()
               / format("{}_{}{}", baseFilename.stem().string(), face, baseFilename.extension().string());
    };

    return make_unique<CubeImage>(std::array<unique_ptr<Image>, 6> {
        ImageAt(faceFilename("x_pos"), false, true),
        ImageAt(faceFilename("x_neg"), false, true),
        ImageAt(faceFilename("y_pos"), true, false),
        ImageAt(faceFilename("y_neg"), true, false),
        ImageAt(faceFilename("z_pos"), false, true),
        ImageAt(faceFilename("z_neg"), false, true),
    });
}

// [Scenes]

unique_ptr<Scene> a3d::util::fs::SceneAt(const filesystem::path& resourcePath, Scene::ImportOptions options) {

    auto path = SearchInPaths(resourcePath, SceneSearchPaths());
    if (path) {
        log::t()("Found scene at path: {}", path->string());
        return Scene::FromFile(*path, options);
    }

    return nullptr;
}

// [Meshes]

shared_ptr<Mesh> a3d::util::fs::MeshAt(const filesystem::path& resourcePath, Mesh::ImportOptions options) {

    auto path = SearchInPaths(resourcePath, ModelSearchPaths());
    if (path) {
        log::t()("Found mesh at path: {}", path->string());
        return Mesh::FromFile(*path, options);
    }

    return nullptr;
}

// [Text]

optional<string> a3d::util::fs::TextAt(const filesystem::path& resourcePath) {

    auto path = SearchInPaths(resourcePath, ResourceSearchPaths());
    if (!path) {
        return {};
    }

    log::t()("Found text file at path: {}", path->string());

    ifstream file(*path);
    if (!file) {
        return {};
    }

    return string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}

// [Fonts]

unique_ptr<Font> a3d::util::fs::FontAt(const filesystem::path& resourcePath) {

    auto path = SearchInPaths(resourcePath, FontSearchPaths());
    if (path) {
        log::t()("Found font at path: {}", path->string());
        return make_unique<Font>(*path);
    }

    return nullptr;
}

// [Auxiliary]

optional<filesystem::path> a3d::util::fs::AuxiliaryFileAt(const filesystem::path& resourcePath) {

    auto path = SearchInPaths(resourcePath, AuxiliarySearchPaths());
    if (path) {
        log::t()("Found auxiliary file at path: {}", path->string());
        return path;
    }

    return nullopt;
}

// [Private Search Paths Implementations]

const vector<filesystem::path>& ResourceSearchPaths() {

    static const vector<filesystem::path> paths = [] {
        vector<filesystem::path>        paths;
        optional<std::filesystem::path> execDir {};
        optional<string>                execName {};

#ifdef A3D_DESKTOP

        execDir = a3d::util::fs::ExecutableDirectory();
        execName = a3d::util::fs::ExecutableName();

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

    return paths;
}

vector<std::filesystem::path> ShaderSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (const auto& path : ResourceSearchPaths()) {
        searchPaths.push_back(path / "shaders");
    }
    return searchPaths;
}

vector<std::filesystem::path> SceneSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (const auto& path : ResourceSearchPaths()) {
        searchPaths.push_back(path / "scenes");
    }
    return searchPaths;
}

vector<std::filesystem::path> ModelSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (const auto& path : ResourceSearchPaths()) {
        searchPaths.push_back(path / "models");
    }
    return searchPaths;
}

vector<std::filesystem::path> ImageSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (const auto& path : ResourceSearchPaths()) {
        searchPaths.push_back(path / "images");
    }
    return searchPaths;
}

vector<std::filesystem::path> FontSearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (const auto& path : ResourceSearchPaths()) {
        searchPaths.push_back(path / "fonts");
    }
    return searchPaths;
}

vector<std::filesystem::path> AuxiliarySearchPaths() {
    auto searchPaths = vector<std::filesystem::path>();
    for (const auto& path : ResourceSearchPaths()) {
        searchPaths.push_back(path / "auxiliary");
    }
    return searchPaths;
}

optional<filesystem::path> SearchInPaths(const filesystem::path&         resourcePath,
                                         const vector<filesystem::path>& paths) {

    if (resourcePath.is_absolute()) {

        if (filesystem::is_regular_file(resourcePath)) {
            return resourcePath;
        }

        log::w()("'{}' not found.", resourcePath.string());
        return {};
    }

    for (const auto& searchPath : paths) {

        if (filesystem::is_directory(searchPath)) {

            log::t()("Searching for '{}' in '{}'", resourcePath.string(), searchPath.string());

            auto path = searchPath / resourcePath;
            if (filesystem::is_regular_file(path)) {
                return path;
            }
        }
    }

    log::w()("'{}' not found.", resourcePath.string());
    return {};
}
