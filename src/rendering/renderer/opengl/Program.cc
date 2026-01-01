//
//  Program.cc
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/renderer/opengl/Program.h"

#include <format>

#ifdef A3D_GL_ES
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <magic_enum/magic_enum.hpp>

#include "a3d/Utilities.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/log/Log.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Internal Static Member Functions ///

Program& Program::Default() {
	static auto program = Program("default");
	return program;
}

Program& Program::Skybox() {
	static auto program = Program("skybox");
	return program;
}

Program& Program::Wireframe() {
	static auto program = Program("wireframe");
	return program;
}

Program& Program::Lines() {
	static auto program = Program("lines");
	return program;
}

Program& Program::GroundPlane() {
	static auto program = Program("ground_plane");
	return program;
}

/// Internal Lifecycle Functions ///

Program::Program(const string& name):
		_name{name},
		_glID{0},
		_isLinked{false},
		_logString{},
		_vertexShaderSource{},
		_fragmentShaderSource{}
		/*_uniformLocationCache{} */{

		_glID = glCreateProgram();

		if (_glID == 0) {
			throw Exception("Unable to create shader program.");
		}
		else {
			auto vsSource = Program::shaderSource(name, "vert");
			auto fsSource = Program::shaderSource(name, "frag");
			
			if (vsSource && fsSource) {
				_vertexShaderSource = *vsSource;
				_fragmentShaderSource = *fsSource;
				
				prepare();
			}
			else {
				throw Exception("Couldn't load shader sources.");
			}
		}
}

Program::~Program() {
	
}

/// Internal Member Functions ///

bool Program::compile() {
	if (!compile(_vertexShaderSource, ShaderType::Vertex)) return false;
	if (!compile(_fragmentShaderSource, ShaderType::Fragment)) return false;
	return true;
}

bool Program::link() {

	if (isLinked()) return true;
	if (_glID <= 0) return false;

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
			log::e()("Failed to link program '{}':\n{}",
					  name(), *_logString);
		}

		glDeleteProgram(_glID);

		return false;
	}
	else {
		isLinked(true);
		log::i()("Done.");
		return true;
	}
}

bool Program::validate() {
	if (!isLinked()) return false;
	
	GLint status = 0;
	glValidateProgram(_glID);
	glGetProgramiv(_glID, GL_VALIDATE_STATUS, &status);

	if (status != GL_NO_ERROR) {
		// Store log and return false
		GLint length = 0;
		_logString = nullopt;
		
		glGetProgramiv(_glID, GL_INFO_LOG_LENGTH, &length);
		
		if (length > 0) {
			auto c_log = (GLchar*)new char[length];
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

void Program::use() {
	
	if (_glID <= 0 || (!_isLinked)) {
		log::e()("Program '{}' not ready.", _name);
	}
	else {
		glUseProgram(_glID);
	}
}

void Program::unuse() {
	glUseProgram(0);
}

void Program::bindAttributeLocation(GLuint location, const char* name) {
	glBindAttribLocation(_glID, location, name);
}

void Program::setUniform(const char* name, float x, float y, float z) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform3f(loc, x, y, z);
	}
	else {
		log::e()("Uniform '{}' not found.", name);
	}
}

void Program::setUniform(const char* name, const vec2& v) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform2f(loc, v.x, v.y);
	}
	else {
		log::e()("Uniform '{}' not found.", name);
	}
}

void Program::setUniform(const char* name, const vec3& v) {
	
	setUniform(name, v.x, v.y, v.z);
}

void Program::setUniform(const char* name, const vec4& v) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform4f(loc, v.x, v.y, v.z, v.w);
	}
	else {
		log::e()("Uniform '{}' not found.", name);
	}
}

void Program::setUniform(const char* name, const mat3& m) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniformMatrix3fv(loc, 1, GL_FALSE, value_ptr(m));
	}
	else {
		log::e()("Uniform '{}' not found.", name);
	}
}

void Program::setUniform(const char* name, const mat4& m) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(m));
	}
	else {
		log::e()("Uniform '{}' not found.", name);
	}
}

void Program::setUniform(const char* name, bool val) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform1i(loc, val);
	}
	else {
		log::e()("Uniform '{}' not found.", name);
	}
}

void Program::setUniform(const char* name, int val) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform1i(loc, val);
	}
	else {
		log::e()("Uniform '{}' not found.", name);
	}
}

void Program::setUniform(const char* name, unsigned val) {

	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform1ui(loc, val);
	}
	else {
		log::e()("Uniform '{}' not found.", name);
	}
}

void Program::setUniform(const char* name, float val) {

	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform1f(loc, val);
	}
	else {
		log::e()("Uniform '{}' not found.", name);
	}
}

//void Program::bindUniformBlock(const char* name, GLuint location) {
//
//	GLint blockIndex = glGetUniformBlockIndex(_glID, name);
//	if (blockIndex != GL_INVALID_INDEX) {
//		glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, location);
//	}
//	else {
//		log::e()("Uniform block '{}' not found.", name);
//	}
//}

void Program::setUniformBlockBinding(const char* blockName, GLuint bindingPoint) {
	GLuint blockIndex = glGetUniformBlockIndex(_glID, blockName);
	if (blockIndex == GL_INVALID_INDEX) {
		log::e()("Uniform block '{}' not found.", blockName);
		return;
	}
	glUniformBlockBinding(_glID, blockIndex, bindingPoint);
}

void Program::bindTexture(const char* name,
						  const int& target,
						  const unsigned& slot,
						  const unsigned& textureID,
						  unsigned index) {
	
	glActiveTexture(slot);
	glBindTexture(target, textureID);
	setUniform(name, (int)index);
}

unsigned Program::getAttributeLocation(const char* name) const {
	
	return glGetAttribLocation(_glID, name);
}

const string& Program::name() const {
	return _name;
}

GLuint Program::glID() {
	return _glID;
}

bool Program::isLinked() const {
	return _isLinked;
}

/// Private Member Functions ///

optional<string> Program::shaderSource(const string& name, const string& type) {

	auto source = utils::ShaderSource(name, type);

	if (source) {


#ifdef A3D_GL_ES
		// replace dekstop GLSL header string with ES version string
		static const string ES_HEADER = "#version 300 es\n\nprecision mediump int;\nprecision mediump float;";

		auto replaced = *source;
		// TODO: sync this up with a new GL version constant also used in Window::Window()
		StringReplace(replaced, "#version 410", ES_HEADER);
		return replaced;
#else
		return  *source;
#endif
	}

	return nullopt;
}

void Program::prepare() {
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
				throw Exception(std::format("Failed linking program '{}'.", _name));
			}
		}
		else {
			//A3D_LOG_C("Failed compiling '{}' shaders:\n{}", _name, *_logString);
			//A3D_LOG_C("Failed compiling '{}' shaders.", _name);
			throw Exception(std::format("Failed compiling '{}' shaders.", _name));
		}
	}
}

bool Program::compile(const string& source, ShaderType type) {

	log::d()("Compiling {} shader for program '{}'...",
			 magic_enum::enum_name(type), name());

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
			log::e()("Failed to compile {} shader program '{}':\n{}",
					  magic_enum::enum_name(type), name(), *_logString);
		}

		glDeleteShader(shaderID);

		return false;
	}
	else {
		glAttachShader(_glID, shaderID);

		log::i()("{} shader {}' compiled.", magic_enum::enum_name(type), name());

		return true;
	}
}

int Program::getUniformLocation(const char* name) {

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

void Program::glID(GLuint glID) {
	_glID = glID;
}

void Program::isLinked(bool isLinked) {
	_isLinked = isLinked;
}
