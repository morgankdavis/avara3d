//
//  Program.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Program.h"

//#include <cstring>
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

Program::Program(const std::string& vertexShaderPath,
				 const std::string& fragmentShaderPath):
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
		auto vs = LoadTextFile(vertexShaderPath);
		auto fs = LoadTextFile(fragmentShaderPath);
		
		cout << "vs: " << *vs << endl;
		cout << "fs: " << *fs << endl;
		
		if (vs && fs) {
			vertexShaderSource(vs);
			fragmentShaderSource(fs);
		}
		else {
			cout << "Couldn't load shader files." << endl;
		}
	}
}

/*******************************************************************************
     MARK:   Public
 ******************************************************************************/

optional<std::string> Program::logString() {
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
		cout << "Program not ready." << endl;
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

GLint Program::getAttributeLocation(const char* name) {
	
	return glGetAttribLocation(m_glID, name);
}

void Program::printActiveUniforms() {
	
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

void Program::printActiveAttribs() {
	
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

GLuint Program::glID() {
	return m_glID;
}

bool Program::isLinked() {
	return m_isLinked;
}

optional<std::string> Program::vertexShaderSource() {
	return m_vertexShaderSource;
}

void Program::vertexShaderSource(optional<std::string> source) {
	m_vertexShaderSource = source;
}

optional<std::string> Program::fragmentShaderSource() {
	return m_fragmentShaderSource;
}

void Program::fragmentShaderSource(optional<std::string> source) {
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




////
////  Program.cpp
////	avara-engine
////
////  Created by Morgan Davis on 12/23/16.
////  Copyright © 2016 Morgan K Davis. All rights reserved.
////
//
//#include "Program.h"
//
//#include <cstring>
//#include <iostream>
//
//#include <GL/glew.h>
//
//#include "Utilities.h"
//
//
//using namespace ae;
//using namespace std;
//
//
///*******************************************************************************
//     MARK:   Constants
// ******************************************************************************/
//
//#define MAX_LOG_SIZE	2048
//
///*******************************************************************************
//     MARK:   Lifecycle
// ******************************************************************************/
//
//Program::Program(const ProgramType type):
//m_glProgramID(0) {
//
//	string shaderName = "";
//
//	switch (type) {
//		case ProgramType_Default:
//			shaderName = "default";
//			break;
//		case ProgramType_PhongPlain:
//			shaderName = "phone_plain";
//			break;
//		case ProgramType_PhongTexture:
//			shaderName = "phong_texture"; // temporary
//			break;
//		case ProgramType_Fancy:
//			shaderName = "fancy"; // temporary
//			break;
//		default:
//			break;
//	}
//
//	//	auto vs = utils::loadTextFile(basePath + shaderName + ".vert");
//	//	auto fs = utils::loadTextFile(basePath + shaderName + ".frag");
//
//	auto vs = utils::ShaderSourceNamed(shaderName, "vert");
//	auto fs = utils::ShaderSourceNamed(shaderName, "frag");
//
//	if (vs && fs) {
//		m_vertexShader = *vs;
//		m_fragmentShader = *fs;
//
//		cout << "Shader program '" << shaderName << "' compiled." << endl;
//	}
//	else {
//		cout << "Couldn't load shaders." << endl;
//	}
//}
//
//Program::Program(const string& vertexShader, const string& fragmentShader):
//m_vertexShader(vertexShader),
//m_fragmentShader(fragmentShader),
//m_glProgramID(0) {
//
//}
//
///*******************************************************************************
//     MARK:   Public
// ******************************************************************************/
//
//bool Program::compile() {
//
//	// TODO: convert to exceptions
//
//	string vertexShaderSrc = vertexShader();
//	string fragmentShaderSrc = fragmentShader();
//
//	if (vertexShaderSrc.length() && fragmentShaderSrc.length()) {
//		const char *vertSourceCStr = vertexShaderSrc.c_str();
//		const char *fragSourceCStr = fragmentShaderSrc.c_str();
//
//		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
//		glShaderSource(vs, 1, &vertSourceCStr, NULL);
//		glCompileShader(vs);
//
//		char log[MAX_LOG_SIZE];
//		if (!checkShaderCompile(vs, log, MAX_LOG_SIZE)) {
//			cout << "Error compiling vertex shader: " << log << endl;
//			return false;
//		}
//
//		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
//		glShaderSource(fs, 1, &fragSourceCStr, NULL);
//		glCompileShader(fs);
//
//		if (!checkShaderCompile(fs, log, MAX_LOG_SIZE)) {
//			cout << "Error compiling fragment shader: " << log << endl;
//			return false;
//		}
//
//		GLuint prog = glCreateProgram();
//		glAttachShader(prog, fs);
//		glAttachShader(prog, vs);
//
//		glLinkProgram(prog);
//
//		if (!checkProgramLink(prog, log, MAX_LOG_SIZE)) {
//			cout << "Error linking program: " << log << endl;
//			return false;
//		}
//
//		m_glProgramID = prog;
//	}
//	else {
//		cout << "Missing vertex or fragment shader source." << endl;
//		return false;
//	}
//
//	return true;
//}
//
//string Program::info() const {
//	return "";
//}
//
//string Program::vertexShader() const {
//	return m_vertexShader;
//}
//
//void Program::vertexShader(const string source) {
//	m_vertexShader = source;
//}
//
//string Program::fragmentShader() const {
//	return m_fragmentShader;
//}
//
//void Program::fragmentShader(const string source) {
//	m_fragmentShader = source;
//}
//
///*******************************************************************************
//     MARK:   Internal
// ******************************************************************************/
//
//GLuint Program::glProgramID() const {
//	return m_glProgramID;
//}
//
////string Program::activeInputAttributes() const {
////	GLint numAttribs;
////	glGetProgramInterfaceiv((GLuint)m_glProgramID, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);
////
////	GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION};
////
////	for (int i=0; i<numAttribs; ++i) {
////		GLint results[3];
////		glGetProgramResourceiv(m_glProgramID, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);
////
////		GLint nameBufSize = results[0] + 1;
////		char name[nameBufSize];
////		glGetProgramResourceName(m_glProgramID, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
////		printf("%-5d %s (%s)\n)", results[2], name, "chicken"); //getTypeString(results[1]));
////	}
////}
//
///*******************************************************************************
//     MARK:   Private
// ******************************************************************************/
//
//bool Program::checkShaderCompile(unsigned int shaderIndex, char* log, const int maxLogSize) {
//	int params = -1;
//	glGetShaderiv(shaderIndex, GL_COMPILE_STATUS, &params);
//	if (params != GL_TRUE) {
//		//		fprintf(stderr, "ERROR, could not compile shader at index %d.", shader_index);
//		//		_print_shader_info_log(shader_index);
//
//
//
//		int actualLength = 0;
//		char log_cstr[2048];
//		glGetShaderInfoLog(shaderIndex, maxLogSize, &actualLength, log_cstr);
//		//printf("shader info log for GL index %u:\n%s\n", shader_index, log_cstr);
//
//		strncpy(log, log_cstr, sizeof(log_cstr));
//
//		return false;
//	}
//	return true;
//}
//
//bool Program::checkProgramLink(unsigned int programIndex, char* log, const int maxLogSize) {
//	int params = -1;
//	glGetProgramiv(programIndex, GL_LINK_STATUS, &params);
//	if (params != GL_TRUE) {
//		//		fprintf(stderr, "ERROR, could not link program at index %d.", program_index);
//		//		_print_program_info_log(program_index);
//
//
//
//		int actualLength = 0;
//		char log_cstr[2048];
//		glGetProgramInfoLog(programIndex, maxLogSize, &actualLength, log_cstr);
//		//printf("program info log for GL index %u:\n%s\n", program_index, log_cstr);
//
//		strncpy(log, log_cstr, sizeof(log_cstr));
//
//		return false;
//	}
//	return true;
//}

