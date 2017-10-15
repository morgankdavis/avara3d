//
//  Program.h
//  avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Program_h
#define Program_h


#include <string>

#include <GL/glew.h>

#include "Types.h"


namespace ae {

	class Program {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Program(const ProgramType type);
		Program(const std::string& vertexShader, const std::string& fragmentShader);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		bool compile(); // TODO: make async
		
		std::string info() const;
		
		std::string vertexShader() const;
		void vertexShader(const std::string source);
		
		std::string fragmentShader() const;
		void fragmentShader(const std::string source);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		GLuint glProgramID() const;
//		std::string activeInputAttributes() const;
		
	private:

		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		bool checkShaderCompile(unsigned int shaderIndex, char* log, const int maxLogSize);
		bool checkProgramLink(unsigned int programIndex, char* log, const int maxLogSize);
		
		std::string			m_vertexShader;
		std::string			m_fragmentShader;
		GLuint				m_glProgramID;
	};
}


#endif /* Program_h */
