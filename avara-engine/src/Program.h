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


#include <memory>
#include <string>

#include <boost/optional.hpp>
#include <GL/glew.h>

#include "Types.h"


namespace ae {

	class Program {
		
	public:
		
		/***************************************************************************************
		     MARK:   Static
		 **************************************************************************************/
		
		static std::shared_ptr<Program> Default();
		static std::shared_ptr<Program> Skybox();
		static std::shared_ptr<Program> Wireframe();
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Program(const std::string& name);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
//		boost::optional<std::string> logString() const;
		
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
		
		void bindUniformBlock(const char* name, GLuint location);
		
		void bindTexture(const char* name, const GLenum& slot, const GLuint& textureID, GLint index);
		
		GLint getAttributeLocation(const char* name) const;
		
//		void printActiveUniforms() const;
//		void printActiveAttribs() const;
		
		std::string name() const;
		GLuint glID();
		bool isLinked() const;
		boost::optional<std::string> vertexShaderSource() const;
		//void vertexShaderSource(boost::optional<std::string> source);
		void vertexShaderSource(std::string source);
		boost::optional<std::string> fragmentShaderSource() const;
		//void fragmentShaderSource(boost::optional<std::string> source);
		void fragmentShaderSource(std::string source);
		
	private:

		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		void prepare();
		
		bool compileShaderFromString(const std::string& source, ShaderType type);
		GLint getUniformLocation(const char* name);
		
		void glID(GLuint glID);
		void isLinked(bool isLinked);
//		void logString(boost::optional<std::string> logString);
		
		std::string  					m_name;
		GLuint  						m_glID;
		bool 							m_isLinked;
//		boost::optional<std::string>	m_logString;
		boost::optional<std::string>	m_vertexShaderSource;
		boost::optional<std::string>	m_fragmentShaderSource;
	};
}


#endif /* Program_h */
