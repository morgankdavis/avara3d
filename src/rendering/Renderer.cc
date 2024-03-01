//
//  Renderer.cc
//	avara3d
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/Renderer.h"

#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

Renderer::Renderer() { }

Renderer::~Renderer() {
	A3D_LOG_D("Destroying Renderer {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

bool Renderer::initialize(const RenderContext& context) {
	return true;
}

void Renderer::beginFrame(const Scene& scene,
						  const RenderContext& context,
						  const DebugOptions& debugOptions,
						  Stats& stats) {
}

void Renderer::endFrame(const Scene& scene,
						const RenderContext& context,
						const DebugOptions& debugOptions,
						Stats& stats) {
}

void Renderer::render(const Scene& scene,
					  const DebugOptions& debugOptions,
					  Stats& stats) {
	
	A3D_LOG_C("Renderer::render(<Scene>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<Mesh> mesh,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DebugOptions& debugOptions,
					  Stats& stats) {
	
	A3D_LOG_C("Renderer::render(<Mesh>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<MeshElement> element,
					  Material& material,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DebugOptions& debugOptions,
					  Stats& stats) {
	
	A3D_LOG_C("Renderer::render(<MeshElement>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<LineSet> lines,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat) {
	
	A3D_LOG_C("Renderer::render(<LineSet>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<PointSet> points,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat) {
	
	A3D_LOG_C("Renderer::render(<PointSet>) should be overidden in derived class.");
}

shared_ptr<Image> Renderer::snapshot(const RenderContext& context) const {
	
	A3D_LOG_C("Renderer::snapshot() should be overidden in derived class.");
	return nullptr;
}
