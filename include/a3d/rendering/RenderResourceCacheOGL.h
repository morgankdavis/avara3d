
#ifndef AVARA3D_RENDERRESOURCECACHEOGL_H
#define AVARA3D_RENDERRESOURCECACHEOGL_H

#include <unordered_map>
#include <vector>

//#ifdef A3D_GL_ES
//#include <EGL/egl.h>
//#include <GLES3/gl3.h>
//#else
//#include "glad/glad.h"
//#endif

#include "a3d/rendering/PipelineKey.h"

namespace a3d {




//	struct PipelineOGL {
//		GLuint program = 0;
//
//		// fixed-state baked into this pipeline definition
//		bool depthTest = true;
//		bool depthWrite = true;
//		bool doubleSided = false;
//		FillMode fillMode = FillMode::Fill;
//		BlendFunction blend = BlendFunction::Disabled;
//
//		// optionally: vertex layout ID, defines, etc.
//	};


	struct PipelineOGL {
		PipelineKey 	key;
		unsigned 		program = 		0; // GLuint
		bool 			depthTest = 	true;
//		bool 			depthWrite =	true;
	};





	struct MaterialOGL {

		bool doubleSided = false;
		FillMode fillMode = FillMode::Fill;
		BlendFunction blend = BlendFunction::Disabled;

		AlphaMode alphaMode = AlphaMode::Opaque; // NEW (Mask = alpha discard, Blend = real transparency)

		math::vec4 ambient, diffuse, specular, emission;
		float specularExponent = 75.0f;
		float uvScale = 1.0f;

		//TextureHandle diffuseTex = {};

		uint64_t pipelineKey = 0;
	};




	class RenderResourceCacheOGL {

	public:

		PipelineHandle ensurePipeline(const PipelineKey& key);

		const PipelineOGL& pipeline(PipelineHandle h) const {
			return _pipelineList.at(h);
		}

	private:

		PipelineOGL buildPipeline(const PipelineKey& key);

		std::unordered_map<
				PipelineKey,
				PipelineHandle,
				PipelineKeyHash> 	_pipelineMap;
		std::vector<PipelineOGL> 	_pipelineList;
	};
}

#endif //AVARA3D_RENDERRESOURCECACHEOGL_H
