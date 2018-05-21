//
//  Renderer.cpp
//	avara-engine
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Renderer.h"

#include "GeometryElement.h"
#include "Logger.h"
#include "Material.h"
#include "RenderContext.h"
#include "Scene.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

Renderer::Renderer() {
		
}

Renderer::~Renderer() {
	
}

/**************************************************************************************
     Internal
 **************************************************************************************/

bool Renderer::initialize() {
	return true;
}

void Renderer::beginFrame(const RenderContext& context) {
	m_renderStats = (RenderStats){};
}

void Renderer::endFrame(const RenderContext& context) {
	
}

void Renderer::render(shared_ptr<Scene> scene,
					  const DEBUG_OPTIONS& debugOptions,
					  RenderStats& stats) {
	
	AE_LOG->critical("Renderer::render(<Scene>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<Geometry> geometry,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DEBUG_OPTIONS& debugOptions,
					  RenderStats& stats) {
	
	AE_LOG->critical("Renderer::render(<Geometry>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<GeometryElement> element,
					  Material& material,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DEBUG_OPTIONS& debugOptions,
					  RenderStats& stats) {
	
	AE_LOG->critical("Renderer::render(<GeometryElement>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<LineSet>,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat) {
	
	AE_LOG->critical("Renderer::render(<LineSet>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<PointSet>,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat) {
	
	AE_LOG->critical("Renderer::render(<PointSet>) should be overidden in derived class.");
}

shared_ptr<Image> Renderer::snapshot(const RenderContext& context) const {
	
	AE_LOG->critical("Renderer::snapshot() should be overidden in derived class.");
	return nullptr;
}

RenderStats& Renderer::renderStats() {
	return m_renderStats;
}
