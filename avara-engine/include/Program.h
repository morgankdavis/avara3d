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


#include <map>
#include <memory>
#include <string>

#include <boost/optional.hpp>

#include "Types.h"


namespace ae {

	class Program : public std::enable_shared_from_this<Program> {
		
	public:
		
/*********************************************************************************************
     Public Static
 *********************************************************************************************/
		
		static std::shared_ptr<Program> Default();
		static std::shared_ptr<Program> Skybox();
		static std::shared_ptr<Program> Wireframe();
		static std::shared_ptr<Program> Lines();
		static std::shared_ptr<Program> Points();
		
/*********************************************************************************************
     Lifecycle
 *********************************************************************************************/
		
		Program(const std::string& name);
		~Program();
		
/*********************************************************************************************
     Internal
 *********************************************************************************************/
		
		bool compile();
		bool link();
		bool validate();
		void use();
		void unuse();
		
		void bindAttribLocation(unsigned location, const char* name);
		//void bindFragDataLocation(unsigned location, const char* name);
		
		void setUniform(const char* name, float x, float y, float z);
		void setUniform(const char* name, const glm::vec2& v);
		void setUniform(const char* name, const glm::vec3& v);
		void setUniform(const char* name, const glm::vec4& v);
		void setUniform(const char* name, const glm::mat3& m);
		void setUniform(const char* name, const glm::mat4& m);
		void setUniform(const char* name, bool val);
		void setUniform(const char* name, int val);
		void setUniform(const char* name, float val);
		
		void bindUniformBlock(const char* name, unsigned location);
		
		void bindTexture(const char* name, const int& target, const unsigned& slot,
						 const unsigned& textureID, unsigned index);
		
		unsigned getAttributeLocation(const char* name) const;
		
//		void printActiveUniforms() const;
//		void printActiveAttribs() const;
		
		std::string name() const;
		unsigned glID();
		bool isLinked() const;
		boost::optional<std::string> vertexShaderSource() const;
		void vertexShaderSource(std::string source);
		boost::optional<std::string> fragmentShaderSource() const;
		void fragmentShaderSource(std::string source);
		
	private:

/*********************************************************************************************
     Private
 *********************************************************************************************/
		
		void prepare();
		
		bool compileShaderFromString(const std::string& source, SHADER_TYPE type);
		unsigned getUniformLocation(const char* name);
		
		void glID(unsigned glID);
		void isLinked(bool isLinked);
		void logString(boost::optional<std::string> logString);
		
		std::string  					m_name;
		unsigned  						m_glID;
		bool 							m_isLinked;
		boost::optional<std::string>	m_logString;
		boost::optional<std::string>	m_vertexShaderSource;
		boost::optional<std::string>	m_fragmentShaderSource;
		std::map<std::string, int>		m_uniformLocations;
	};
}


#endif /* Program_h */
