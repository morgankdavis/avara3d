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
//#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Types.h"


namespace ae {


	class Color;
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
		
		Scene(); // no want but Window is complaining about wanting a default contructor
		Scene(const std::string& path);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::shared_ptr<Node> rootNode() const;
		
		std::shared_ptr<MaterialProperty> background() const;
		void background(std::shared_ptr<MaterialProperty> background);
		
		//bool					isPaused;

		float fogStartDistance() const;
		void fogStartDistance(float distance);
		float fogEndDistance() const;
		void fogEndDistance(float distance);
		float fogDensityExponent() const;
		void fogDensityExponent(float exponent);
		// 0 = constant, alpha respected
		// 1 = linear, alpha ignored
		// >=2 = exponential, alpha ignored
		std::shared_ptr<Color> fogColor() const;
		void fogColor(std::shared_ptr<Color> color);
		
		//PhysicsWorld			physicsWorld;

		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		unsigned draw(std::shared_ptr<Node> pointOfView, DebugOption debugOptions) const;
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
		void bindEnvironment(const Node& pointOfView) const;
		
		std::shared_ptr<Node>						m_rootNode;
		std::shared_ptr<MaterialProperty>			m_background;
		std::shared_ptr<SkyboxGeometry>				m_skyboxGeometry;
		
		float										m_fogStartDistance;
		float										m_fogEndDistance;
		float										m_fogDensityExponent;
		std::shared_ptr<Color>						m_fogColor;
		
		int											m_glEnvironmentUBO;
	};
}


#endif /* Scene_h */
