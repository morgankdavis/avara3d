//
//  Geometry.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Geometry_h
#define Geometry_h


#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <glm/glm.hpp>

#include "Types.h"


namespace ae {


	class GeometryElement;
	class Material;
	class Node;
    class Program;


	class Geometry {
		
	public:
		
		virtual void thing();
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/

		Geometry();
		Geometry(const std::vector<std::shared_ptr<GeometryElement>> elements,
				 const std::vector<std::shared_ptr<Material>> materials);
		~Geometry();
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		boost::optional<std::string> name() const;
		void name(const std::string& name);
		
		const std::vector<std::shared_ptr<GeometryElement>>& elements();
		const std::vector<std::shared_ptr<Material>>& materials();
		
		std::shared_ptr<Material> firstMaterial() const;
		std::shared_ptr<Material> materialNamed(const std::string& name) const;
		void addMaterial(const std::shared_ptr<Material> material);
		void insertMaterial(const std::shared_ptr<Material> material, int index);
		void removeMaterial(int index);
		void replaceMaterial(int index, const std::shared_ptr<Material> replacement);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		Node* node() const;
		void node(Node* node);
		
		void loadVertexData();

		void hardTransform(glm::mat4 t, bool norm);
		void generateSmoothNormals();
		void generateFlatNormals();
		
		void draw(const glm::mat4& modelMat,
				  const glm::mat4& viewMat,
				  const glm::mat4& projectionMat,
				  int glEnvironmentUBO,
				  DebugOption debugOptions,
				  DrawStats& stats);
		
		void drawAABB(const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat);
		
		std::shared_ptr<std::map<std::string, glm::vec3>> boundingPoints(bool worldSpace) const;
        void loadAABBVertexData(const Program& program);

	protected:

		/***************************************************************************************
		     MARK:   Protected
		 **************************************************************************************/

		std::vector<std::shared_ptr<GeometryElement>>			m_elements;
		std::vector<std::shared_ptr<Material>>					m_materials;

	private:

		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/

		boost::optional<std::string>							m_name;
		Node*													m_node;
		unsigned												m_glAABBVBO;
        int                                                		m_glAABBVAO;
	};
}


#endif /* Geometry_h */
