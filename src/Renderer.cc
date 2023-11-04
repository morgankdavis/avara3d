//
//  Renderer.cc
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


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

Renderer::Renderer():
		_frametimeAveragingInterval(.25) {
		
}

Renderer::~Renderer() {
	AE_LOG_D("Destroying Renderer {:p}", (void*)this);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

bool Renderer::initialize(const RenderContext& context) {
	return true;
}

void Renderer::beginFrame(const RenderContext& context) {
	_renderStats = (RenderStats){};
}

void Renderer::endFrame(const RenderContext& context) {
	updateFrametimeStats(renderStats(), context.sceneTime());
}

void Renderer::render(Scene& scene,
					  const DEBUG_OPTIONS& debugOptions,
					  RenderStats& stats) {
	
	AE_LOG_C("Renderer::render(<Scene>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<Geometry> geometry,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DEBUG_OPTIONS& debugOptions,
					  RenderStats& stats) {
	
	AE_LOG_C("Renderer::render(<Geometry>) should be overidden in derived class.");
}

void Renderer::render(shared_ptr<GeometryElement> element,
					  Material& material,
					  const glm::mat4& modelMat,
					  const glm::mat4& viewMat,
					  const glm::mat4& projectionMat,
					  const DEBUG_OPTIONS& debugOptions,
					  RenderStats& stats) {
	
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

float Renderer::frametimeAveragingInterval() const {
	return _frametimeAveragingInterval;
}

void Renderer::frametimeAveragingInterval(float interval) {
	_frametimeAveragingInterval = interval;
}

shared_ptr<Image> Renderer::snapshot(const RenderContext& context) const {
	
	AE_LOG_C("Renderer::snapshot() should be overidden in derived class.");
	return nullptr;
}

RenderStats& Renderer::renderStats() {
	return _renderStats;
}

void Renderer::updateFrametimeStats(RenderStats& stats, float time) {

	static float fpsAvg = 0.0;
	static float msAvg = 0.0;

	static int elapsedFramesThisSample = 0;

	static float lastSampleStartTime = time;

	float elapsedSecondsSinceLastFrame = time - lastSampleStartTime;

	float timeSinceBeginSample = time - lastSampleStartTime;
	if (timeSinceBeginSample >= frametimeAveragingInterval()) {

		fpsAvg = (float)elapsedFramesThisSample / timeSinceBeginSample;
		msAvg = (elapsedSecondsSinceLastFrame * 1000.0f) / elapsedFramesThisSample;

		elapsedFramesThisSample = 0;
		lastSampleStartTime = time;
	}
	else {
		++elapsedFramesThisSample;
	}

	stats.averageFramerate = fpsAvg;
	stats.averageFrametime = msAvg;

	stats.currentFramerate = 60.0f / elapsedSecondsSinceLastFrame;
	stats.currentFrametime = elapsedSecondsSinceLastFrame * 1000.0f; // is this wrong?

	stats.frametimeAveragingInterval = frametimeAveragingInterval();
}
