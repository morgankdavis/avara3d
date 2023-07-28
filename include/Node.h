//
//  Node.h
//	avara-engine
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Node_h
#define Node_h


#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>

//#include <boost/optional.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Aliases.h"
#include "Types.h"


namespace ae {


	class Camera;
	class Geometry;
	class Light;
	class PhysicsBody;
	class Scene;

	
	class Node : public std::enable_shared_from_this<Node> {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static NodeSPtr 				GeometryNode(GeometrySPtr geometry);
		static NodeSPtr 				LightNode(LightSPtr light);
		static NodeSPtr 				CameraNode(CameraSPtr camera);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Node();
		Node(const std::string& name);
		~Node();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> 		name() const;
		void 							name(const std::string& name);
		
		LightSPtr 						light() const;
		void 							light(const LightSPtr light);
		
		CameraSPtr 						camera() const;
		void 							camera(const CameraSPtr camera);
		
		GeometrySPtr 					geometry() const;
		void 							geometry(const GeometrySPtr& geometry);

		bool 							hidden() const;
		void 							hidden(const bool hidden);
		
		float 							opacity() const;
		void				 			opacity(const float opacity);
		
		/* REMOVE? */ int 				renderOrder() const;
		/* REMOVE? */ void 				renderOrder(const int renderOrder);
		
		/* REMOVE? */ bool 				castsShadow() const;
		/* REMOVE? */ void			 	castsShadow(const bool castsShadow);
		
		glm::vec3 						position() const;
		void 							position(const glm::vec3 position);

		glm::vec4 						rotation() const; // axis-angle
		void 							rotation(const glm::vec4 rotation);
		
		glm::vec3 						eulerAngles() const; // pitch, yaw, roll
		void 							eulerAngles(const glm::vec3 eulerAngles);
		
		glm::quat 						orientation() const; // angle == 1st component
		void 							orientation(const glm::quat orientation);
		
		glm::vec3 						scale() const;
		void 							scale(const glm::vec3 scale);
		
		glm::mat4 						transform() const;
		void 							transform(const glm::mat4 transform);
		
		glm::vec3 						worldPosition();
		glm::vec4 						worldRotation(); // axis-angle
		glm::vec3 						worldEulerAngles(); // pitch, yaw, roll
		glm::quat 						worldOrientation(); // angle == 1st component
		glm::vec3 						worldScale();
		
		glm::vec3 						worldForward();
		glm::vec3 						worldUp();
		glm::vec3 						worldRight();
		
		glm::mat4 						worldTransform();
		
		void 							addChildren(std::vector<NodeSPtr> nodes);
		void 							addChild(NodeSPtr node);
		void 							insertChild(const Node& node, int index);
		void 							removeFromParent();
		void 							replaceChild(const Node& replace, const Node& with);
		
		std::weak_ptr<Node> 			parent() const;
		std::vector<NodeSPtr> 			children(bool resursive);
		NodeSPtr 						child(const std::string& name, bool resursive);
		
		PhysicsBodySPtr 				physicsBody() const;
		void 							physicsBody(PhysicsBodySPtr body);

//		glm::vec3 						convertPositionFromNode(const glm::vec3& position, const Node& fromNode);
//		glm::vec3 						convertPositionToNode(const glm::vec3& position, const Node& toNode);
//		glm::mat4 						convertTransformFromNode(const glm::mat4& transform, const Node& fromNode);
//		glm::mat4 						convertTransformToNode(const glm::mat4& transform, const Node& toNode);

/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void	 						unrollWorldTransform(glm::mat4 transform);
		
		void 							updateWorldTransform();
		bool 							containsChild(NodeSPtr node);
		void 							attachedToScene(SceneSPtr scene);
		void 							attachedToParent(NodeSPtr parentNode);
		
//		NodeSPtr 						root() const;
//		SceneWPtr 						scene() const;

//		NodeWPtr 						model() const;
//		void 							attachedToModel(NodeSPtr model);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::vector<NodeSPtr> 			pathToRoot() const;
		void			 				addDirtyBitsRecursive(NODE_DIRTY_BITS bits);
		std::vector<NodeSPtr> 			topologicalChildren(NodeSPtr top);
		void 							topologicalChildrenRec(NodeSPtr node,
															   std::map<NodeSPtr, bool>& visited,
															   std::stack<NodeSPtr>& stack);
		
		//void 							checkPhysicsScale(const glm::vec3& oldScale, const glm::vec3& newScale);
		
		NODE_DIRTY_BITS 				dirtyBits() const;
		void 							dirtyBits(NODE_DIRTY_BITS bits);
		
		std::optional<std::string>		_name;
		
		LightSPtr						_light;
		CameraSPtr						_camera;
		GeometrySPtr					_geometry;
		
		bool							_hidden;

		std::vector<NodeSPtr>			_children;
		
		glm::vec3						_position;
		glm::quat						_orientation;
		glm::vec3						_scale;
//		glm::mat4						_pivot;
		glm::mat4						_worldTransform;
		
		PhysicsBodySPtr					_physicsBody;
		
//		SceneWPtr 						_scene;
		NodeWPtr						_parent;
		
		NODE_DIRTY_BITS					_dirtyBits;
	};
}


#endif /* Node_h */
