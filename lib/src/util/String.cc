//
//  String.cc
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/util/String.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <ctime>
#include <format>
#include <fstream>
#include <sstream>

#ifdef A3D_POSIX
    #ifndef A3D_WEB
        #include <execinfo.h>
    #endif
    #include <unistd.h>
    #include <sys/time.h>
#endif

#ifdef A3D_MACOS
    #include <CoreGraphics/CoreGraphics.h> // NEEDED?
    #include <mach-o/dyld.h>
#endif

#include "a3d/mesh/Mesh.h"
#include "a3d/scene/Node.h"
#include "a3d/visual/camera/Camera.h"
#include "a3d/visual/light/Light.h"
#include "a3d/visual/material/Material.h"

using namespace std;

namespace a3d::util::string {

namespace {

    // [Private Non-Member Prototypes]

    void TreeStringRec(Node& n, stringstream& ss, unsigned depth);

} // namespace

void Replace(std::string& str, const std::string& oldStr, const std::string& newStr) {
    std::string::size_type pos = 0u;
    while ((pos = str.find(oldStr, pos)) != std::string::npos) {
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
}

vector<std::string> Split(const std::string& s, std::string delim) {
    // https://stackoverflow.com/a/46931770

    size_t              pos_start = 0, pos_end, delim_len = delim.length();
    std::string         token;
    vector<std::string> res;

    while ((pos_end = s.find(delim, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }
    res.push_back(s.substr(pos_start));

    return res;
}

std::string Uppercase(std::string_view s) {
    std::string result {s};

    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });

    return result;
}

std::string Lowercase(std::string_view s) {
    std::string result {s};

    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    return result;
}

std::string Tree(const Node& root) {

    stringstream ss;
    std::string  name = (root.name() ? "\"" + *(root.name()) + "\"" : "null");
    ss << "[NODE] (" << static_cast<const void*>(&root) << ", " << name << ")" << endl;

    unsigned depth = 0;

    for (const auto& c : root.children(false)) {
        TreeStringRec(*c, ss, depth + 1);
    }

    return ss.str();
}

std::string Timestamp() {
    constexpr size_t BUF_SIZE = 128;
    char             buf[BUF_SIZE];
#ifdef A3D_WINDOWS
    time_t     rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf, sizeof(buf), "%Y.%m.%d_%I.%M.%S", timeinfo);
    return std::string(buf);
#else
    timeval curTime;
    gettimeofday(&curTime, NULL); // gettimeofday() is POSIX
    int milli = curTime.tv_usec / 1000;
    strftime(buf, sizeof(buf), "%Y.%m.%d_%H.%M.%S", localtime(&curTime.tv_sec));
    char msBuf[std::strlen(buf) + 5];
    snprintf(msBuf, sizeof(msBuf), "%s.%03d", buf, milli);
    return std::string(msBuf);
#endif
}

namespace {

    // [Private Non-Member Functions]

    void TreeStringRec(Node& n, stringstream& ss, unsigned depth) {

        std::string padding = "";
        for (unsigned d = 0; d < depth; ++d) {
            padding += "\t";
        }
        std::string nodeName = (n.name() ? "\"" + *(n.name()) + "\"" : "null");
        ss << padding << "[NODE] (" << static_cast<const void*>(&n) << ", " << nodeName << ")" << endl;

        auto mesh = n.mesh();
        if (mesh) {
            std::string meshName = (mesh->name() ? "\"" + *(mesh->name()) + "\"" : "null");
            ss << padding << "\t[MESH] (" << static_cast<const void*>(mesh.get()) << ", " << meshName << ")"
               << endl;

            for (auto& element : mesh->elements()) {
                ss << padding << "\t\t[ELEMENT] (" << static_cast<const void*>(element.get()) << ")" << endl;
            }

            for (auto& material : mesh->materials()) {

                std::string properties = "";
                if (!holds_alternative<monostate>(material->ambient())) {
                    properties += "a";
                }
                if (!holds_alternative<monostate>(material->diffuse())) {
                    properties += "d";
                }
                if (!holds_alternative<monostate>(material->specular())) {
                    properties += "s";
                }
                if (!holds_alternative<monostate>(material->emission())) {
                    properties += "e";
                }

                std::string materialName = (material->name() ? "\"" + *(material->name()) + "\"" : "null");
                ss << padding << "\t\t[MATERIAL] (" << static_cast<const void*>(material.get()) << ", "
                   << materialName << ", " << properties << ")" << endl;
            }
        }

        auto light = n.light();
        if (light) {
            std::string lightName = (light->name() ? "\"" + *(light->name()) + "\"" : "null");
            ss << padding << "\t[LIGHT] (" << static_cast<const void*>(light.get()) << ", " << lightName << ")"
               << endl;
        }

        auto camera = n.camera();
        if (camera) {
            std::string cameraName = (camera->name() ? "\"" + *(camera->name()) + "\"" : "null");
            ss << padding << "\t[CAMERA] (" << static_cast<const void*>(camera.get()) << ", " << cameraName
               << ")" << endl;
        }

        for (auto& c : n.children(false)) {
            TreeStringRec(*c, ss, depth + 1);
        }
    }

} // namespace

} // namespace a3d::util::string
