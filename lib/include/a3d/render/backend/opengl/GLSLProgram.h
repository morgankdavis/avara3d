//
//  GLSLProgram.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//
//  Based largely on code from "OpenGL 4 Shading Language Cookbook" by David Wolff
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_GLSLPROGRAM_H
#define AVARA3D_RENDER_BACKEND_OPENGL_GLSLPROGRAM_H

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "a3d/Math.h"
#include "a3d/render/backend/ShaderType.h"
#include "a3d/render/backend/opengl/GLTypes.h"

namespace a3d {

class GLSLProgram {

public:
    // [Internal Lifecycle Functions]

    explicit GLSLProgram(const std::string& name);

    GLSLProgram(const GLSLProgram&)            = delete;
    GLSLProgram& operator=(const GLSLProgram&) = delete;

    GLSLProgram(GLSLProgram&&)            = delete;
    GLSLProgram& operator=(GLSLProgram&&) = delete;

    ~GLSLProgram();

    // [Internal Member Functions]

    bool               compile();
    bool               link();
    bool               validate();
    void               use();
    void               unuse();

    void               bindAttributeLocation(unsigned location, const char* name);

    void               setUniform(const char* name, float x, float y, float z);
    void               setUniform(const char* name, const math::vec2& v);
    void               setUniform(const char* name, const math::vec3& v);
    void               setUniform(const char* name, const math::vec4& v);
    void               setUniform(const char* name, const math::mat3& m);
    void               setUniform(const char* name, const math::mat4& m);
    void               setUniform(const char* name, bool val);
    void               setUniform(const char* name, int val);
    void               setUniform(const char* name, unsigned val);
    void               setUniform(const char* name, float val);

//		void 						bindUniformBlock(const char* name,
//														 unsigned location);
    void               setUniformBlockBinding(const char* blockName, unsigned bindingPoint);

    void               bindTexture(const char*     name,
                                   const int&      target,
                                   const unsigned& slot,
                                   const unsigned& textureID,
                                   unsigned        index);

    unsigned           getAttributeLocation(const char* name) const;

    const std::string& name() const;
    gl::uint_t         glID();
    bool               isLinked() const;

private:
    // [Private Member Functions]

    std::optional<std::string> shaderSource(const std::string& name, ShaderType type);
    void                       prepare();
    bool                       compile(const std::string& source, ShaderType type);
    int                        getUniformLocation(const char* name);
    void                       glID(unsigned glID);
    void                       isLinked(bool isLinked);

    // [Private Member Variables]

    std::string                _name;
    unsigned                   _glID;
    bool                       _isLinked;
    std::optional<std::string> _logString;
    std::string                _vertexShaderSource;
    std::string                _fragmentShaderSource;
};

}

#endif // AVARA3D_RENDER_BACKEND_OPENGL_GLSLPROGRAM_H
