//
//  Scene.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Scene_h
#define Scene_h


#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

//#include <boost/filesystem.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Types.h"


namespace ae {


	class Color;
	class Geometry;
	class MaterialProperty;
	class Node;
	class PhysicsSimulator;
	class PhysicsWorld;
	class Renderer;
	class RenderContext;
	
	
	class Scene : public std::enable_shared_from_this<Scene> {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

#ifndef ANDROID
		static std::shared_ptr<ae::Scene> 		LoadFromFile(const std::filesystem::path& path);
#endif
//		static std::shared_ptr<Scene> LoadFromData(const std::vector<unsigned char>& data);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Scene();
		~Scene();

/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::shared_ptr<ae::Node> 				rootNode() const;
		void 									rootNode(std::shared_ptr<ae::Node> node);
		
		std::shared_ptr<ae::MaterialProperty>	background() const;
		void 									background(std::shared_ptr<ae::MaterialProperty> background);
		
		float 									fogStartDistance() const;
		void 									fogStartDistance(float distance);
		float 									fogEndDistance() const;
		void 									fogEndDistance(float distance);
		float 									fogDensityExponent() const;
		void 									fogDensityExponent(float exponent);
													// 0 = constant, alpha respected
													// 1 = linear, alpha ignored
													// >=2 = exponential, alpha ignored
		std::shared_ptr<ae::Color> 				fogColor() const;
		void 									fogColor(std::shared_ptr<ae::Color> color);
		
		std::shared_ptr<ae::PhysicsWorld> 		physicsWorld() const;
		void 									physicsWorld(std::shared_ptr<ae::PhysicsWorld> world);
		
/*********************************************************************************************
	Internal
 ************************************************************************ *********************/

		void 												draw(Renderer& renderer,
															unsigned framebufferWidth,
															unsigned framebufferHeight,
													 		Node& pointOfView,
													 		const DEBUG_OPTIONS& debugOptions,
													 		RenderStats& stats);

		std::shared_ptr<ae::Geometry>						skyboxGeometry() const;
		
		std::shared_ptr<std::map<std::string, glm::vec3>>	boundingPoints() const;
		glm::vec3 											extent() const;
		
		void 												attachedToRenderContext(std::shared_ptr<ae::RenderContext> renderContext);

		std::weak_ptr<ae::RenderContext> 					renderContext() const;
		void 												renderContext(std::shared_ptr<ae::RenderContext> context);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<ae::Node>					_rootNode;
		std::shared_ptr<ae::MaterialProperty>		_background;
		std::shared_ptr<ae::Geometry>				_skyboxGeometry;
		float										_fogStartDistance;
		float										_fogEndDistance;
		float										_fogDensityExponent;
		std::shared_ptr<ae::Color>					_fogColor;
		std::shared_ptr<ae::PhysicsWorld> 			_physicsWorld;
		std::weak_ptr<ae::RenderContext>			_renderContext;
	};
}


#endif /* Scene_h */
