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

#include "Aliases.h"
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
		static SceneSPtr 			LoadFromFile(const std::filesystem::path& path);
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
		
		NodeSPtr 					rootNode() const;
		void 						rootNode(NodeSPtr node);
		
		MaterialPropertySPtr 		background() const;
		void 						background(MaterialPropertySPtr background);
		
		float 						fogStartDistance() const;
		void 						fogStartDistance(float distance);
		float 						fogEndDistance() const;
		void 						fogEndDistance(float distance);
		float 						fogDensityExponent() const;
		void 						fogDensityExponent(float exponent);
										// 0 = constant, alpha respected
										// 1 = linear, alpha ignored
										// >=2 = exponential, alpha ignored
		ColorSPtr 					fogColor() const;
		void 						fogColor(ColorSPtr color);
		
		PhysicsWorldSPtr 			physicsWorld() const;
		void 						physicsWorld(PhysicsWorldSPtr world);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 						draw(Renderer& renderer,
										 unsigned framebufferWidth,
										 unsigned framebufferHeight,
										 Node& pointOfView,
										 const DEBUG_OPTIONS& debugOptions,
										 RenderStats& stats);
		
		GeometrySPtr				skyboxGeometry() const;
		
		std::shared_ptr<std::map<std::string, glm::vec3>> 	boundingPoints() const;
		glm::vec3 					extent() const;
		
		void 						attachedToRenderContext(RenderContextSPtr renderContext);

		RenderContextWPtr 			renderContext() const;
		void 						renderContext(RenderContextSPtr context);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		NodeSPtr					_rootNode;
		MaterialPropertySPtr		_background;
		SkyboxGeometrySPtr			_skyboxGeometry;
		float						_fogStartDistance;
		float						_fogEndDistance;
		float						_fogDensityExponent;
		ColorSPtr					_fogColor;
		PhysicsWorldSPtr 			_physicsWorld;
		RenderContextWPtr			_renderContext;
	};
}


#endif /* Scene_h */
