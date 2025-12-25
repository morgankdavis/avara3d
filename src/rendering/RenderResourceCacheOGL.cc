
#include "a3d/rendering/RenderResourceCacheOGL.h"

#include "a3d/rendering/renderer/opengl/Program.h"

using namespace a3d;
using namespace std;

PipelineHandle RenderResourceCacheOGL::ensurePipeline(const PipelineKey& key) {

	if (auto it = _pipelineMap.find(key); it != _pipelineMap.end()) return it->second;

	PipelineOGL p = buildPipeline(key);

	const PipelineHandle h = (PipelineHandle)_pipelineList.size();
	_pipelineList.push_back(std::move(p));
	_pipelineMap.emplace(key, h);

	return h;
}

PipelineOGL RenderResourceCacheOGL::buildPipeline(const PipelineKey& key) {
	PipelineOGL p;
	p.key = key;
	p.key.doubleSided = key.doubleSided;
	p.key.fillMode = key.fillMode;
	p.key.blendFunction = key.blendFunction;
	p.key.depthWrite = true;

	// For now assume depth always on for your main pass:
	p.depthTest  = true;

#warning TEMPORARY
	switch (p.key.shaderKind) {
		case ShaderKind::Wireframe:
			p.program = Program::Wireframe().glID();
			break;
		default:
			p.program = Program::Default().glID();
			break;
	}

	// p.program = compileProgramForKey(key); // you already had this placeholder

	return p;
}
