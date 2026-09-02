//
//  GLSLProgram.cc
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/GLSLProgram.h"

#include <format>
#include <stdexcept>

#include "a3d/render/backend/opengl/gl.h" // MOVE?

#include "a3d/log/Log.h"
#include "a3d/render/backend/opengl/GLSLPreprocessor.h"
#include "a3d/util/Enum.h"
#include "a3d/util/Filesystem.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

// [Private Non-Member Prototypes]

static optional<string> ShaderSourceAt(const string& name, ShaderType type);
static optional<string> ShaderIncludeSourceAt(const filesystem::path& filename);

namespace a3d {

// [Internal Lifecycle Functions]

GLSLProgram::GLSLProgram(const string& name):
    _name {name},
    _glID {0},
    _isLinked {false},
    _logString {},
    _vertexShaderSource {},
    _fragmentShaderSource {} {

    _glID = glCreateProgram();

    if (_glID == 0) {
        throw runtime_error("Unable to create shader program.");
    }

    try {
        auto vsSource = shaderSource(name, ShaderType::Vertex);
        auto fsSource = shaderSource(name, ShaderType::Fragment);

        if (!vsSource || !fsSource) {
            throw runtime_error("Couldn't load shader sources.");
        }

        _vertexShaderSource = *vsSource;
        _fragmentShaderSource = *fsSource;

        prepare();
    }
    catch (...) {

        if (_glID != 0) {
            glDeleteProgram(_glID);
            _glID = 0;
        }

        throw;
    }
}

GLSLProgram::~GLSLProgram() {

    if (_glID != 0) {
        glDeleteProgram(_glID);
        _glID = 0;
    }
}

// [Internal Member Functions]

bool GLSLProgram::compile() {
    if (!compile(_vertexShaderSource, ShaderType::Vertex)) {
        return false;
    }
    if (!compile(_fragmentShaderSource, ShaderType::Fragment)) {
        return false;
    }
    return true;
}

bool GLSLProgram::link() {

    if (isLinked()) {
        return true;
    }
    if (_glID <= 0) {
        return false;
    }

    log::i()("Linking program '{}'...", _name);

    glLinkProgram(_glID);

    GLint linkSucceeded = 0;
    glGetProgramiv(_glID, GL_LINK_STATUS, &linkSucceeded);

    if (linkSucceeded == GL_FALSE) {
        GLint logSize = 0;
        _logString = nullopt;

        glGetProgramiv(_glID, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            // win11
            vector<GLchar> c_log(logSize);
            glGetProgramInfoLog(_glID, logSize, nullptr, c_log.data());
            _logString = string(c_log.data());
            log::e()("Failed to link program '{}':\n{}", name(), *_logString);
        }

        glDeleteProgram(_glID);
        _glID = 0;

        return false;
    }
    else {
        isLinked(true);
        log::i()("Done.");
        return true;
    }
}

bool GLSLProgram::validate() {
    if (!isLinked()) {
        return false;
    }

    GLint status = 0;
    glValidateProgram(_glID);
    glGetProgramiv(_glID, GL_VALIDATE_STATUS, &status);

    if (status != GL_NO_ERROR) {
        // Store log and return false
        GLint length = 0;
        _logString = nullopt;

        glGetProgramiv(_glID, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            auto  c_log = (GLchar*) new char[length];
            GLint written = 0;
            glGetProgramInfoLog(_glID, length, &written, c_log);
            _logString = string(c_log);
            //log::e()("Validate log:\n{}", c_log);
            delete[] c_log;
        }

        return false;
    }
    else {
        return true;
    }
}

void GLSLProgram::use() {

    if (_glID <= 0 || (!_isLinked)) {
        throw logic_error(std::format("Program '{}' not ready.", _name));
    }

    glUseProgram(_glID);
}

void GLSLProgram::unuse() {
    glUseProgram(0);
}

void GLSLProgram::bindAttributeLocation(GLuint location, const char* name) {
    glBindAttribLocation(_glID, location, name);
}

void GLSLProgram::setUniform(const char* name, float x, float y, float z) {

    int loc = getUniformLocation(name);
    if (loc >= 0) {
        glUniform3f(loc, x, y, z);
    }
    else {
        log::e()("Uniform '{}' not found.", name);
    }
}

void GLSLProgram::setUniform(const char* name, const vec2& v) {

    int loc = getUniformLocation(name);
    if (loc >= 0) {
        glUniform2f(loc, v.x, v.y);
    }
    else {
        log::e()("Uniform '{}' not found.", name);
    }
}

void GLSLProgram::setUniform(const char* name, const vec3& v) {

    setUniform(name, v.x, v.y, v.z);
}

void GLSLProgram::setUniform(const char* name, const vec4& v) {

    int loc = getUniformLocation(name);
    if (loc >= 0) {
        glUniform4f(loc, v.x, v.y, v.z, v.w);
    }
    else {
        log::e()("Uniform '{}' not found.", name);
    }
}

void GLSLProgram::setUniform(const char* name, const mat3& m) {

    int loc = getUniformLocation(name);
    if (loc >= 0) {
        glUniformMatrix3fv(loc, 1, GL_FALSE, value_ptr(m));
    }
    else {
        log::e()("Uniform '{}' not found.", name);
    }
}

void GLSLProgram::setUniform(const char* name, const mat4& m) {

    int loc = getUniformLocation(name);
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(m));
    }
    else {
        log::e()("Uniform '{}' not found.", name);
    }
}

void GLSLProgram::setUniform(const char* name, bool val) {

    int loc = getUniformLocation(name);
    if (loc >= 0) {
        glUniform1i(loc, val);
    }
    else {
        log::e()("Uniform '{}' not found.", name);
    }
}

