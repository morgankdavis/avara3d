//
//  VisualWorld.cc
//  avara3d
//
//  Created by Morgan Davis on 11/25/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/VisualWorld.h"

#include <variant>

#include "a3d/Color.h"
#include "a3d/CubeImage.h"
#include "a3d/log/Log.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/profile/Profile.h"
#include "a3d/render/DrawPacket.h"
#include "a3d/render/DrawPacketizer.h"
#include "a3d/render/GatherOutput.h"
#include "a3d/render/RenderGatherer.h"
#include "a3d/render/Renderer.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/Flow.h"
#include "a3d/visual/material/Sampler.h"
#include "a3d/visual/material/Texture.h"
#include "a3d/visual/camera/PerspectiveCamera.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Lifecycle Functions ///

VisualWorld::VisualWorld(RenderContext& context):
		_background{},
		_backgroundMaterial{},
		_fogStartDistance{0.0f},
		_fogEndDistance{0.0f},
		_fogDensityExponent{0.0f},
		_fogColor{},
		_usesDefaultLighting{false},
		_autoEnablesDefaultLighting{true},
		_pointOfView{},
		_renderContext{&context},
		_scene{},
		// _processRenderCommandsCallback{},
		// _renderFrameCallback{}
		_didBeginFrameCallback{} {

	_renderContext->attachedToVisualWorld(this);
}

VisualWorld::~VisualWorld() {
	log::d()("Destroying VisualWorld {:p}", static_cast<void*>(this));

	if (_renderContext) _renderContext->detachedFromVisualWorld(this);
}

/// Public Member Functions ///

const Material::Property& VisualWorld::background() {
	return _background;
}

