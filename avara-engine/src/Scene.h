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


	class Material;
	class Node;
	class GeometryElement;
	
	
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
		
//		bool					isPaused;
//		
//		MaterialProperty		background;
//		MaterialProperty		lightingEnvironment;
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
		
		std::vector<std::shared_ptr<GeometryElement>>& geometryElements();
		std::vector<std::shared_ptr<Material>>& materials();
		std::shared_ptr<std::map<std::string, glm::vec3>> boundingPoints() const;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		void loadFile(const std::string& path);
		void addAIGeometryNodes(const aiScene* aiScene,
								std::shared_ptr<Node> aeRootNode);
		void addAIGeometryNodeRec(const aiScene* aiScene,
								  const aiNode* aiGeometryNode,
								  std::shared_ptr<Node> aeParentNode);
		
		std::shared_ptr<Node>								m_rootNode;
		std::vector<std::shared_ptr<GeometryElement>>		m_geometryElements;
		std::vector<std::shared_ptr<Material>>				m_materials;
	};
}


#endif /* Scene_h */
