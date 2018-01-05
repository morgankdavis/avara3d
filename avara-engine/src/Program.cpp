//
//  Program.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Program.h"

#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace boost;
using namespace glm;
using namespace std;


/*******************************************************************************
     MARK:   Lifecycle
 ******************************************************************************/

Program::Program(const string& name):
	m_name(name),
	m_glID(0),
	m_isLinked(false) {
		
		m_logString = {};
		m_vertexShaderSource = {};
		m_fragmentShaderSource = {};
		
		m_glID = glCreateProgram();
		
		if (m_glID == 0) {
			logString(string("Unable to create shader program."));
		}
		else {
			
			string vsPath = ShaderPath(name, "vert");
			string fsPath = ShaderPath(name, "frag");
			
			auto vsSource = LoadTextFile(vsPath);
			auto fsSource = LoadTextFile(fsPath);
			
	//		cout << "vs: " << *vs << endl;
	//		cout << "fs: " << *fs << endl;
			
			if (vsSource && fsSource) {
				vertexShaderSource(vsSource);
				fragmentShaderSource(fsSource);
			}
			else {
				cout << "Couldn't load shader files." << endl;
			}
		}
}

/*******************************************************************************
     MARK:   Public
 ******************************************************************************/

optional<string> Program::logString() const {
	return m_logString;
}

/*******************************************************************************
     MARK:   Internal
 ******************************************************************************/

bool Program::compile() {
	
	if (vertexShaderSource()) {
		if (!compileShaderFromString(*vertexShaderSource(), ShaderType_Vertex)) return false;
	}
	
	if (fragmentShaderSource()) {
		if (!compileShaderFromString(*fragmentShaderSource(), ShaderType_Fragment)) return false;
	}
	
	return true;
}

bool Program::link() {
	
	if (isLinked()) return true;
	if (m_glID <= 0) return false;
	
	glLinkProgram(m_glID);
	
	int status = 0;
	glGetProgramiv(m_glID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		int length = 0;
		logString({});
		
		glGetProgramiv(m_glID, GL_INFO_LOG_LENGTH, &length);
		
		if (length > 0) {
			// TODO: put on stack
			char* c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog(m_glID, length, &written, c_log);
			logString(string(c_log));
			delete[] c_log;
		}
		
		return false;
	}
	else {
		isLinked(true);
		return true;
	}
}

bool Program::validate() {
	if (!isLinked()) return false;
	
	GLint status;
	glValidateProgram(m_glID);
	glGetProgramiv(m_glID, GL_VALIDATE_STATUS, &status);
	
	if (status == GL_FALSE) {
		// Store log and return false
		int length = 0;
		logString({});
		
		glGetProgramiv(m_glID, GL_INFO_LOG_LENGTH, &length);
		
		if (length > 0) {
			char * c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog(m_glID, length, &written, c_log);
			logString(string(c_log));
			delete[] c_log;
		}
		
		return false;
	}
	else {
		return true;
	}
}

void Program::use() {
	
	if (m_glID <= 0 || (!m_isLinked)) {
		cout << "*** Program NOT ready. ***" << endl;
	}
	else {
		glUseProgram(m_glID);
	}
}

void Program::bindAttribLocation(GLuint location, const char* name) {
	glBindAttribLocation(m_glID, location, name);
}

void Program::bindFragDataLocation(GLuint location, const char* name) {
	glBindFragDataLocation(m_glID, location, name);
}

void Program::setUniform(const char* name, float x, float y, float z) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform3f(loc, x, y, z);
	}
	else {
		printf("Uniform: %s not found.\n", name);
	}
}

void Program::setUniform(const char* name, const vec2& v) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform2f(loc, v.x, v.y);
	}
	else {
		printf("Uniform: %s not found.\n", name);
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
		printf("Uniform: %s not found.\n", name);
	}
}

void Program::setUniform(const char* name, const mat3& m) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniformMatrix3fv(loc, 1, GL_FALSE, value_ptr(m));
	}
	else {
		printf("Uniform: %s not found.\n", name);
	}
}

void Program::setUniform(const char* name, const mat4& m) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(m));
	}
	else {
		printf("Uniform: %s not found.\n", name);
	}
}

void Program::setUniform(const char* name, bool val) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform1i(loc, val);
	}
	else {
		printf("Uniform: %s not found.\n", name);
	}
}

