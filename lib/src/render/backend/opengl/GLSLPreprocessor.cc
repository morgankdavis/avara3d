//
//  GLSLPreprocessor.cc
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/GLSLPreprocessor.h"

#include <algorithm>
#include <filesystem>
#include <format>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "a3d/util/Filesystem.h"

using namespace a3d;
using namespace std;

/// Private Static Non-Member Constants ///

static constexpr string_view HeaderDirective = "#header";
static constexpr string_view IncludeDirective = "#include";

/// Private Static Non-Member Prototypes ///

static size_t           SkipUtf8Bom(const string& source);
static string           PlatformShaderHeader();
static string_view      Trim(string_view value);
static optional<string> ParseIncludeDirective(string_view line, const string& sourceName, size_t lineNumber);
static string           ExpandIncludes(const string&                            source,
                                       const string&                            sourceName,
                                       const GLSLPreprocessor::IncludeResolver& includeResolver,
                                       vector<string>&                          includeStack);
static string           IncludeCycleMessage(const vector<string>& includeStack, const string& includeName);

/// Internal Static Functions ///

string GLSLPreprocessor::Process(const string&          source,
                                 const string&          sourceName,
                                 const IncludeResolver& includeResolver) {

    const size_t sourceStart = SkipUtf8Bom(source);

    const size_t lineEnd = source.find('\n', sourceStart);

    const string_view firstLine {source.data() + sourceStart,
                                 lineEnd == string::npos ? source.size() - sourceStart : lineEnd - sourceStart};

    if (Trim(firstLine) != HeaderDirective) {
        throw runtime_error(format("Shader '{}' must begin with '{}'.", sourceName, HeaderDirective));
    }

    string output = PlatformShaderHeader();

    if (lineEnd == string::npos) {
        return output;
    }

    vector<string> includeStack {sourceName};

    output += ExpandIncludes(source.substr(lineEnd + 1), sourceName, includeResolver, includeStack);

    return output;
}

/// Private Non-Member Functions ///

optional<string> ShaderIncludeSourceAt(const filesystem::path& filename) {

    if (filename.empty() || filename.has_root_path()) {
        return nullopt;
    }

    for (const auto& component : filename) {
        if (component == "..") {
            return nullopt;
        }
    }

    return util::fs::TextAt(filesystem::path("shaders") / "include" / filename);
}

size_t SkipUtf8Bom(const std::string& source) {

    if (source.size() >= 3 && static_cast<unsigned char>(source[0]) == 0xEF
        && static_cast<unsigned char>(source[1]) == 0xBB && static_cast<unsigned char>(source[2]) == 0xBF) {
        return 3;
    }
    return 0;
}

std::string PlatformShaderHeader() {

#if defined(A3D_GL_WEB) || defined(A3D_GL_ES)
    return "#version 300 es\n"
           "#define A3D_GLSL_ES 1\n"
           "precision highp float;\n"
           "precision highp int;\n"
           "#line 2\n";
#else
    return "#version 330 core\n"
           "#define A3D_GLSL_DESKTOP 1\n"
           "#line 2\n";
#endif
}

string_view Trim(string_view value) {

    constexpr string_view whitespace = " \t\r";

    const auto begin = value.find_first_not_of(whitespace);

    if (begin == string_view::npos) {
        return {};
    }

    const auto end = value.find_last_not_of(whitespace);

    return value.substr(begin, end - begin + 1);
}

optional<string> ParseIncludeDirective(string_view line, const string& sourceName, size_t lineNumber) {

    line = Trim(line);

    if (!line.starts_with(IncludeDirective)) {
        return nullopt;
    }

    if (line.size() > IncludeDirective.size() && line[IncludeDirective.size()] != ' '
        && line[IncludeDirective.size()] != '\t') {

        return nullopt;
    }

    auto remainder = Trim(line.substr(IncludeDirective.size()));

    if (remainder.empty() || remainder.front() != '"') {

        throw runtime_error(format("Malformed shader include directive in '{}' at line {}.", sourceName,
                                   lineNumber));
    }

    const auto closingQuote = remainder.find('"', 1);

    if (closingQuote == string_view::npos) {
        throw runtime_error(format("Malformed shader include directive in '{}' at line {}.", sourceName,
                                   lineNumber));
    }

    const auto includeName = remainder.substr(1, closingQuote - 1);

    if (includeName.empty()) {
        throw runtime_error(format("Empty shader include in '{}' at line {}.", sourceName, lineNumber));
    }

    const auto trailing = Trim(remainder.substr(closingQuote + 1));

    if (!trailing.empty()) {
        throw runtime_error(format("Unexpected text after shader include in '{}' at line {}.", sourceName,
                                   lineNumber));
    }

    return string(includeName);
}

string IncludeCycleMessage(const vector<string>& includeStack, const string& includeName) {

    string message = "Shader include cycle detected: ";

    for (const auto& entry : includeStack) {
        message += entry;
        message += " -> ";
    }

    message += includeName;

    return message;
}

string ExpandIncludes(const string&                            source,
                      const string&                            sourceName,
                      const GLSLPreprocessor::IncludeResolver& includeResolver,
                      vector<string>&                          includeStack) {

    istringstream stream {source.substr(SkipUtf8Bom(source))};

    string output;
    string line;
    size_t lineNumber = 0;

    while (getline(stream, line)) {

        ++lineNumber;

        const auto trimmed = Trim(line);

        if (trimmed == HeaderDirective) {
            throw runtime_error(format("Shader '#header' directive is only valid at the beginning "
                                       "of a top-level shader; found in '{}' at line {}.",
                                       sourceName, lineNumber));
        }

        const auto includeName = ParseIncludeDirective(line, sourceName, lineNumber);

        if (!includeName) {
            output += line;
            output += '\n';
            continue;
        }

        if (!includeResolver) {
            throw runtime_error(format("Shader '{}' includes '{}', but no include resolver is available.",
                                       sourceName, *includeName));
        }

        if (find(includeStack.begin(), includeStack.end(), *includeName) != includeStack.end()) {

            throw runtime_error(IncludeCycleMessage(includeStack, *includeName));
        }

        const auto includeSource = includeResolver(*includeName);

        if (!includeSource) {
            throw runtime_error(format("Shader include '{}' referenced by '{}' at line {} was not found.",
                                       *includeName, sourceName, lineNumber));
        }

        includeStack.push_back(*includeName);

        output += ExpandIncludes(*includeSource, *includeName, includeResolver, includeStack);

        includeStack.pop_back();
    }

    return output;
}
