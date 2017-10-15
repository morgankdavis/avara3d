//
//  GeometryElement.h
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef GeometryElement_h
#define GeometryElement_h


#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


namespace ae {


	class Program;


	typedef struct {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoordinate;
	} Vertex;

	typedef struct {
		unsigned int a;
		unsigned int b;
		unsigned int c;
	} Face;
	
	
	class GeometryElement {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		GeometryElement(std::vector<Vertex> verticies, std::vector<Face> faces);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		std::shared_ptr<Program> program() const;
		void program(const std::shared_ptr<Program> program);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/

		void load();
		GLint glVAO();
		GLint glIBO();
		std::vector<Vertex>	vertices() const;
		std::vector<Face> faces() const;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		std::shared_ptr<Program>				m_program;
		std::vector<Vertex>						m_vertices;
		std::vector<Face>						m_faces;
		GLint									m_glVAO;
		GLint									m_glIBO;
	};
}


#endif /* GeometryElement_h */