void Program::setUniform(const char* name, int val) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform1i(loc, val);
	}
	else {
		printf("Uniform: %s not found.\n", name);
	}
}

void Program::setUniform(const char* name, float val) {
	
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform1f(loc, val);
	}
	else {
		printf("Uniform: %s not found.\n", name);
	}
}

void Program::bindUniformBlock(const char* name, GLuint location) {
	
    // TODO: TEMPORARY
    static GLint blockIndex = -1;
    if (blockIndex == -1) {
        cout << "bind" << endl;
        blockIndex = glGetUniformBlockIndex(m_glID, name);
    }
	glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, location);
}

void Program::bindTexture(const char* name, const GLenum& slot, const GLuint& textureID, GLint index) {
	
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, textureID);
	setUniform(name, index);
}

GLint Program::getAttributeLocation(const char* name) const {
	
	return glGetAttribLocation(m_glID, name);
}

void Program::printActiveUniforms() const {
	
	GLint nUniforms, size, location, maxLen;
	GLchar* name;
	GLsizei written;
	GLenum type;
	
	glGetProgramiv(m_glID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
	glGetProgramiv(m_glID, GL_ACTIVE_UNIFORMS, &nUniforms);
	
	name = (GLchar*)malloc(maxLen);
	
	printf(" Location | Name\n");
	printf("------------------------------------------------\n");
	for (int i=0 ; i<nUniforms ; ++i) {
		glGetActiveUniform(m_glID, i, maxLen, &written, &size, &type, name);
		location = glGetUniformLocation(m_glID, name);
		printf(" %-8d | %s\n" ,location, name);
	}
	
	free(name);
}

void Program::printActiveAttribs() const {
	
	GLint written, size, location, maxLength, nAttribs;
	GLenum type;
	GLchar* name;
	
	glGetProgramiv(m_glID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	glGetProgramiv(m_glID, GL_ACTIVE_ATTRIBUTES, &nAttribs);
	
	name = (GLchar*)malloc(maxLength);
	
	printf(" Index | Name\n");
	printf("------------------------------------------------\n");
	for (int i=0 ; i<nAttribs ; i++) {
		glGetActiveAttrib(m_glID, i, maxLength, &written, &size, &type, name);
		location = glGetAttribLocation(m_glID, name);
		printf(" %-5d | %s\n", location, name);
	}
	
	free(name);
}

std::string Program::name() const {
	return m_name;
}

GLuint Program::glID() {
	return m_glID;
}

bool Program::isLinked() const {
	return m_isLinked;
}

optional<string> Program::vertexShaderSource() const {
	return m_vertexShaderSource;
}

void Program::vertexShaderSource(optional<string> source) {
	m_vertexShaderSource = source;
}

optional<string> Program::fragmentShaderSource() const {
	return m_fragmentShaderSource;
}

void Program::fragmentShaderSource(optional<string> source) {
	m_fragmentShaderSource = source;
}

/*******************************************************************************
     MARK:   Private
 ******************************************************************************/

bool Program::compileShaderFromString(const string& source, ShaderType type) {
	
//	if (m_glID <= 0) {
//		glID(glCreateProgram());
//		if (m_glID == 0) {
//			logString(string("Unable to create shader program."));
//			return false;
//		}
//	}
	
	GLuint shaderID = 0;
	
	switch (type) {
		case ShaderType_Vertex:
			shaderID = glCreateShader(GL_VERTEX_SHADER);
			break;
		case ShaderType_Fragment:
			shaderID = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		default:
			return false;
	}
	
	const char *c_code = source.c_str();
	glShaderSource(shaderID, 1, &c_code, NULL);
	
	glCompileShader(shaderID);
	
	int result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		int length = 0;
		logString({});
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		if (length > 0) {
			// TODO: put on stack
			char* c_log = new char[length];
			int written = 0;
			glGetShaderInfoLog(shaderID, length, &written, c_log);
			logString(string(c_log));
			delete[] c_log;
		}
		
		return false;
	}
	else {
		glAttachShader(m_glID, shaderID);
		
		return true;
	}
}

GLint Program::getUniformLocation(const char* name) {
	return glGetUniformLocation(m_glID, name);
}

void Program::glID(GLuint glID) {
	m_glID = glID;
}

void Program::isLinked(bool isLinked) {
	m_isLinked = isLinked;
}

void Program::logString(optional<string> logString) {
	m_logString = logString;
}
