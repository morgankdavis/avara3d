//
//  Program.h
//  avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//
//  Based largely on code from "OpenGL 4 Shading Language Cookbook" by David Wolff
//

#ifndef Program_h
#define Program_h


#include <string>

#include <boost/optional.hpp>
#include <GL/glew.h>

#include "Types.h"


namespace ae {

	class Program {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Program(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		boost::optional<std::string> logString();
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		bool compile();
		bool link();
		bool validate();
		void use();
		
		void bindAttribLocation(GLuint location, const char* name);
		void bindFragDataLocation(GLuint location, const char* name);
		
		void setUniform(const char* name, float x, float y, float z);
		void setUniform(const char* name, const glm::vec2& v);
		void setUniform(const char* name, const glm::vec3& v);
		void setUniform(const char* name, const glm::vec4& v);
		void setUniform(const char* name, const glm::mat3& m);
		void setUniform(const char* name, const glm::mat4& m);
		void setUniform(const char* name, bool val);
		void setUniform(const char* name, int val);
		void setUniform(const char* name, float val);
		
		void printActiveUniforms();
		void printActiveAttribs();
		
		GLuint glID();
		bool isLinked();
		boost::optional<std::string> vertexShaderSource();
		void vertexShaderSource(boost::optional<std::string> source);
		boost::optional<std::string> fragmentShaderSource();
		void fragmentShaderSource(boost::optional<std::string> source);
		
	private:

		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		bool compileShaderFromString(const std::string& source, ShaderType type);
		GLint getUniformLocation(const char* name);
		
		void glID(GLuint glID);
		void isLinked(bool isLinked);
		void logString(boost::optional<std::string> logString);
		
		GLuint  						m_glID;
		bool 							m_isLinked;
		boost::optional<std::string>	m_logString;
		boost::optional<std::string>	m_vertexShaderSource;
		boost::optional<std::string>	m_fragmentShaderSource;
	};
}


#endif /* Program_h */










////
////  Program.h
////  avara-engine
////
////  Created by Morgan Davis on 12/23/16.
////  Copyright © 2016 Morgan K Davis. All rights reserved.
////
//
//#ifndef Program_h
//#define Program_h
//
//
//#include <string>
//
//#include <GL/glew.h>
//
//#include "Types.h"
//
//
//namespace ae {
//
//	class Program {
//
//	public:
//
//		/***************************************************************************************
//		     MARK:   Lifecycle
//		 **************************************************************************************/
//
//		Program(const ProgramType type);
//		Program(const std::string& vertexShader, const std::string& fragmentShader);
//
//		/***************************************************************************************
//		     MARK:   Public
//		 **************************************************************************************/
//
//		bool compile(); // TODO: make async
//
//		std::string info() const;
//
//		std::string vertexShader() const;
//		void vertexShader(const std::string source);
//
//		std::string fragmentShader() const;
//		void fragmentShader(const std::string source);
//
//		/***************************************************************************************
//		     MARK:   Internal
//		 **************************************************************************************/
//
//		GLuint glProgramID() const;
//		//		std::string activeInputAttributes() const;
//
//	private:
//
//		/***************************************************************************************
//		     MARK:   Private
//		 **************************************************************************************/
//
//		bool checkShaderCompile(unsigned int shaderIndex, char* log, const int maxLogSize);
//		bool checkProgramLink(unsigned int programIndex, char* log, const int maxLogSize);
//
//		std::string			m_vertexShader;
//		std::string			m_fragmentShader;
//		GLuint				m_glProgramID;
//	};
//}
//
//
//#endif /* Program_h */

