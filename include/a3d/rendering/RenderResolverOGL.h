
#ifndef AVARA3D_RENDERRESOLVEROGL_H
#define AVARA3D_RENDERRESOLVEROGL_H

#include "a3d/Types.h"
//#include "a3d/rendering/PipelineKey.h"
#include "a3d/rendering/PipelineKey.h"

namespace a3d {

	class Material;

	class RenderResolverOGL {

	public:

		static PipelineKey ComputePipelineKey(const Material& material,
											  uint32_t vertexLayoutKey);
	};
}

#endif //AVARA3D_RENDERRESOLVEROGL_H
