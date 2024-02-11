//
// Created by mkd on 1/30/24.
//

#ifndef AVARA_ENGINE_GLTFIMPORTER_H
#define AVARA_ENGINE_GLTFIMPORTER_H


#include <array>
#include <cstddef>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>

#include "fastgltf/types.hpp"
#include "glm/glm.hpp"

#include "ae/Types.h"
#include "ae/rendering/material/Material.h"


namespace ae {


	class Camera;
	class Color;
	class Geometry;
	class GeometryElement;
	class Image;
	class Light;
	class Material;
	class Node;
	class Sampler;
	class Scene;
	class Texture;


	class GlTFImporter {

	public:

		GlTFImporter(const std::filesystem::path& path,
					 SceneImportOptions options = SceneImportOptions::ImportAll);

		std::shared_ptr<Scene> 				scene();
		std::shared_ptr<Geometry> 			firstGeometry();

		const std::filesystem::path&		path() const;
		SceneImportOptions					options() const;

	private:

		bool								parse();
		void 								visitGlTFNode(fastgltf::Asset& asset,
														  fastgltf::Node& node,
														  std::shared_ptr<Node> parent);
		std::shared_ptr<Geometry> 			geometryFromGlFTNode(fastgltf::Asset& asset,
																  fastgltf::Node& node);
		std::shared_ptr<Geometry> 			geometryFromGlFTMeshIndex(fastgltf::Asset& asset,
																	   std::size_t meshIndex);
		std::shared_ptr<GeometryElement> 	geometryElementFromGlFTPrimitive(fastgltf::Asset& asset,
																			 fastgltf::Primitive& primitive);
		std::shared_ptr<Material> 			materialFromGlFTPrimitive(fastgltf::Asset& asset,
																	   fastgltf::Primitive& primitive);
		std::shared_ptr<Image> 				imageFromGlTFTexture(fastgltf::Asset& asset,
																   fastgltf::Texture& texture);
//		std::shared_ptr<Material::Property> materialPropertyFromGlTFTexture(fastgltf::Asset& asset,
//																			fastgltf::Texture& texture);
		std::shared_ptr<Light> 				lightFromGlTFNode(fastgltf::Asset& asset,
																fastgltf::Node& node);
		std::shared_ptr<Camera> 			cameraFromGlTFNode(fastgltf::Asset& asset,
															  fastgltf::Node& node);



		std::shared_ptr<Texture> 			textureFromGlTFTextureIndex(fastgltf::Asset& asset,
																		std::size_t textureIndex);

		std::shared_ptr<Sampler> 			samplerFromGlTFTexture(fastgltf::Asset& asset,
																   fastgltf::Texture& texture);
											// ^^ imageFromGlTFTexture()




		bool														_parsed;
		fastgltf::Asset												_asset;
		std::shared_ptr<Scene> 										_scene;
		std::filesystem::path										_path;
		SceneImportOptions											_options;
		// TODO: switch these to vectors resized from asset?
		std::map<std::size_t, std::shared_ptr<Camera>> 				_cameras;
		std::map<std::size_t, std::shared_ptr<Geometry>> 			_geometries;
		std::map<std::size_t, std::shared_ptr<Image>> 				_images;
		std::map<std::size_t, std::shared_ptr<Light>> 				_lights;
		std::map<std::size_t, std::shared_ptr<Material>> 			_materials;
		//std::map<std::size_t, std::shared_ptr<Material::Property>> 	_materialProperties;


		std::map<std::size_t, std::shared_ptr<Sampler>> 			_samplers;
		std::map<std::size_t, std::shared_ptr<Texture>> 			_textures;
	};
}


#endif //AVARA_ENGINE_GLTFIMPORTER_H
