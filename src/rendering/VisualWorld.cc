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
#include "a3d/Utilities.h"
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


#warning TEMPORARY
#include "a3d/rendering/renderer/opengl/OpenGLRenderer.h"



using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Private Static Non-Member Prototypes ///

static unique_ptr<Mesh> MakeSkyboxMesh(const MaterialProperty& property);

/// Public Lifecycle Functions ///

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
		_willRenderCallback{},
		_didRenderCallback{} {

	_renderContext->attachedToVisualWorld(this);
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

weak_ptr<Node>& VisualWorld::pointOfView() {
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








// TODO: MOVE
//static inline uint8_t passOrder(const DrawItem& it) {
//	// 0 = main opaque, 1 = main transparent, 2 = wire overlay
//	if (it.pass == PassKind::Wire) return 2;
//	return it.transparent ? 1 : 0;
//}
//
//static inline float computeDepth(const DrawItem& it,
//								 const math::vec3& camPos,
//								 const math::vec3& camFwd) {
//	// Use world-space center of item bounds (or model * localCenter)
//	const math::vec3 c = it.aabb.center();
//	return dot(camFwd, (c - camPos)); // larger => farther
//}




void VisualWorld::draw(const Scene& scene,
					   const PhysicalWorld* physicalWorld,
					   double runT,
					   double deltaRunT,
					   DebugOptions debugOptions,
					   FrameStats& stats,
					   Profiler& profiler,
					   const FrameStatsHistory& statsHistory) {

	A3D_EDGE_GUARD(!_renderContext, return;, [&] {
		A3D_LOG_E("No RenderContext attached to VisualWorld {:p}", static_cast<void *>(this));
	});

	auto renderer = _renderContext->renderer();
	A3D_EDGE_GUARD(!renderer, return;, [&] {
		A3D_LOG_E("No Renderer attached to RenderContext {:p}", static_cast<void*>(_renderContext));
	});

	A3D_ONCE([&] {
		firstDraw();
	});

	auto pov = pointOfView().lock();
	A3D_EDGE_GUARD(!pov, return;, [&] {
		A3D_LOG_E("No point of view!");
		renderer->blank();
		_renderContext->swapBuffers();
	});

	auto povScene = pov->scene();
	A3D_EDGE_GUARD(povScene == nullptr || povScene != &scene, return;, [&] {
		A3D_LOG_W("Point of view not in our scene!");
		renderer->blank();
		_renderContext->swapBuffers();
	});

	if (auto willRender = VisualWorld::willRenderCallback()) {
		A3D_PROFILE(profiler, Profiler::Tag::Application, [&] {
			willRender(*this, runT, deltaRunT);
		});
	}

	renderer->beginFrame(scene, *_renderContext, debugOptions, stats, profiler);
	_renderContext->beginFrame(scene);

	auto [view, proj] = A3D_PROFILE(profiler, Profiler::Tag::EngineCpu, [&] {

		if (auto pc = dynamic_pointer_cast<PerspectiveCamera>(pov->camera())) {
			auto fbSize = _renderContext->framebufferSize();
			auto aspect = float(fbSize.x) / float(fbSize.y);
			pc->aspectRatio(aspect);
		}

		return std::tuple{ inverse(pov->worldTransform()), pov->camera()->projection() };
	});

	// TODO: REMOVE
	A3D_PROFILE(profiler, Profiler::Tag::RenderCpu, [&] {

		renderer->render(scene,
						 *_renderContext,
						 view,
						 proj,
						 debugOptions,
						 stats);

		renderer->preTraversal(scene, *_renderContext, debugOptions, stats);
	});

	auto gatherItems = A3D_PROFILE(profiler, Profiler::Tag::EngineCpu, [&] {

		return RenderGatherer::GatherRenderItems(scene,
												 *_renderContext,
												 view,
												 debugOptions,
												 stats);
	});

	A3D_PROFILE(profiler, Profiler::Tag::RenderCpu, [&] {

		renderer->postTraversal(scene,
								*_renderContext,
								gatherItems.temp_lightNodes,
								debugOptions, stats);


#warning TEMPORARY
		auto& cache = (static_cast<OpenGLRenderer*>(renderer))->cache();

		RenderPacket packet = RenderGatherer::BuildRenderPacket(gatherItems,
																cache,
				/*vertexLayoutKey=*/1);

		for (const auto &di: packet.main) {

			renderer->bindPipeline(di.pipeline, cache);
			renderer->bindMaterial(*di.material);
			renderer->bindMeshElement(*di.element);
			renderer->setPerObject(di.model, view, proj);
			renderer->drawBound();
		}

		for (const auto &di: packet.wire) {

			renderer->bindPipeline(di.pipeline, cache);
			// bindMaterial
			renderer->bindMeshElement(*di.element);
			renderer->setPerObject(di.model, view, proj);
			renderer->drawBound();
		}

		// TODO: REMOVE -- ONLY used for AABB drawing
		for (const auto &mi: gatherItems.temp_meshInstances) {
			renderer->render(*mi.mesh,
							 *_renderContext,
							 mi.model,
							 view,
							 proj,
							 debugOptions,
							 stats);
		}
	});

	if (physicalWorld) {

		if (auto bwp = dynamic_cast<BulletWorldProxy*>(physicalWorld->proxy())) {
			bwp->drawDebug(*renderer,
						   *_renderContext,
						   view,
						   proj,
						   debugOptions);
		}
	}

	_renderContext->endFrame(scene);
	renderer->endFrame(scene, *_renderContext,
					   debugOptions, stats, profiler, statsHistory);

	_renderContext->swapBuffers();

	if (auto didRender = VisualWorld::didRenderCallback()) {
		A3D_PROFILE(profiler, Profiler::Tag::Application, [&] {
			didRender(*this, runT, deltaRunT);
		});
	}

	if (_renderContext->recordingGIF()) {
		_renderContext->saveGIFFrame(deltaRunT);
	}
}

Mesh* VisualWorld::skyboxMesh() const {
	return _skyboxMesh.get();
}

/// Private Member Functions ///

void VisualWorld::firstDraw() {

	// check for or create a POV

	if (!_pointOfView.lock()) {

		// try to assign a POV from the scene
		for (auto &node: _scene->rootNode()->children(true)) {
			if (node->camera()) {
				A3D_LOG_I("Setting {:p} as POV.", static_cast<void*>(node.get()));
				_pointOfView = node;
				break;
			}
		}
	}

	if (!_pointOfView.lock()) {

		// still no POV. add a default one.
		A3D_LOG_I("Adding default POV.");
		auto pov = defaultPOV();
		_scene->rootNode()->addChild(pov);
		_pointOfView = pov;
	}
}

shared_ptr<Node> VisualWorld::defaultPOV() {

	if (!_scene) {
		A3D_LOG_W("Can't create default camera: scene is null.");
		return {};
	}

	auto camera = make_shared<PerspectiveCamera>();
	camera->name("Default Camera");

	auto aabb = _scene->rootNode()->aabb();
	vec3 center  = (aabb.min + aabb.max) * 0.5f;
	vec3 extents = (aabb.max - aabb.min) * 0.5f;

	auto fbSize = _renderContext->framebufferSize();
	float aspect = float(fbSize.x) / float(fbSize.y);

	float vFov = camera->yFov();
	float hFov = 2.0f * math::atan(math::tan(vFov * 0.5f) * aspect);

	float distH = extents.x / math::tan(hFov * 0.5f);
	float distV = extents.y / math::tan(vFov * 0.5f);

	float dist = math::max(distH, distV) + extents.z;

	vec3 eye = center + vec3(0, 0, dist);
	mat4 view = math::look_at(eye, center, vec3(0, 1, 0));

	auto cameraNode = make_shared<Node>();
	cameraNode->transform(inverse(view));

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
