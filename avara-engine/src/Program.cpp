//
//  Program.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Program.h"

#include <cstring>
#include <iostream>

#include <GL/glew.h>

#include "Utilities.h"


using namespace ae;
using namespace std;


/*******************************************************************************
     MARK:   Constants
 ******************************************************************************/

#define MAX_LOG_SIZE	2048

/*******************************************************************************
     MARK:   Lifecycle
 ******************************************************************************/

Program::Program(const ProgramType type):
	m_glProgramID(0) {
		
		string shaderName = "";
	
	switch (type) {
		case ProgramTypeDefault:
			shaderName = "default";
			break;
		case ProgramTypePhongPlain:
			shaderName = "phone_plain";
			break;
		case ProgramTypePhongTexture:
			shaderName = "phong_texture"; // temporary
			break;
		case ProgramTypeFancy:
			shaderName = "fancy"; // temporary
			break;
		default:
			break;
	}
	
//	auto vs = utils::loadTextFile(basePath + shaderName + ".vert");
//	auto fs = utils::loadTextFile(basePath + shaderName + ".frag");
		
		auto vs = utils::ShaderSourceNamed(shaderName, "vert");
		auto fs = utils::ShaderSourceNamed(shaderName, "frag");
	
	if (vs && fs) {
		m_vertexShader = *vs;
		m_fragmentShader = *fs;
		
		cout << "Shader program '" << shaderName << "' compiled." << endl;
	}
	else {
		cout << "Couldn't load shaders." << endl;
	}
}

Program::Program(const string& vertexShader, const string& fragmentShader):
	m_vertexShader(vertexShader),
	m_fragmentShader(fragmentShader),
	m_glProgramID(0) {
	
}

/*******************************************************************************
     MARK:   Public
 ******************************************************************************/

bool Program::compile() {

	// TODO: convert to exceptions
	
	string vertexShaderSrc = vertexShader();
	string fragmentShaderSrc = fragmentShader();
	
	if (vertexShaderSrc.length() && fragmentShaderSrc.length()) {
		const char *vertSourceCStr = vertexShaderSrc.c_str();
		const char *fragSourceCStr = fragmentShaderSrc.c_str();

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertSourceCStr, NULL);
		glCompileShader(vs);
		
		char log[MAX_LOG_SIZE];
		if (!checkShaderCompile(vs, log, MAX_LOG_SIZE)) {
			cout << "Error compiling vertex shader: " << log << endl;
			return false;
		}
		
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fragSourceCStr, NULL);
		glCompileShader(fs);
		
		if (!checkShaderCompile(fs, log, MAX_LOG_SIZE)) {
			cout << "Error compiling fragment shader: " << log << endl;
			return false;
		}
		
		GLuint prog = glCreateProgram();
		glAttachShader(prog, fs);
		glAttachShader(prog, vs);
		
		glLinkProgram(prog);
		
		if (!checkProgramLink(prog, log, MAX_LOG_SIZE)) {
			cout << "Error linking program: " << log << endl;
			return false;
		}

		m_glProgramID = prog;
	}
	else {
		cout << "Missing vertex or fragment shader source." << endl;
		return false;
	}
	
	return true;
}

string Program::info() const {
	return "";
}

string Program::vertexShader() const {
	return m_vertexShader;
}

void Program::vertexShader(const string source) {
	m_vertexShader = source;
}

string Program::fragmentShader() const {
	return m_fragmentShader;
}

void Program::fragmentShader(const string source) {
	m_fragmentShader = source;
}

/*******************************************************************************
     MARK:   Internal
 ******************************************************************************/

GLuint Program::glProgramID() const {
	return m_glProgramID;
}

//string Program::activeInputAttributes() const {
//	GLint numAttribs;
//	glGetProgramInterfaceiv((GLuint)m_glProgramID, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);
//
//	GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION};
//
//	for (int i=0; i<numAttribs; ++i) {
//		GLint results[3];
//		glGetProgramResourceiv(m_glProgramID, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);
//		
//		GLint nameBufSize = results[0] + 1;
//		char name[nameBufSize];
//		glGetProgramResourceName(m_glProgramID, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
//		printf("%-5d %s (%s)\n)", results[2], name, "chicken"); //getTypeString(results[1]));
//	}
//}

/*******************************************************************************
     MARK:   Private
 ******************************************************************************/

bool Program::checkShaderCompile(unsigned int shaderIndex, char* log, const int maxLogSize) {
	int params = -1;
	glGetShaderiv(shaderIndex, GL_COMPILE_STATUS, &params);
	if (params != GL_TRUE) {
//		fprintf(stderr, "ERROR, could not compile shader at index %d.", shader_index);
//		_print_shader_info_log(shader_index);
		
		
		
		int actualLength = 0;
		char log_cstr[2048];
		glGetShaderInfoLog(shaderIndex, maxLogSize, &actualLength, log_cstr);
		//printf("shader info log for GL index %u:\n%s\n", shader_index, log_cstr);

		strncpy(log, log_cstr, sizeof(log_cstr));
		
		return false;
	}
	return true;
}

bool Program::checkProgramLink(unsigned int programIndex, char* log, const int maxLogSize) {
	int params = -1;
	glGetProgramiv(programIndex, GL_LINK_STATUS, &params);
	if (params != GL_TRUE) {
//		fprintf(stderr, "ERROR, could not link program at index %d.", program_index);
//		_print_program_info_log(program_index);
		
		
		
		int actualLength = 0;
		char log_cstr[2048];
		glGetProgramInfoLog(programIndex, maxLogSize, &actualLength, log_cstr);
		//printf("program info log for GL index %u:\n%s\n", program_index, log_cstr);

		strncpy(log, log_cstr, sizeof(log_cstr));
		
		return false;
	}
	return true;
}
