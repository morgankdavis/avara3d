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
#include <optional>
#include <string>
#include <vector>

//#include <boost/optional.hpp>
#include <glm/glm.hpp>

#include "GeometryElement.h"
#include "Material.h"
#include "Types.h"


namespace ae {


//	class GeometryElement;
//	class Material;
	class Node;
	class Renderer;


	class Geometry : public std::enable_shared_from_this<Geometry> {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Geometry();
		Geometry(const std::shared_ptr<ae::GeometryElement> element,
				 const std::shared_ptr<ae::Material> material);
		Geometry(const std::vector<std::shared_ptr<ae::GeometryElement>> elements,
				 const std::vector<std::shared_ptr<ae::Material>> materials);
		virtual ~Geometry();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::optional<std::string> 							name() const;
		void 												name(const std::string& name);

		const std::vector<std::shared_ptr<ae::GeometryElement>>& 			elements();
		const std::vector<std::shared_ptr<ae::Material>>& 					materials();

		std::shared_ptr<ae::Material> 										firstMaterial() const;
		std::shared_ptr<ae::Material> 										materialNamed(const std::string& name) const;
		void 												addMaterial(const std::shared_ptr<ae::Material> material);
		void 												insertMaterial(const std::shared_ptr<ae::Material> material,
																		   int index);
		void 												removeMaterial(int index);
		void 												replaceMaterial(int index,
																			const std::shared_ptr<ae::Material> replacement);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void 												burnTransform(const glm::mat4& transform,
																		  bool normals);

		void 												draw(Renderer& renderer,
																 const glm::mat4& modelMat,
																 const glm::mat4& viewMat,
																 const glm::mat4& projectionMat,
																 const DEBUG_OPTIONS& debugOptions,
																 RenderStats& stats);

		std::shared_ptr<std::map<std::string, glm::vec3>> 	boundingPoints(bool worldSpace) const;
		glm::vec3 											extent(bool worldSpace) const;
		
		void 												attachedToNode(std::shared_ptr<Node> node);
		
		std::weak_ptr<Node> 								node() const;

		GEOMETRY_DIRTY_BITS 								dirtyBits() const;
		void 												dirtyBits(GEOMETRY_DIRTY_BITS bits);

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		std::vector<std::shared_ptr<ae::GeometryElement>>					_elements;
		std::vector<std::shared_ptr<ae::Material>>							_materials;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>							_name;
		std::weak_ptr<Node>									_node;

		GEOMETRY_DIRTY_BITS									_dirtyBits;
	};
}


#endif /* Geometry_h */
