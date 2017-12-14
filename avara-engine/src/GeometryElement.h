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

#include "Types.h"


namespace ae {


	class Material;
	class Program;


	class GeometryElement {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		//GeometryElement(std::string shaderName);
		GeometryElement(std::vector<Vertex>& verticies,
						std::vector<Face>& faces);
		GeometryElement(std::vector<Vertex>& verticies,
						std::vector<Face>& faces,
						std::string shaderName);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		std::shared_ptr<Program> program() const;
		void program(const std::shared_ptr<Program> program);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		unsigned draw(const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const Material* material);
		
		void hardTransform(const glm::mat4 t, bool norm);
		void generateSmoothNormals();
		void generateFlatNormals();

		void loadShaderNamed(const std::string& shaderName);
		void loadVertexData();

		std::vector<Vertex>& vertices();
		std::vector<Face>& faces();
		
	protected:
		
		/***************************************************************************************
		     MARK:   Protected
		 **************************************************************************************/
		
		std::shared_ptr<Program>				m_program;
		std::vector<Vertex>						m_vertices;
		std::vector<Face>						m_faces;
		GLuint									m_glVAO;
		GLuint									m_glIBO;
		
//	private:
//		
//		/***************************************************************************************
//		     MARK:   Private
//		 **************************************************************************************/
//		
//		std::shared_ptr<Program>				m_program;
//		std::vector<Vertex>						m_vertices;
//		std::vector<Face>						m_faces;
//		GLuint									m_glVAO;
//		GLuint									m_glIBO;
	};
}


#endif /* GeometryElement_h */
