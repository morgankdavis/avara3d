//
//  Mesh.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_H
#define AVARA3D_MESH_H


#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "a3d/Types.h"


namespace a3d {


	class Line;
	class Material;
	class MeshElement;
	class Node;
	class Renderer;


	class Mesh {

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static std::shared_ptr<Mesh> 			FromFile(const std::filesystem::path& path,
														 MeshImportOptions options =
														 MeshImportOptions::ImportMaterials);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		Mesh(const std::string& name,
			 std::unique_ptr<MeshElement> element,
			 const std::shared_ptr<Material>& material);
		Mesh(std::unique_ptr<MeshElement> element,
			 const std::shared_ptr<Material>& material);
		Mesh(const std::string& name,
			 std::vector<std::unique_ptr<MeshElement>>& elements,
			 const std::vector<std::shared_ptr<Material>>& materials);
		Mesh(std::vector<std::unique_ptr<MeshElement>>& elements,
			 const std::vector<std::shared_ptr<Material>>& materials);
		virtual ~Mesh();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		std::optional<std::string> 				name() const;
		void 									name(const std::string& name);

		const std::vector<std::unique_ptr<MeshElement>>&	elements();
		const std::vector<std::shared_ptr<Material>>& 		materials();

		const std::shared_ptr<Material>& 		firstMaterial() const;
		const std::shared_ptr<Material>& 		materialNamed(const std::string& name) const;
		void 									addMaterial(const std::shared_ptr<Material>& material);
		void 									insertMaterial(const std::shared_ptr<Material>& material,
															   int index);
		void 									removeMaterial(int index);
		void 									replaceMaterial(int index,
																const std::shared_ptr<Material>& replacement);

/*********************************************************************************************
	Internal Member Functions
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

		const std::vector<Line>&				aabbLines();

		MeshDirtyMask 							dirtyMask() const;
		void 									dirtyMask(MeshDirtyMask mask);

/*********************************************************************************************
	Protected Member Variables
 *********************************************************************************************/

	protected:

		std::vector<std::unique_ptr<MeshElement>>	_elements;
		std::vector<std::shared_ptr<Material>>		_materials;

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

	private:

		Mesh();

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		std::optional<std::string>				_name;
		std::vector<Line>						_aabbLines;
		MeshDirtyMask							_dirtyMask;
	};
}


#endif /* AVARA3D_MESH_H */
