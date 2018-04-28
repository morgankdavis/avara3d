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

bool Renderer::init() {
	
}

void Renderer::render(Scene& scene,
					  unsigned framebufferWidth,
					  unsigned framebufferHeight,
					  const DEBUG_OPTIONS& debugOptions) {
	
	m_renderStats = (RenderStats){};
}

void Renderer::render(Geometry& geometry,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DEBUG_OPTIONS& debugOptions) {
	
	AE_LOG->critical("Renderer::render(<Geometry>) should be overidden in derived class.");
}

void Renderer::render(GeometryElement& geometryElement,
					  Material& material,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DEBUG_OPTIONS& debugOptions) {
	
	AE_LOG->critical("Renderer::render(<GeometryElement>) should be overidden in derived class.");
}

shared_ptr<Image> Renderer::snapshot(unsigned framebufferWidth,
									 unsigned framebufferHeight) const {
	
	AE_LOG->critical("Renderer::snapshot() should be overidden in derived class.");
	return nullptr;
	
}

RenderStats& Renderer::renderStats() {
	return m_renderStats;
}
