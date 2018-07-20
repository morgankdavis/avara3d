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
	class Renderer;


	class Geometry : public std::enable_shared_from_this<Geometry> {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		Geometry();
		Geometry(const std::shared_ptr<GeometryElement> element,
				 const std::shared_ptr<Material> material);
		Geometry(const std::vector<std::shared_ptr<GeometryElement>> elements,
				 const std::vector<std::shared_ptr<Material>> materials);
		virtual ~Geometry();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
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
		     Internal
		 ***************************************************************************************/
		
		void burnTransform(const glm::mat4& transform, bool normals);
		
		void draw(Renderer& renderer,
				  const glm::mat4& modelMat,
				  const glm::mat4& viewMat,
				  const glm::mat4& projectionMat,
				  const DEBUG_OPTIONS& debugOptions,
				  RenderStats& stats);
		
		std::shared_ptr<std::map<std::string, glm::vec3>> boundingPoints(bool worldSpace) const;
		glm::vec3 extent(bool worldSpace) const;
		
		void attachedToNode(std::shared_ptr<Node> node);
		
		std::weak_ptr<Node> node() const;

		GEOMETRY_DIRTY_BITS dirtyBits() const;
		void dirtyBits(GEOMETRY_DIRTY_BITS bits);

	protected:

		/***************************************************************************************
		     Protected
		 ***************************************************************************************/

		std::vector<std::shared_ptr<GeometryElement>>		m_elements;
		std::vector<std::shared_ptr<Material>>				m_materials;

	private:

		/***************************************************************************************
		     Private
		 ***************************************************************************************/

		boost::optional<std::string>						m_name;
		std::weak_ptr<Node>									m_node;

		GEOMETRY_DIRTY_BITS									m_dirtyBits;
	};
}


#endif /* Geometry_h */
