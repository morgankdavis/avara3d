//
//  Renderer.cc
//	avara-engine
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "ae/rendering/Renderer.h"

#include "ae/diagnostic/logging/Logger.h"
#include "ae/geometry/GeometryElement.h"
#include "ae/rendering/VisualWorld.h"
#include "ae/rendering/context/RenderContext.h"
#include "ae/rendering/materials/Material.h"
#include "ae/scene/Scene.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

Renderer::Renderer() { }

Renderer::~Renderer() {
	AE_LOG_D("Destroying Renderer {:p}", static_cast<void*>(this));
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
	
	AE_LOG_C("Renderer::render(<Scene>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<Geometry> geometry,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DebugOptions& debugOptions,
					  Stats& stats) {
	
	AE_LOG_C("Renderer::render(<Geometry>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<GeometryElement> element,
					  Material& material,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DebugOptions& debugOptions,
					  Stats& stats) {
	
	AE_LOG_C("Renderer::render(<GeometryElement>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<LineSet> lines,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat) {
	
	AE_LOG_C("Renderer::render(<LineSet>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<PointSet> points,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat) {
	
	AE_LOG_C("Renderer::render(<PointSet>) should be overidden in derived class.");
}

shared_ptr<Image> Renderer::snapshot(const RenderContext& context) const {
	
	AE_LOG_C("Renderer::snapshot() should be overidden in derived class.");
	return nullptr;
}
