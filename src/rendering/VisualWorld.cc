//
//  VisualWorld.cc
//  avara3d
//
//  Created by Morgan Davis on 11/25/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/VisualWorld.h"

#include <variant>

#include "a3d/Color.h"
#include "a3d/CubeImage.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/mesh/primitive/Plane.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/physics/bullet/BulletWorldProxy.h"
#include "a3d/profiling/Profiling.h"
#include "a3d/rendering/RenderGatherer.h"
#include "a3d/rendering/RenderItem.h"
#include "a3d/rendering/RenderPacket.h"
#include "a3d/rendering/RenderResourceCacheOGL.h"
#include "a3d/rendering/light/Light.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/rendering/material/Sampler.h"
#include "a3d/rendering/material/Texture.h"
#include "a3d/rendering/camera/PerspectiveCamera.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/renderer/Renderer.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Private Static Non-Member Prototypes ///

static unique_ptr<Mesh> MakeSkyboxMesh(const MaterialProperty& property);
static unique_ptr<Mesh> GroundPlaneMesh();

/// Public Lifecycle Functions ///

VisualWorld::VisualWorld(RenderContext& context):
		_background{},
		_skyboxMesh{},
		_groundPlaneMesh{},
		_fogStartDistance{0.0},
		_fogEndDistance{0.0},
		_fogDensityExponent{0.0},
		_fogColor{},
		_pointOfView{},
		_usesDefaultLighting{false},
		_autoEnablesDefaultLighting{true},
		_renderContext{&context},
		_scene{},
		_willRenderCallback{},
		_didRenderCallback{} {

	_renderContext->attachedToVisualWorld(this);

	// *** TEMPORARY ***
	_groundPlaneMesh = GroundPlaneMesh();
}

VisualWorld::~VisualWorld() {
	A3D_LOG_D("Destroying VisualWorld {:p}", static_cast<void*>(this));

	if (_renderContext) _renderContext->detachedFromVisualWorld(this);
	//renderContext(nullptr);
}

/// Public Member Functions ///

const MaterialProperty& VisualWorld::background() {
	return _background;
}

