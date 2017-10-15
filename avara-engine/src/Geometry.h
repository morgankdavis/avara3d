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

#include <glm/glm.hpp>


namespace ae {


	class GeometryElement;
	class Material;
	class Node;


	class Geometry {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/

		Geometry(const std::vector<std::shared_ptr<GeometryElement>> elements,
				 const std::vector<std::shared_ptr<Material>> materials);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::string name() const;
		void name(const std::string& name);
		
		std::vector<std::shared_ptr<GeometryElement>>& elements();
		
		std::vector<std::shared_ptr<Material>>& materials();
		
//		Material firstMaterial();
//		Material materialNamed(const std::string& name);
//		void insertMaterial(const Material& material, const int index);
//		void removeMaterial(const int index);
//		void removeMaterial(const int index, const Material& replacement);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		Node* node() const;
		void node(Node* node);

		unsigned int draw(const glm::mat4& viewMat, const glm::mat4& projectionMat);
		std::shared_ptr<std::map<std::string, glm::vec3>> boundingPoints() const;

	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/

		std::string												m_name;
		std::vector<std::shared_ptr<GeometryElement>>			m_elements;
		std::vector<std::shared_ptr<Material>>					m_materials;
		Node*													m_node;
	};
}


#endif /* Geometry_h */
