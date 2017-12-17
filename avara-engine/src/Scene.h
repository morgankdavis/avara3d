//
//  Scene.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Scene_h
#define Scene_h


#include <map>
#include <memory>
#include <string>
#include <vector>

#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Types.h"


namespace ae {


	class GeometryElement;
	class Material;
	class MaterialProperty;
	class Node;
	class SkyboxGeometry;
	
	
	class Scene {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Scene(); // no want but Window is complaining about wanting a "default contructor" for us
		Scene(const std::string& path);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::shared_ptr<Node> rootNode() const;
		//void rootNode(const std::shared_ptr<Node> node);
		
		std::shared_ptr<MaterialProperty> background() const;
		void background(std::shared_ptr<MaterialProperty> background);
		
//		bool					isPaused;
//
//		float					fogDistanceStart;
//		float					fogDistanceEnd;
//		float					fogDensityExponent;
//		Color					fogColor;
//		
//		PhysicsWorld			physicsWorld;
//
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		unsigned draw(std::shared_ptr<Node> pointOfView) const;
		std::shared_ptr<std::map<std::string, glm::vec3>> boundingPoints() const;
		glm::vec3 extent() const;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		void loadFile(const std::string& path);
		void addAIGeometryNodes(const aiScene* aiScene,
								std::shared_ptr<Node> aeRootNode,
								const std::vector<std::shared_ptr<GeometryElement>>& elements,
								const std::vector<std::shared_ptr<Material>>& materials);
		void addAIGeometryNodeRec(const aiScene* aiScene,
								  const aiNode* aiGeometryNode,
								  std::shared_ptr<Node> aeParentNode,
								  const std::vector<std::shared_ptr<GeometryElement>>& elements,
								  const std::vector<std::shared_ptr<Material>>& materials);
		
		std::shared_ptr<Node>								m_rootNode;
		std::shared_ptr<MaterialProperty>					m_background;
		std::shared_ptr<SkyboxGeometry>						m_skyboxGeometry;
	};
}


#endif /* Scene_h */
