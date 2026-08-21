//
//  GlTFImporter.h
//  avara3d
//
//  Created by Morgan Davis on 1/30/24.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SCENE_IMPORTER_GLTFIMPORTER_H
#define AVARA3D_SCENE_IMPORTER_GLTFIMPORTER_H

#include <cstddef>
#include <filesystem>
#include <map>
#include <memory>

#include <fastgltf/types.hpp>

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
        // [Internal Lifecycle Functions]

        explicit GlTFImporter(const std::filesystem::path& path,
                              Scene::ImportOptions         options = Scene::ImportOptions::ImportAll);

        // [Internal Member Functions]

        std::unique_ptr<Scene>       scene();
        std::shared_ptr<Mesh>        firstMesh();

        const std::filesystem::path& path() const;
        Scene::ImportOptions         options() const;

    private:
        // [Private Member Functions]

        bool                         parse();
        void                         visitGlTFNode(fastgltf::Asset& asset, fastgltf::Node& node, Node* parent);
        std::shared_ptr<Mesh>        meshFromGlTFNode(fastgltf::Asset& asset, fastgltf::Node& node);
        std::shared_ptr<Mesh>        meshFromGlTFMeshIndex(fastgltf::Asset& asset, std::size_t meshIndex);
        std::unique_ptr<MeshElement> meshElementFromGlTFPrimitive(fastgltf::Asset&     asset,
                                                                  fastgltf::Primitive& primitive);
        std::shared_ptr<Material>    materialFromGlTFPrimitive(fastgltf::Asset&     asset,
                                                               fastgltf::Primitive& primitive);
        std::shared_ptr<Texture> textureFromGlTFTextureIndex(fastgltf::Asset& asset, std::size_t textureIndex);

        std::shared_ptr<Sampler> samplerFromGlTFTexture(fastgltf::Asset& asset, fastgltf::Texture& texture);
        std::shared_ptr<Image>   imageFromGlTFTexture(fastgltf::Asset& asset, fastgltf::Texture& texture);
        std::shared_ptr<Light>   lightFromGlTFNode(fastgltf::Asset& asset, fastgltf::Node& node);
        std::shared_ptr<Camera>  cameraFromGlTFNode(fastgltf::Asset& asset, fastgltf::Node& node);

        // [Private Member Variables]

        bool                     _parsed;
        fastgltf::Asset          _asset;
        std::unique_ptr<Scene>   _scene;
        std::filesystem::path    _path;
        Scene::ImportOptions     _options;
        std::map<std::size_t, std::shared_ptr<Camera>>   _cameras;
        std::map<std::size_t, std::shared_ptr<Mesh>>     _meshes;
        std::map<std::size_t, std::shared_ptr<Image>>    _images;
        std::map<std::size_t, std::shared_ptr<Light>>    _lights;
        std::map<std::size_t, std::shared_ptr<Material>> _materials;
        std::map<std::size_t, std::shared_ptr<Texture>>  _textures;
        std::map<std::size_t, std::shared_ptr<Sampler>>  _samplers;
    };

}

#endif // AVARA3D_SCENE_IMPORTER_GLTFIMPORTER_H
