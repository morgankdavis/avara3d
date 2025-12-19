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

#include "a3d/Math.h"
#include "a3d/Types.h"

namespace a3d {

	class Line;
	class Material;
	class MeshElement;
	class Node;
	class Renderer;
	class RenderContext;
	class RenderItem;

	class Mesh {

	public:
		/// Public Static Member Functions ///

		static std::shared_ptr<Mesh> 			FromFile(const std::filesystem::path& path,
														 MeshImportOptions options =
														 MeshImportOptions::ImportMaterials);

		/// Public Lifecycle Functions ///

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

		/// Public Member Functions ///

		std::optional<std::string> 	name() const;
		void 						name(const std::string& name);

		const std::vector<std::unique_ptr<MeshElement>>&	elements();
		const std::vector<std::shared_ptr<Material>>& 		materials();

		std::shared_ptr<Material> 	firstMaterial() const;
		std::shared_ptr<Material> 	materialNamed(const std::string& name) const;
		void 						addMaterial(const std::shared_ptr<Material>& material);
		void 						insertMaterial(const std::shared_ptr<Material>& material,
												   int index);
		void 						removeMaterial(int index);
		void 						replaceMaterial(int index,
													const std::shared_ptr<Material>& replacement);

		/// Internal Member Functions ///

		void 						burnTransform(const math::mat4& transform,
												  bool normals);

		void 						gather(std::vector<RenderItem>& items,
										   math::mat4& model,
										   FrameStats& stats);

		void 						draw(Renderer& renderer,
										 const RenderContext& context,
										 const math::mat4& modelMat,
										 const math::mat4& viewMat,
										 const math::mat4& projectionMat,
										 const DebugOptions& debugOptions,
										 FrameStats& stats);

		AABB						obb() const;
		void						genLocalAABB();
		AABB						localAABB() const;
		AABB						worldAABB(const AABB& local,
											  const math::mat4& worldMat,
											  bool vertfit) const;
//		math::vec3 					extent(const Node* convertTo = nullptr) const;

		std::vector<Line>			obbLines();
		std::vector<Line>			worldAabbLines(const math::mat4& worldMat, bool vertfit);

		MeshDirtyMask 				dirtyMask() const;
		void 						dirtyMask(MeshDirtyMask mask);

	protected:
		/// Protected Member Variables ///

		std::vector<std::unique_ptr<MeshElement>>	_elements;
		std::vector<std::shared_ptr<Material>>		_materials;

	private:
		/// Private Lifecycle Functions ///

		Mesh();

		/// Private Member Variables ///

		std::optional<std::string>	_name;
//		std::vector<Line>			_aabbLines;
		MeshDirtyMask				_dirtyMask;

		AABB						_localAABB;
	};
}

#endif /* AVARA3D_MESH_H */