void VisualWorld::background(const MaterialProperty& background) {

	if (auto texture = get_if<shared_ptr<Texture>>(&background)) {

		if (auto cubeImage = get_if<shared_ptr<CubeImage>>(&((*texture)->contents()))) {

			auto material = make_shared<Material>(monostate{},
												  monostate{},
												  monostate{},
												  background);

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

//		if (dynamic_pointer_cast<CubeImage>((*texture)->contents())) {
//			auto material = make_shared<Material>(monostate{}, monostate{}, monostate{}, background);
//
//			auto sampler = (*texture)->sampler();
//			sampler->wrapS(WrapMode::ClampToEdge);
//			sampler->wrapT(WrapMode::ClampToEdge);
//			sampler->wrapR(WrapMode::ClampToEdge);
//
//			// generate the skybox mesh if it hasn't already been
//			if (!_skyboxMesh) {
//				_skyboxMesh = MakeSkyboxMesh(background);
//			}
//			else {
//				// we already have the mesh, just update its material
//				_skyboxMesh->replaceMaterial(0, material);
//			}
//		}
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
		A3D_LOG_I("Adding default point of view.");
		_pointOfView  = defaultPOV();
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

VisualWorld::WillRenderCallback VisualWorld::willRenderCallback() const {
	return _willRenderCallback;
}

void VisualWorld::willRenderCallback(WillRenderCallback function) {
	_willRenderCallback = function;
}

VisualWorld::DidRenderCallback VisualWorld::didRenderCallback() const {
	return _didRenderCallback;
}

void VisualWorld::didRenderCallback(DidRenderCallback function) {
	_didRenderCallback = function;
}

/// Internal Member Functions ///

void VisualWorld::attachedToScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

	_scene = &scene;
}

void VisualWorld::detachedFromScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

	_scene = nullptr;
}

#include "glad/glad.h"
#define A3D_GL_CHECK()                           		\
    do {                                              	\
        GLenum err;                                   	\
        while ((err = glGetError()) != GL_NO_ERROR) { 	\
            A3D_LOG_E("GL error 0x{:X}", err); 			\
        }                                             	\
    } while (0);

void VisualWorld::draw(const Scene& scene,
					   const PhysicalWorld* physicalWorld,
					   double runT,
					   double deltaRunT,
					   DebugOptions debugOptions,
					   FrameStats& stats,
					   Profiler& profiler,
					   const FrameStatsHistory& statsHistory) {

	if (!_renderContext) {
		A3D_LOG_E("No RenderContext attached to VisualWorld {:p}", static_cast<void*>(this));
		return;
	}

	auto renderer = _renderContext->renderer();
	if (!renderer) {
		A3D_LOG_E("No Renderer attached to RenderContext {:p}", static_cast<void*>(_renderContext));
		return;
	}

	auto pov = pointOfView().lock();
	if (!pov) {
		A3D_LOG_W("No point of view!");
		return;
	}

	auto povScene = pov->scene();
	if (povScene == nullptr || povScene != &scene) {
		A3D_LOG_W("Point of view not in our scene!");
		return;
	}

	if (auto willRender = VisualWorld::willRenderCallback()) {
		A3D_PROFILE(profiler, Profiler::Tag::Application, [&] {
			willRender(*this, runT, deltaRunT);
		});
	}

	renderer->beginFrame(scene, *_renderContext, debugOptions, stats, profiler);
	_renderContext->beginFrame(scene);

	auto [viewMat, projMat] = A3D_PROFILE(profiler, Profiler::Tag::EngineCpu, [&] {

		auto fbSize = _renderContext->framebufferSize();

		if (auto pc = dynamic_pointer_cast<PerspectiveCamera>(pov->camera())) {
			auto aspectRatio = float(fbSize.x) / float(fbSize.y);
			pc->aspectRatio(aspectRatio);
		}

		return std::tuple{ inverse(pov->worldTransform()), pov->camera()->projection() };
	});

	A3D_PROFILE(profiler, Profiler::Tag::RenderCpu, [&] {

		renderer->render(scene,
						 *_renderContext,
						 viewMat,
						 projMat,
						 debugOptions,
						 stats);

		renderer->preTraversal(scene, *_renderContext, debugOptions, stats);
	});

	auto gatherItems = A3D_PROFILE(profiler, Profiler::Tag::EngineCpu, [&] {

		return RenderGatherer::GatherRenderItems(scene,
												 *_renderContext,
												 viewMat,
												 debugOptions,
												 stats);
	});

	A3D_PROFILE(profiler, Profiler::Tag::RenderCpu, [&] {

		renderer->postTraversal(scene,
								*_renderContext,
								gatherItems.temp_lightNodes,
								debugOptions, stats);


		static RenderResourceCacheOGL _cache{};
		RenderPacket packet = RenderGatherer::BuildRenderPacket(gatherItems,
																debugOptions,
																_cache,
				/*vertexLayoutKey=*/1);

		for (const auto &di: packet.main) {

			renderer->bindPipeline(di.pipeline, _cache);
			renderer->bindMaterial(*di.material);
			renderer->bindMeshElement(*di.element);
			renderer->setPerObject(di.model, viewMat, projMat);
			renderer->drawBound();
		}

		for (const auto &di: packet.wire) {

			renderer->bindPipeline(di.pipeline, _cache);
			// optional: bindMaterial for wire, depending on shader
			renderer->bindMeshElement(*di.element);
			renderer->setPerObject(di.model, viewMat, projMat);
			renderer->drawBound();
		}

		// ! temporary !
		for (const auto &mi: gatherItems.temp_meshInstances) {
			renderer->render(*mi.mesh,
							 *_renderContext,
							 mi.model,
							 viewMat,
							 projMat,
							 debugOptions,
							 stats);
		}
	});

	if (physicalWorld) {

		if (auto bwp = dynamic_cast<BulletWorldProxy*>(physicalWorld->proxy())) {
			bwp->drawDebug(*renderer,
						   *_renderContext,
						   viewMat,
						   projMat,
						   debugOptions);
		}
	}

	_renderContext->endFrame(scene);
	renderer->endFrame(scene, *_renderContext,
					   debugOptions, stats, profiler, statsHistory);

	_renderContext->swapBuffers();

	if (auto didRender = VisualWorld::didRenderCallback()) {
		Timer appTimer(true);
		didRender(*this, runT, deltaRunT);
		profiler.add(Profiler::Tag::Application, appTimer.stop());
	}

	if (_renderContext->recordingGIF()) {
		_renderContext->saveGIFFrame(deltaRunT);
	}
}

Mesh* VisualWorld::skyboxMesh() const {
	return _skyboxMesh.get();
}

Mesh* VisualWorld::groundPlaneMesh() const {
	return _groundPlaneMesh.get();
}

weak_ptr<Node> VisualWorld::defaultPOV() {

	if (!_scene) {
		A3D_LOG_W("Can't create default camera: scene is null.");
		return {};
	}

	auto cameraNode = make_shared<Node>();
	auto camera = make_shared<PerspectiveCamera>();
	camera->name("Default Camera");

	auto aabb = _scene->rootNode()->aabb();

	auto fovH = camera->yFov();
	auto frameBufferSize = _renderContext->framebufferSize();
	auto aspectRatio = float(frameBufferSize.x) / float(frameBufferSize.y);
	auto inverseAspectRatio = 1.0f / aspectRatio;
	auto fovV = fovH * inverseAspectRatio;

	// tan(angle) = x/z
	// ztan(angle) = x
	// z = x/tan(angle)

	auto maxZ = math::abs(aabb.max.z);

	auto xH = math::abs(aabb.min.x) + math::abs(aabb.max.x) / 2.0f;
	auto angleH = fovH / 2.0;
	auto zH = xH / tan(angleH);

	auto xV = math::abs(aabb.min.y) + math::abs(aabb.max.y) / 2.0f;
	auto angleV = fovV / 2.0;
	auto zV = xV / tan(angleV);

	zH += maxZ;
	zV += maxZ;

	auto z = math::max(zH, zV);
	auto midX = (aabb.min.x + aabb.max.x) / 2.0f;
	auto midY = (aabb.min.y + aabb.max.y) / 2.0f;

	// not sure why z is devided by 2.0, but it seems to work better...
	auto eye = vec3(midX, midY, z / 2.0f);
	//vec3 eye = vec3(midX, midY, z);

	mat4 viewMat = translate(mat4(1.0f), eye);
	cameraNode->transform(viewMat);

	_scene->rootNode()->addChild(cameraNode);

	cameraNode->camera(camera);

	return cameraNode;
}

/// Private Static Member Functions ///

unique_ptr<Mesh> MakeSkyboxMesh(const MaterialProperty& property) {

	auto mesh = make_unique<a3d::Mesh>(make_unique<Box>(1, 1, 1), nullptr);

	auto material = make_shared<Material>(monostate{}, monostate{}, monostate{}, property);
	material->doubleSided(false);
	mesh->addMaterial(material);

	return mesh;
}

unique_ptr<Mesh> GroundPlaneMesh() {

	//return make_unique<a3d::Mesh>(make_unique<Box>(0.5, 0.5, 0.5), nullptr);
	return make_unique<a3d::Mesh>(make_unique<Plane>(1.0, 1.0, 1, 1), nullptr);
}
