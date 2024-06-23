//
//  VisualWorld.cc
//  avara3d
//
//  Created by Morgan Davis on 11/25/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/VisualWorld.h"

#include <utility>
#include <variant>

#include "glm/glm.hpp"

#include "a3d/Color.h"
#include "a3d/Configuration.h"
#include "a3d/CubeImage.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/physics/bullet/BulletWorldProxy.h"
#include "a3d/rendering/Light.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/rendering/material/Sampler.h"
#include "a3d/rendering/material/Texture.h"
#include "a3d/rendering/camera/PerspectiveCamera.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/renderer/Renderer.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Private Static Non-Member Prototypes
 *********************************************************************************************/

static unique_ptr<Mesh> MakeSkyboxMesh(const MaterialProperty& property);
static void UpdateTimeStats(Stats& stats, double startTime, double endTime);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

VisualWorld::VisualWorld(RenderContext& context):
		_background{},
		_skyboxMesh{},
		_fogStartDistance{0.0},
		_fogEndDistance{0.0},
		_fogDensityExponent{0.0},
		_fogColor{},
		_pointOfView{},
		_usesDefaultLighting{false},
		_autoEnablesDefaultLighting{true},
		_renderContext{&context},
		_scene{},
		_willRender{},
		_didRender{} {

	_renderContext->attachedToVisualWorld(this);
}

VisualWorld::~VisualWorld() {
	A3D_LOG_D("Destroying VisualWorld {:p}", static_cast<void*>(this));

	if (_renderContext) _renderContext->detachedFromVisualWorld(this);
	//renderContext(nullptr);
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

const MaterialProperty& VisualWorld::background() {
	return _background;
}

void VisualWorld::background(const MaterialProperty& background) {

	if (auto* texture = get_if<shared_ptr<Texture>>(&background)) {

		if (dynamic_pointer_cast<CubeImage>((*texture)->contents())) {
			auto material = make_shared<Material>(monostate{}, monostate{}, monostate{}, background);

			auto sampler = (*texture)->sampler();
			sampler->wrapS(WrapMode::ClampToEdge);
			sampler->wrapT(WrapMode::ClampToEdge);
			sampler->wrapR(WrapMode::ClampToEdge);

			// generate the skybox mesh if it hasn't already been
			if (!_skyboxMesh) {
				_skyboxMesh = MakeSkyboxMesh(background);
			}
			else {
				// we already have the mesh, just update its material
				_skyboxMesh->replaceMaterial(0, material);
			}
		}
	}

	_background = background;
}

float VisualWorld::fogStartDistance() const {
	return _fogStartDistance;
}

void VisualWorld::fogStartDistance(float distance) {
	_fogStartDistance = distance;
}

float VisualWorld::fogEndDistance() const {
	return _fogEndDistance;
}

void VisualWorld::fogEndDistance(float distance) {
	_fogEndDistance = distance;
}

float VisualWorld::fogDensityExponent() const {
	return _fogDensityExponent;
}

void VisualWorld::fogDensityExponent(float exponent) {
	_fogDensityExponent = exponent;
}

const shared_ptr<Color>& VisualWorld::fogColor() const {
	return _fogColor;
}

void VisualWorld::fogColor(const shared_ptr<Color>& color) {
	_fogColor = color;
}

weak_ptr<Node> VisualWorld::pointOfView() {

	if (_pointOfView.lock()) {
		return _pointOfView;
	}
	else {
		// try to assign one from the scene
		for (auto& node : _scene->rootNode()->children(true)) {
			if (node->camera()) {
				_pointOfView = node;
				break;
			}
		}
	}
	if (!_pointOfView.lock()) {
		// still no POV. add a default one.
		_pointOfView  = defaultPointOfView();
	}

	return _pointOfView;
}

void VisualWorld::pointOfView(const weak_ptr<Node>& cameraNode) {
	_pointOfView = cameraNode;
}

bool VisualWorld::usesDefaultLighting() const {
	return _usesDefaultLighting;
}

void VisualWorld::usesDefaultLighting(bool enabled) {
	_usesDefaultLighting = enabled;
}

bool VisualWorld::autoEnablesDefaultLighting() const {
	return _autoEnablesDefaultLighting;
}

void VisualWorld::autoEnablesDefaultLighting(bool enabled) {
	_autoEnablesDefaultLighting = enabled;
}

RenderContext* VisualWorld::renderContext() const {
	return _renderContext;
}

Scene* VisualWorld::scene() const {
	return _scene;
}

VisualWorld::WillRenderCallback VisualWorld::willRender() const {
	return _willRender;
}

void VisualWorld::willRender(WillRenderCallback function) {
	_willRender = function;
}

VisualWorld::DidRenderCallback VisualWorld::didRender() const {
	return _didRender;
}

void VisualWorld::didRender(DidRenderCallback function) {
	_didRender = function;
}

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

void VisualWorld::attachedToScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

	_scene = &scene;
}

void VisualWorld::detachedFromScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

	_scene = nullptr;
}

