//
//  Mesh.h
//	avara3d
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

#include "a3d/Types.h"


namespace a3d {


	class Material;
	class MeshElement;
	class Node;
	class Renderer;


	class Mesh {

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
		Mesh(std::unique_ptr<MeshElement> element,
			 const std::shared_ptr<Material>& material);
		Mesh(std::vector<std::unique_ptr<MeshElement>>& elements,
			 const std::vector<std::shared_ptr<Material>>& materials);
		virtual ~Mesh();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> 				name() const;
		void 									name(const std::string& name);

		const std::vector<std::unique_ptr<MeshElement>>&	elements();
		const std::vector<std::shared_ptr<Material>>& 		materials();

		std::shared_ptr<Material> 				firstMaterial() const;
		std::shared_ptr<Material> 				materialNamed(const std::string& name) const;
		void 									addMaterial(std::shared_ptr<Material> material);
		void 									insertMaterial(std::shared_ptr<Material> material,
															   int index);
		void 									removeMaterial(int index);
		void 									replaceMaterial(int index,
																std::shared_ptr<Material> replacement);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 									burnTransform(const glm::mat4& transform,
															  bool normals);

		void 									draw(Renderer& renderer,
														 const glm::mat4& modelMat,
														 const glm::mat4& viewMat,
														 const glm::mat4& projectionMat,
														 const DebugOptions& debugOptions,
														 Stats& stats);

		AABB									aabb(const Node* convertTo = nullptr) const;
		glm::vec3 								extent(const Node* convertTo = nullptr) const;

		MeshDirtyMask 							dirtyMask() const;
		void 									dirtyMask(MeshDirtyMask mask);

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		std::vector<std::unique_ptr<MeshElement>>	_elements;
		std::vector<std::shared_ptr<Material>>		_materials;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>				_name;

		MeshDirtyMask							_dirtyMask;
	};
}


#endif /* Geometry_h */
