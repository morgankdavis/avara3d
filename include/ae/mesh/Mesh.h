//
//  Geometry.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Geometry_h
#define Geometry_h


#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "ae/Types.h"
#include "ae/mesh/MeshElement.h"
#include "ae/rendering/material/Material.h"


namespace ae {


	class Node;
	class Renderer;


	class Mesh : public std::enable_shared_from_this<Mesh> {

	public:

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

		static std::shared_ptr<Mesh> 		FromFile(const std::filesystem::path& path,
													 MeshImportOptions options =
													 MeshImportOptions::ImportMaterials);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Mesh();
		Mesh(const std::shared_ptr<MeshElement> element,
			 const std::shared_ptr<Material> material);
		Mesh(const std::vector<std::shared_ptr<MeshElement>> elements,
			 const std::vector<std::shared_ptr<Material>> materials);
		virtual ~Mesh();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> 					name() const;
		void 										name(const std::string& name);

		const std::vector<std::shared_ptr<MeshElement>>& 	elements();
		const std::vector<std::shared_ptr<Material>>& 		materials();

		std::shared_ptr<Material> 					firstMaterial() const;
		std::shared_ptr<Material> 					materialNamed(const std::string& name) const;
		void 										addMaterial(const std::shared_ptr<Material> material);
		void 										insertMaterial(const std::shared_ptr<Material> material,
																   int index);
		void 										removeMaterial(int index);
		void 										replaceMaterial(int index,
																	const std::shared_ptr<Material> replacement);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 										burnTransform(const glm::mat4& transform,
																  bool normals);

		void 										draw(Renderer& renderer,
														 const glm::mat4& modelMat,
														 const glm::mat4& viewMat,
														 const glm::mat4& projectionMat,
														 const DebugOptions& debugOptions,
														 Stats& stats);

		AABB										aabb(const std::shared_ptr<Node> convertToNode = nullptr) const;
		glm::vec3 									extent(const std::shared_ptr<Node> convertToNode = nullptr) const;

//		void 										attachedToNode(std::shared_ptr<Node> node);

//		std::weak_ptr<Node> 						node() const;

		MeshDirtyMask 								dirtyMask() const;
		void 										dirtyMask(MeshDirtyMask mask);

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		std::vector<std::shared_ptr<MeshElement>>	_elements;
		std::vector<std::shared_ptr<Material>>		_materials;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>					_name;
//		std::weak_ptr<Node>							_node;

		MeshDirtyMask								_dirtyMask;
	};
}


#endif /* Geometry_h */