void VisualWorld::draw(const Scene& scene,
					   const PhysicalWorld* physicalWorld,
					   double runT,
					   double deltaRunT,
					   DebugOptions debugOptions,
					   Stats& stats) {

	if (_renderContext) {

		if (auto renderer = _renderContext->renderer()) {

			if (auto willRender = VisualWorld::willRender()) {
				willRender(*this, (float)runT, (float)deltaRunT);
			}

			auto startTime = scene.time();

			renderer->beginFrame(scene, *_renderContext, debugOptions, stats);

			if (auto pov = pointOfView().lock()) {

				stats.cameraPosition = pov->position();

				auto frameBufferSize = _renderContext->framebufferSize();
				auto aspectRatio = float(frameBufferSize.x) / float(frameBufferSize.y);
				dynamic_pointer_cast<PerspectiveCamera>(pov->camera())->aspectRatio(aspectRatio);

				renderer->render(scene, debugOptions, stats);

				auto viewMat = pov->worldTransform();
				auto projectionMat = pov->camera()->projection();
				scene.rootNode()->draw(*renderer,
									   viewMat,
									   projectionMat,
									   debugOptions,
									   stats);
				--stats.nodes; // don't count the root node

				if (physicalWorld) {

					if (auto bulletWorldProxy = dynamic_cast<BulletWorldProxy *>(physicalWorld->proxy())) {
						bulletWorldProxy->drawDebug(*renderer,
													viewMat,
													projectionMat,
													debugOptions);
					}
				}
			}
			else {
				A3D_LOG_W("No point of view!");
			}

			UpdateTimeStats(stats, startTime, scene.time());

			renderer->endFrame(scene, *_renderContext, debugOptions, stats);

			_renderContext->swapBuffers();

			if (auto didRender = VisualWorld::didRender()) {
				didRender(*this, (float)runT, (float)deltaRunT);
			}

			if (_renderContext->recordingGIF()) {
				_renderContext->saveGIFFrame((float)deltaRunT);
			}
		}
		else {
			A3D_LOG_E("No Renderer attached to RenderContext {:p}", static_cast<void*>(_renderContext));
		}
	}
	else {
		A3D_LOG_E("No RenderContext attached to VisualWorld {:p}", static_cast<void*>(this));
	}
}

Mesh* VisualWorld::skyboxMesh() const {
	return _skyboxMesh.get();
}

weak_ptr<Node> VisualWorld::defaultPointOfView() {

	if (_scene) {

		auto cameraNode = make_shared<Node>();
		auto camera = make_shared<PerspectiveCamera>();
		camera->name("default camera");

		auto aabb = _scene->rootNode()->aabb();

		auto fovH = camera->yFov();
		auto frameBufferSize = _renderContext->framebufferSize();
		auto aspectRatio = float(frameBufferSize.x) / float(frameBufferSize.y);
		auto inverseAspectRatio = 1.0f / aspectRatio;
		auto fovV = fovH * inverseAspectRatio;

		// tan(angle) = x/z
		// ztan(angle) = x
		// z = x/tan(angle)

		auto maxZ = abs(aabb.max.z);

		auto xH = abs(aabb.min.x) + abs(aabb.max.x) / 2.0f;
		auto angleH = fovH / 2.0;
		auto zH = xH / tan(angleH);

		auto xV = abs(aabb.min.y) + abs(aabb.max.y) / 2.0f;
		auto angleV = fovV / 2.0;
		auto zV = xV / tan(angleV);

		zH += maxZ;
		zV += maxZ;

		auto z = fmax(zH, zV);
		auto midX = (aabb.min.x + aabb.max.x) / 2.0f;
		auto midY = (aabb.min.y + aabb.max.y) / 2.0f;

		auto eye = vec3(midX, midY, z / 2.0f); // not sure why z is devided by 2.0, but it seems to work better...
		//vec3 eye = vec3(midX, midY, z);

		mat4 viewMat = translate(mat4(1.0f), eye);
		cameraNode->transform(viewMat);

		_scene->rootNode()->addChild(cameraNode);

		cameraNode->camera(camera);

		return cameraNode;
	}
	else {
		A3D_LOG_W("Can't create default camera: scene is null.");
	}

	return {};
}

/*********************************************************************************************
	Private Static Member Functions
 *********************************************************************************************/

static unique_ptr<Mesh> MakeSkyboxMesh(const MaterialProperty& property) {

	auto mesh = make_unique<a3d::Mesh>(make_unique<Box>(1, 1, 1), nullptr);

	auto material = make_shared<Material>(monostate{}, monostate{}, monostate{}, property);
	material->doubleSided(false);
	mesh->addMaterial(material);

	return mesh;
}

void UpdateTimeStats(Stats& stats, double startTime, double endTime) {

	// current
	auto drawTime = endTime - startTime;
	stats.currentDrawtime = drawTime * 1000.0f;

	// average
	static double avg = 0.0;
	static double sampleStartTime = startTime;
	static unsigned drawsSinceSampleStart = 0;
	static double accumulatedDrawTimeSinceSampleStart = 0;
	double elapsedTimeSinceSampleStart = endTime - sampleStartTime;
	if (elapsedTimeSinceSampleStart >= FRAMETIME_AVERAGING_INTERVAL) {

		avg = (accumulatedDrawTimeSinceSampleStart * 1000.0f) / drawsSinceSampleStart;

		sampleStartTime = startTime;
		drawsSinceSampleStart = 0;
		accumulatedDrawTimeSinceSampleStart = 0;
	}
	else {
		++drawsSinceSampleStart;
		accumulatedDrawTimeSinceSampleStart += drawTime;
	}

	stats.averageDrawtime = avg;
}
