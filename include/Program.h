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
#include <optional>
#include <string>

#include "Types.h"


namespace ae {

	class Program : public std::enable_shared_from_this<Program> {

/*********************************************************************************************
	Public Static
 *********************\************************************************************************/

	public:

		static std::shared_ptr<ae::Program> 	Default();
		static std::shared_ptr<ae::Program> 	Skybox();
		static std::shared_ptr<ae::Program> 	Wireframe();
		static std::shared_ptr<ae::Program> 	Lines();
		static std::shared_ptr<ae::Program> 	Points();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Program(const std::string& name);
		~Program();
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		bool 							compile();
		bool 							link();
		bool 							validate();
		void 							use();
		void 							unuse();
		
		void 							bindAttribLocation(unsigned location, const char* name);
		//void bindFragDataLocation(unsigned location, const char* name);
		
		void 							setUniform(const char* name, float x, float y, float z);
		void 							setUniform(const char* name, const glm::vec2& v);
		void 							setUniform(const char* name, const glm::vec3& v);
		void 							setUniform(const char* name, const glm::vec4& v);
		void 							setUniform(const char* name, const glm::mat3& m);
		void 							setUniform(const char* name, const glm::mat4& m);
		void 							setUniform(const char* name, bool val);
		void 							setUniform(const char* name, int val);
		void 							setUniform(const char* name, float val);
		
		void 							bindUniformBlock(const char* name,
														 unsigned location);
		
		void 							bindTexture(const char* name,
													const int& target,
													const unsigned& slot,
													const unsigned& textureID, unsigned index);
		
		unsigned 						getAttributeLocation(const char* name) const;
		
//		void 							printActiveUniforms() const;
//		void 							printActiveAttribs() const;
		
		std::string 					name() const;
		unsigned 						glID();
		bool	 						isLinked() const;
		std::optional<std::string> 		vertexShaderSource() const;
		void 							vertexShaderSource(std::string source);
		std::optional<std::string> 		fragmentShaderSource() const;
		void 							fragmentShaderSource(std::string source);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string> 		ShaderSource(const std::string& name, const std::string& type);
		void 							prepare();
		bool 							compileShaderFromString(const std::string& source,
																SHADER_TYPE type);
		unsigned 						getUniformLocation(const char* name);
		void 							glID(unsigned glID);
		void 							isLinked(bool isLinked);
		void 							logString(std::optional<std::string> logString);
		
		std::string  					_name;
		unsigned  						_glID;
		bool 							_isLinked;
		std::optional<std::string>		_logString;
		std::optional<std::string>		_vertexShaderSource;
		std::optional<std::string>		_fragmentShaderSource;
		std::map<std::string, int>		_uniformLocations;
	};
}


#endif /* Program_h */
