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
//#include <cuchar>

#include "glm/glm.hpp"


namespace fastgltf {
	class Asset;
	class Camera;
	class Mesh;
	class Node;
	class Primitive;
}


namespace ae {


	class Camera;
	class Color;
	class Geometry;
	class GeometryElement;
	class Image;
	class Light;
	class Material;
	class MaterialProperty;
	class Node;
	class Scene;


	class GlTFImporter {

	public:

		GlTFImporter(const std::filesystem::path& path);

		std::shared_ptr<Scene> scene();

	private:

		std::shared_ptr<Scene> load();
		void visitGlTFNode(fastgltf::Asset& asset,
						   fastgltf::Node& node,
						   std::shared_ptr<Node> parent);
		std::shared_ptr<Geometry> geometryFromGlFTNode(fastgltf::Asset& asset,
													   fastgltf::Node& node);
		std::shared_ptr<GeometryElement> geometryElementFromGlFTPrimitive(fastgltf::Asset& asset,
																		  fastgltf::Primitive& primitive);
		std::shared_ptr<Material> materialFromGlFTPrimitive(fastgltf::Asset& asset,
															fastgltf::Primitive& primitive);
		std::shared_ptr<Light> lightFromGlTFNode(fastgltf::Asset& asset,
												 fastgltf::Node& node);
		std::shared_ptr<Camera> cameraFromGlTFNode(fastgltf::Asset& asset,
															  fastgltf::Node& node);
		glm::mat4 transformFromGlFTNode(fastgltf::Node& node);
		std::shared_ptr<Color> colorFromGlTFColorArray(std::array<float, 3>& arr);

		std::shared_ptr<Scene> 										_scene;
		std::filesystem::path										_path;
		std::map<std::size_t, std::shared_ptr<Camera>> 				_cameras;
		std::map<std::size_t, std::shared_ptr<Geometry>> 			_geometries;
		std::map<std::size_t, std::map<std::size_t, std::shared_ptr<GeometryElement>>>
																	_geometryElements;
		std::map<std::size_t, std::shared_ptr<Image>> 				_images;
		std::map<std::size_t, std::shared_ptr<Light>> 				_lights;
		std::map<std::size_t, std::shared_ptr<Material>> 			_materials;
		std::map<std::size_t, std::shared_ptr<MaterialProperty>> 	_materialProperties;
	};
}


#endif //AVARA_ENGINE_GLTFIMPORTER_H
