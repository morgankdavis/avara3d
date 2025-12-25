//
//#include "a3d/rendering/RenderResolver.h"
//
//#include "a3d/rendering/material/Material.h"
//
//using namespace a3d;
//using namespace a3d::math;
//using namespace std;
//
//PipelineKey RenderResolver::ComputePipelineKey(const Material& material,
//												  uint32_t vertexLayoutKey) {
//
//	PipelineKey k{};
//
//	k.vertexLayoutKey = vertexLayoutKey;
//	k.fillMode = material.fillMode();
//	k.blendFunction = material.blendFunction();
//	k.doubleSided = material.doubleSided();
//
////#warning TEMPORARY
////	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowWireframes)) {
////		k.shaderKind = ShaderKind::Wireframe;
////	}
////	else {
//		k.shaderKind = ShaderKind::Default;
////	}
//
//	// example:
////	const bool hasEmission = !std::holds_alternative<std::monostate>(material.emission());
////	k.shaderKind = hasEmission ? ShaderKind::Default : ShaderKind::Default;
//
//	return k;
//}
