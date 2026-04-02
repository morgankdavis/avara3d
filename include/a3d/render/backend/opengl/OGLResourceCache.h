//
//  OGLResourceCache.h
//  avara3d
//
//  Created by Morgan Davis on 12/31/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_OGLRESOURCECACHE_H
#define AVARA3D_RENDER_BACKEND_OPENGL_OGLRESOURCECACHE_H

#include <array>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "a3d/render/PipelineKey.h"
#include "a3d/render/backend/opengl/GLTypes.h"

namespace a3d {

	class Material;
	class MeshElement;
	class Texture;

	/// Internal Types ///

	struct OGLPipeline {
		PipelineKey 	key =		{};
		gl::uint_t		program = 	0;
	};

	struct OGLMeshElement {
		gl::uint_t 		vao = 				0;
		gl::uint_t 		vbo = 				0;
		gl::uint_t 		ebo = 				0;
		gl::enum_t 		indexType = 		gl::value::unsigned_int;
		uint32_t 		indexCount = 		0;
		uint32_t        vertexCount =		0;
		VertexLayout 	vertexLayoutKey = 	VertexLayout::None;
	};

	static constexpr size_t NUM_MATERIAL_PROPERTY_SLOTS = 4;
	struct OGLMaterial {
		std::array<gl::uint_t, NUM_MATERIAL_PROPERTY_SLOTS>
		        								tex = 				{};
		float 									specularExponent = 	75.0f;
		float 									uvScale = 			1.0f;
	};

	struct OGLTexture {
		gl::uint_t id = 0;
	};

	class OGLResourceCache {

	public:
		/// Internal Member Functions ///

		PipelineHandle 						ensurePipeline(const PipelineKey& key);
		const OGLPipeline& 					pipeline(PipelineHandle h) const;
		const OGLMeshElement& 				ensureMeshElement(MeshElement& element);
		const OGLMaterial& 					ensureMaterial(Material& material);
		const OGLTexture& 					ensureTexture(Texture& texture);

	private:
		/// Private Member Variables ///

		std::unordered_map<
				PipelineKey,
				PipelineHandle,
				PipelineKeyHash> 			_pipelineMap;
		std::vector<OGLPipeline> 			_pipelineList;
		std::unordered_map<MeshElement*,
				OGLMeshElement> 			_meshElementMap;
		std::unordered_map<Material*,
				OGLMaterial> 				_materialMap;
		std::unordered_map<Texture*,
				OGLTexture> 				_textureMap;
	};
}

#endif //AVARA3D_RENDER_BACKEND_OPENGL_OGLRESOURCECACHE_H