void VisualWorld::background(const Material::Property& background) {

	// TODO: check equality?

//	_dirtyMask = util::bitmask::add(_dirtyMask, VisualWorldDirtyMask::Background);

	if (auto texture = get_if<shared_ptr<Texture>>(&background)) {

		if (auto cubeImage = get_if<shared_ptr<CubeImage>>(&((*texture)->contents()))) {

			auto sampler = (*texture)->sampler();
			sampler->wrapS(Sampler::WrapMode::ClampToEdge);
			sampler->wrapT(Sampler::WrapMode::ClampToEdge);
			sampler->wrapR(Sampler::WrapMode::ClampToEdge);

			_backgroundMaterial = make_unique<Material>(monostate{},
														monostate{},
														monostate{},
														background);
		}
		else if (auto image = get_if<shared_ptr<Image>>(&((*texture)->contents()))) {
			log::w()("Image background not supported.");
			_backgroundMaterial = nullptr;
		}
	}
	else if (auto color = get_if<shared_ptr<Color>>(&background)) {
		_backgroundMaterial = make_unique<Material>(monostate{},
													monostate{},
													monostate{},
													background);
	}
	else {
		_backgroundMaterial = nullptr;
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

/// Internal Member Functions ///

void VisualWorld::attachedToScene(Scene& scene) {
	log::t()("scene: {:p}", static_cast<void*>(&scene));

	_scene = &scene;
}

void VisualWorld::detachedFromScene(Scene& scene) {
	log::t()("scene: {:p}", static_cast<void*>(&scene));

	_scene = nullptr;
}

VisualWorld::DidBeginFrameCallback VisualWorld::didBeginFrameCallback() const {
	return _didBeginFrameCallback;
}

void VisualWorld::didBeginFrameCallback(DidBeginFrameCallback function) {
	_didBeginFrameCallback = function;
}

bool VisualWorld::draw(const Scene& scene,
					   const PhysicsWorld* physicsWorld,
					   const RenderInfo& info,
					   Scene::DebugOptions debugOptions,
					   FrameStats& stats,
					   Profiler& profiler,
					   const FrameStatsHistory& statsHistory) {
	if (!util::flow::edge_guard(_renderContext, [&] {
		log::e()("No RenderContext attached to VisualWorld {:p}", static_cast<void *>(this));
	})) return false;

	auto renderer = _renderContext->renderer();
	if (!util::flow::edge_guard(renderer, [&] {
		log::e()("No Renderer attached to RenderContext {:p}", static_cast<void*>(_renderContext));
	})) return false;

	util::flow::once([&] { firstDraw(); });

	// auto pov = pointOfView().lock();
	// if (!util::flow::edge_guard(pov, [&] {
	// 	log::e()("No point of view!");
	// 	renderer->clear(Renderer::ClearCommand{}, *_renderContext);
	// 	_renderContext->swapBuffers();
	// })) return false;
	//
	// auto povScene = pov->scene();
	// if (!util::flow::edge_guard(povScene && povScene == &scene, [&] {
	// 	log::e()("Point of view not in our scene!");
	// 	renderer->clear(Renderer::ClearCommand{}, *_renderContext);
	// 	_renderContext->swapBuffers();
	// })) return false;

	prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {
		// there is some "RenderCpu" type stuff bundled in here for GLFWWindow and QtViewport
		_renderContext->beginFrame(scene);
	});

	prof::profile(profiler, Profiler::Tag::RenderCpu, [&] {
		renderer->beginFrame(scene, *_renderContext, debugOptions, stats, profiler);
	});

	if (auto didBeginFrame = didBeginFrameCallback()) {
		prof::profile(profiler, Profiler::Tag::Application, [&] {
			didBeginFrame(*this, info);
		});
	}

	auto pov = pointOfView().lock();
	auto camera = pov ? pov->camera() : nullptr;
	bool povValid = true;

	if (!pov) {
		log::e()("No point of view!");
		povValid = false;
	}
	else if (pov->scene() != &scene) {
		log::e()("Point of view not in our scene!");
		povValid = false;
	}
	else if (!camera) {
		log::e()("Point of view has no camera!");
		povValid = false;
	}

	if (povValid) {

		auto [view, proj] = prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {

			if (auto pc = dynamic_pointer_cast<PerspectiveCamera>(pov->camera())) {
				auto fbSize = _renderContext->framebufferSize();
				auto aspect = float(fbSize.x) / float(fbSize.y);
				pc->aspectRatio(aspect);
			}

			return std::tuple{ inverse(pov->worldTransform()), pov->camera()->projection() };
		});

		prof::profile(profiler, Profiler::Tag::RenderCpu, [&] {

			renderer->preTraversal(scene, *_renderContext, debugOptions, stats);

			auto gatherItems = RenderGatherer::Gather(scene,
													  view,
													  physicsWorld,
													  debugOptions,
													  stats);

			renderer->postTraversal(scene,
									*_renderContext,
									gatherItems.lightNodes,
									debugOptions,
									stats);

			auto packet = DrawPacketizer::Packetize(gatherItems);

			Renderer::FrameParams params = { *_renderContext,
											 view,
											 proj,
											 debugOptions,
											 &stats,
											 &profiler };

			renderer->renderPacket(packet, params);
		});
	}
	else {
		renderer->clear(Renderer::ClearCommand{}, *_renderContext);
	}

	prof::profile(profiler, Profiler::Tag::RenderCpu, [&] {
		renderer->endFrame(scene, *_renderContext,
		                   debugOptions, stats, profiler, statsHistory);
	});

	prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {
		// there is some "RenderCpu" type stuff bundled in here for GLFWWindow and QtViewport
		_renderContext->endFrame(scene);
	});

	prof::profile(profiler, Profiler::Tag::RenderCpu, [&] {
		_renderContext->swapBuffers();
	});

	prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {
		if (_renderContext->recordingGIF()) {
			_renderContext->saveGIFFrame(info.updateDeltaTime);
		}
	});

	return true;
}

shared_ptr<Material> VisualWorld::backgroundMaterial() {
	return _backgroundMaterial;
}

/// Private Member Functions ///

void VisualWorld::firstDraw() {

	// check for or create a POV

	if (!_pointOfView.lock()) {

		// try to assign a POV from the scene
		for (auto &node: _scene->rootNode()->children(true)) {
			if (node->camera()) {
				log::i()("Setting {:p} as POV.", static_cast<void*>(node.get()));
				_pointOfView = node;
				break;
			}
		}
	}

	if (!_pointOfView.lock()) {

		// still no POV. add a default one.
		log::i()("Adding default POV.");
		auto pov = defaultPOV();
		_scene->rootNode()->addChild(pov);
		_pointOfView = pov;
	}
}

shared_ptr<Node> VisualWorld::defaultPOV() {

	if (!_scene) {
		log::w()("Can't create default camera: scene is null.");
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
