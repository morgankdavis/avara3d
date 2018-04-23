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
#include <boost/filesystem.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Types.h"


namespace ae {


	class Color;
	class GeometryElement;
	class Material;
	class MaterialProperty;
	class Node;
	class PhysicsWorld;
	class Renderer;
	class SkyboxGeometry;
	//class Window;
	
	
	class Scene : public std::enable_shared_from_this<Scene> {
		
	public:
		
		/***************************************************************************************
		     Static
		 ***************************************************************************************/
		
		static std::shared_ptr<Scene> LoadFromFile(const boost::filesystem::path& path);

		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Scene();

		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		std::shared_ptr<Node> rootNode() const;
		void rootNode(std::shared_ptr<Node> node);
		
		std::shared_ptr<MaterialProperty> background() const;
		void background(std::shared_ptr<MaterialProperty> background);
		
		//bool isPaused;

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
		
		std::shared_ptr<PhysicsWorld> physicsWorld() const;
		void physicsWorld(std::shared_ptr<PhysicsWorld> world);
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		void draw(std::shared_ptr<Node> pointOfView,
				  DEBUG_OPTIONS& debugOptions,
				  DrawStats& stats);
		
		std::shared_ptr<std::map<std::string, glm::vec3>> boundingPoints() const;
		
		glm::vec3 extent() const;
		
		//void attachedToWindow(std::shared_ptr<Window> window);
		void attachedToRenderer(std::shared_ptr<Renderer> renderer);
		
//		std::weak_ptr<Window> window() const;
//		void window(std::shared_ptr<Window> window);
		std::weak_ptr<Renderer> renderer() const;
		void renderer(std::shared_ptr<Renderer> renderer);
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		void bindEnvironment(const Node& pointOfView, DrawStats& stats) const;
		
		std::shared_ptr<Node>					m_rootNode;
		std::shared_ptr<MaterialProperty>		m_background;
		std::shared_ptr<SkyboxGeometry>			m_skyboxGeometry;
		
		float									m_fogStartDistance;
		float									m_fogEndDistance;
		float									m_fogDensityExponent;
		std::shared_ptr<Color>					m_fogColor;
		
		int										m_glEnvironmentUBO;
		
		std::shared_ptr<PhysicsWorld> 			m_physicsWorld;
		
		//std::weak_ptr<Window>					m_window;
		std::weak_ptr<Renderer>					m_renderer;
	};
}


#endif /* Scene_h */
