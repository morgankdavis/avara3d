
#ifndef AVARA3D_RENDERRESOURCECACHEOGL_H
#define AVARA3D_RENDERRESOURCECACHEOGL_H

#include <array>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "a3d/render/pipeline/PipelineKey.h"
#include "a3d/render/backend/opengl/gl_types.h"

namespace a3d {

	class Material;
	class MeshElement;
	class Texture;

	struct PipelineOGL {
		PipelineKey 	key =		{};
		gl::uint_t		program = 	0;
	};

	struct MeshElementOGL {
		gl::uint_t 	vao = 				0;
		gl::uint_t 	vbo = 				0;
		gl::uint_t 	ebo = 				0;
		uint32_t 	indexCount = 		0;
		uint32_t 	vertexLayoutKey = 	0;
	};

	static constexpr size_t NUM_MATERIAL_PROPERTY_SLOTS = 4;
	struct MaterialOGL {
		std::array<gl::uint_t,
				NUM_MATERIAL_PROPERTY_SLOTS> 	tex = 				{};
		float 									specularExponent = 	75.0f;
		float 									uvScale = 			1.0f;
	};

	struct TextureOGL {
		gl::uint_t id = 0;
	};

	class RenderResourceCacheOGL {

	public:

		PipelineHandle 						ensurePipeline(const PipelineKey& key);
		const PipelineOGL& 					pipeline(PipelineHandle h) const;
		const MeshElementOGL& 				ensureMeshElement(MeshElement& element,
															   uint32_t vertexLayoutKey);
		const MaterialOGL& 					ensureMaterial(Material& material);
		gl::uint_t 							ensureTexture(Texture& texture);

	private:

		std::unordered_map<
				PipelineKey,
				PipelineHandle,
				PipelineKeyHash> 			_pipelineMap;
		std::vector<PipelineOGL> 			_pipelineList;
		std::unordered_map<MeshElement*,
				MeshElementOGL> 			_meshElementMap;
		std::unordered_map<Material*,
				MaterialOGL> 				_materialMap;
		std::unordered_map<Texture*,
				TextureOGL> 				_textureMap;
	};
}

#endif //AVARA3D_RENDERRESOURCECACHEOGL_H
