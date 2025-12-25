
#ifndef AVARA3D_RENDERRESOURCECACHEOGL_H
#define AVARA3D_RENDERRESOURCECACHEOGL_H

#include <unordered_map>
#include <vector>

#ifdef A3D_GL_ES
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif

#include "a3d/rendering/PipelineKey.h"
#include "a3d/rendering/RenderResolver.h"

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
		GLuint 			program = 		0;
		bool 			depthTest = 	true;
//		bool 			depthWrite =	true;
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
