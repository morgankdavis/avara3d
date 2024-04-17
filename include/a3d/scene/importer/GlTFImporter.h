//
//  GlTFImporter.h
//  avara3d
//
//  Created by Morgan Davis on 1/30/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_GLTFIMPORTER_H
#define AVARA3D_GLTFIMPORTER_H


#include <array>
#include <cstddef>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>

#include "fastgltf/types.hpp"
#include "glm/glm.hpp"

#include "a3d/Types.h"
#include "a3d/scene/Scene.h"


namespace a3d {


	class Camera;
	class Color;
	class Image;
	class Light;
	class Material;
	class Mesh;
	class MeshElement;
	class Node;
	class Sampler;
	class Texture;


	class GlTFImporter {

	public:
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		explicit GlTFImporter(const std::filesystem::path& path,
							  SceneImportOptions options = SceneImportOptions::ImportAll);

		std::unique_ptr<Scene> 				scene();
		std::shared_ptr<Mesh> 				firstMesh();

		const std::filesystem::path&		path() const;
		SceneImportOptions					options() const;

	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

		bool								parse();
		void 								visitGlTFNode(fastgltf::Asset& asset,
														  fastgltf::Node& node,
														  Node* parent);
		std::shared_ptr<Mesh> 				meshFromGlTFNode(fastgltf::Asset& asset,
															  fastgltf::Node& node);
		std::shared_ptr<Mesh> 				meshFromGlTFMeshIndex(fastgltf::Asset& asset,
																   std::size_t meshIndex);
		std::unique_ptr<MeshElement> 		meshElementFromGlTFPrimitive(fastgltf::Asset& asset,
																		 fastgltf::Primitive& primitive);
		std::shared_ptr<Material> 			materialFromGlTFPrimitive(fastgltf::Asset& asset,
																	   fastgltf::Primitive& primitive);
		std::shared_ptr<Texture> 			textureFromGlTFTextureIndex(fastgltf::Asset& asset,
																		std::size_t textureIndex);

		std::shared_ptr<Sampler> 			samplerFromGlTFTexture(fastgltf::Asset& asset,
																   fastgltf::Texture& texture);
		std::shared_ptr<Image> 				imageFromGlTFTexture(fastgltf::Asset& asset,
																   fastgltf::Texture& texture);
		std::shared_ptr<Light>				lightFromGlTFNode(fastgltf::Asset& asset,
																fastgltf::Node& node);
		std::shared_ptr<Camera> 			cameraFromGlTFNode(fastgltf::Asset& asset,
															  fastgltf::Node& node);

		bool														_parsed;
		fastgltf::Asset												_asset;
		std::unique_ptr<Scene> 										_scene;
		std::filesystem::path										_path;
		SceneImportOptions											_options;
		std::map<std::size_t, std::shared_ptr<Camera>> 				_cameras;
		std::map<std::size_t, std::shared_ptr<Mesh>> 				_meshes;
		std::map<std::size_t, std::shared_ptr<Image>> 				_images;
		std::map<std::size_t, std::shared_ptr<Light>> 				_lights;
		std::map<std::size_t, std::shared_ptr<Material>> 			_materials;
		std::map<std::size_t, std::shared_ptr<Texture>> 			_textures;
		std::map<std::size_t, std::shared_ptr<Sampler>> 			_samplers;
	};
}


#endif //AVARA3D_GLTFIMPORTER_H