void GLSLProgram::setUniform(const char* name, int val) {

    int loc = getUniformLocation(name);
    if (loc >= 0) {
        glUniform1i(loc, val);
    }
    else {
        log::e()("Uniform '{}' not found.", name);
    }
}

void GLSLProgram::setUniform(const char* name, unsigned val) {

    int loc = getUniformLocation(name);
    if (loc >= 0) {
        glUniform1ui(loc, val);
    }
    else {
        log::e()("Uniform '{}' not found.", name);
    }
}

void GLSLProgram::setUniform(const char* name, float val) {

    int loc = getUniformLocation(name);
    if (loc >= 0) {
        glUniform1f(loc, val);
    }
    else {
        log::e()("Uniform '{}' not found.", name);
    }
}

void GLSLProgram::setUniformBlockBinding(const char* blockName, GLuint bindingPoint) {
    GLuint blockIndex = glGetUniformBlockIndex(_glID, blockName);
    if (blockIndex == GL_INVALID_INDEX) {
        log::e()("Uniform block '{}' not found.", blockName);
        return;
    }
    glUniformBlockBinding(_glID, blockIndex, bindingPoint);
}

void GLSLProgram::bindTexture(const char*     name,
                              const int&      target,
                              const unsigned& slot,
                              const unsigned& textureID,
                              unsigned        index) {

    glActiveTexture(slot);
    glBindTexture(target, textureID);
    setUniform(name, (int) index);
}

unsigned GLSLProgram::getAttributeLocation(const char* name) const {

    return glGetAttribLocation(_glID, name);
}

const string& GLSLProgram::name() const {
    return _name;
}

gl::uint_t GLSLProgram::glID() {
    return _glID;
}

bool GLSLProgram::isLinked() const {
    return _isLinked;
}

// [Private Member Functions]

optional<string> GLSLProgram::shaderSource(const string& name, ShaderType type) {

    auto source = ShaderSourceAt(name, type);

    if (!source) {
        return nullopt;
    }

    string sourceName;
    switch (type) {
        case ShaderType::Vertex:
            sourceName = name + ".vert";
            break;
        case ShaderType::Fragment:
            sourceName = name + ".frag";
            break;
    }

    return GLSLPreprocessor::Process(*source, sourceName, ShaderIncludeSourceAt);
}

void GLSLProgram::prepare() {
    log::t();

    if (!_isLinked) {
        if (compile()) {
            log::i()("Shaders for program '{}' compiled.", _name);

            if (link()) {
                // _uniformLocationCache = map<string, int>();
                log::i()("Program '{}' linked.", _name);
            }
            else {
                //A3D_LOG_C("Failed linking '{}' program:\n{}", _name, *_logString);
                //A3D_LOG_C("Failed linking program '{}'.", _name);
                throw std::runtime_error(std::format("Failed linking program '{}'.", _name));
            }
        }
        else {
            //A3D_LOG_C("Failed compiling '{}' shaders:\n{}", _name, *_logString);
            //A3D_LOG_C("Failed compiling '{}' shaders.", _name);
            throw std::runtime_error(std::format("Failed compiling '{}' shaders.", _name));
        }
    }
}

bool GLSLProgram::compile(const string& source, ShaderType type) {

    log::d()("Compiling {} shader for program '{}'...", util::enums::enum_name(type), name());

    GLuint shaderID = 0;

    switch (type) {
        case ShaderType::Vertex:
            shaderID = glCreateShader(GL_VERTEX_SHADER);
            break;
        case ShaderType::Fragment:
            shaderID = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        default:
            return false;
    }

    const char* c_source = source.c_str();
    glShaderSource(shaderID, 1, &c_source, NULL);
    glCompileShader(shaderID);

    // https://www.khronos.org/opengl/wiki/Shader_Compilation#Shader_error_handling
    GLint compileSucceeded = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileSucceeded); // error = 1 (GL_TRUE = 1)
    if (compileSucceeded == GL_FALSE) {
        GLint logSize = 0;
        _logString = nullopt;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
        // win11
            vector<GLchar> c_log(logSize);
            glGetShaderInfoLog(shaderID, logSize, nullptr, c_log.data());
            _logString = string(c_log.data());
            log::e()("Failed to compile {} shader program '{}':\n{}", util::enums::enum_name(type), name(),
                     *_logString);
        }

        glDeleteShader(shaderID);

        return false;
    }
    else {
        glAttachShader(_glID, shaderID);
        glDeleteShader(shaderID);

        log::i()("{} shader {}' compiled.", util::enums::enum_name(type), name());

        return true;
    }
}

int GLSLProgram::getUniformLocation(const char* name) {

    GLint location = -1;
    // if (_uniformLocationCache.find(name) == _uniformLocationCache.end()) {
    location = glGetUniformLocation(_glID, name);

    if (location < 0) {
        log::e()("Could not find uniform location: {}", name);
    }
        // _uniformLocationCache[name] = location;
    // }
    // else {
    // 	location = _uniformLocationCache[name];
    // }
    return location;
}

void GLSLProgram::glID(GLuint glID) {
    _glID = glID;
}

void GLSLProgram::isLinked(bool isLinked) {
    _isLinked = isLinked;
}

} // namespace a3d

// [Private Non-Member Functions]

optional<string> ShaderSourceAt(const string& name, ShaderType type) {

    const auto extension = type == ShaderType::Vertex ? ".vert" : ".frag";
    return util::fs::TextAt(filesystem::path("shaders") / (name + extension));
}

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
