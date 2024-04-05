//
//  Program.h
//  avara3d
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
#include <optional>
#include <string>


#include "a3d/Types.h"


namespace a3d {

	class Program {

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static Program& 	Default();
		static Program& 	Skybox();
		static Program& 	Wireframe();
		static Program& 	Lines();
		static Program& 	Points();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Program(const std::string& name);
		~Program();
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		bool 								compile();
		bool 								link();
		bool 								validate();
		void 								use();
		void 								unuse();
		
		void 								bindAttributeLocation(unsigned location, const char* name);
		//void bindFragDataLocation(unsigned location, const char* name);
		
		void 								setUniform(const char* name, float x, float y, float z);
		void 								setUniform(const char* name, const glm::vec2& v);
		void 								setUniform(const char* name, const glm::vec3& v);
		void 								setUniform(const char* name, const glm::vec4& v);
		void 								setUniform(const char* name, const glm::mat3& m);
		void 								setUniform(const char* name, const glm::mat4& m);
		void 								setUniform(const char* name, bool val);
		void 								setUniform(const char* name, int val);
		void 								setUniform(const char* name, float val);

		void 								bindUniformBlock(const char* name,
															 unsigned location);

		void 								bindTexture(const char* name,
														const int& target,
														const unsigned& slot,
														const unsigned& textureID, unsigned index);

		unsigned 							getAttributeLocation(const char* name) const;
		
//		void 								printActiveUniforms() const;
//		void 								printActiveAttribs() const;
		
		const std::string&					name() const;
		unsigned 							glID();
		bool	 							isLinked() const;
		const std::optional<std::string>&	vertexShaderSource() const;
		void 								vertexShaderSource(std::string source);
		const std::optional<std::string>&	fragmentShaderSource() const;
		void 								fragmentShaderSource(std::string source);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>			shaderSource(const std::string& name,
														   const std::string& type);
		void 								prepare();
		bool 								compile(const std::string& source,
													ShaderType type);
		unsigned 							getUniformLocation(const char* name);
		void 								glID(unsigned glID);
		void 								isLinked(bool isLinked);
		void 								logString(std::optional<std::string> logString);

		std::string  						_name;
		unsigned  							_glID;
		bool 								_isLinked;
		std::optional<std::string>			_logString;
		std::optional<std::string>			_vertexShaderSource;
		std::optional<std::string>			_fragmentShaderSource;
		std::map<std::string, int>			_uniformLocationCache;
	};
}


#endif /* Program_h */
