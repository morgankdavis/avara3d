//
//  Program.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//
//  Based largely on code from "OpenGL 4 Shading Language Cookbook" by David Wolff
//

#ifndef AVARA3D_PROGRAM_H
#define AVARA3D_PROGRAM_H

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "a3d/Types.h"
#include "a3d/rendering/renderer/opengl/gl_types.h"

namespace a3d {

	class Program {

	public:
		/// Internal Static Member Functions ///

		static Program& 	Default();
		static Program& 	Skybox();
		static Program& 	Wireframe();
		static Program& 	Lines();
		static Program& 	GroundPlane();

		/// Internal Lifecycle Functions ///

		explicit Program(const std::string& name);
		~Program();

		/// Internal Member Functions ///

		bool 							compile();
		bool 							link();
		bool 							validate();
		void 							use();
		void 							unuse();

		void 							bindAttributeLocation(unsigned location, const char* name);

		void 							setUniform(const char* name, float x, float y, float z);
		void 							setUniform(const char* name, const math::vec2& v);
		void 							setUniform(const char* name, const math::vec3& v);
		void 							setUniform(const char* name, const math::vec4& v);
		void 							setUniform(const char* name, const math::mat3& m);
		void 							setUniform(const char* name, const math::mat4& m);
		void 							setUniform(const char* name, bool val);
		void 							setUniform(const char* name, int val);
		void 							setUniform(const char* name, unsigned val);
		void 							setUniform(const char* name, float val);

//		void 							bindUniformBlock(const char* name,
//														 unsigned location);
		void 							setUniformBlockBinding(const char* blockName,
															   unsigned bindingPoint);

		void 							bindTexture(const char* name,
													const int& target,
													const unsigned& slot,
													const unsigned& textureID,
													unsigned index);

		unsigned 						getAttributeLocation(const char* name) const;

		const std::string&				name() const;
		gl::uint_t						glID();
		bool	 						isLinked() const;

	private:
		/// Private Member Functions ///

		std::optional<std::string>		shaderSource(const std::string& name,
													   const std::string& type);
		void 							prepare();
		bool 							compile(const std::string& source,
												ShaderType type);
		int 							getUniformLocation(const char* name);
		void 							glID(unsigned glID);
		void 							isLinked(bool isLinked);

		/// Private Member Variables ///

		std::string  					_name;
		unsigned  						_glID;
		bool 							_isLinked;
		std::optional<std::string>		_logString;
		std::string						_vertexShaderSource;
		std::string						_fragmentShaderSource;
	};
}

#endif /* AVARA3D_PROGRAM_H */
