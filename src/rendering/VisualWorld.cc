//
// Created by mkd on 11/25/23.
//

#include "rendering/VisualWorld.h"


#include "glm/glm.hpp"

#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "geometry/primitives/Box.h"
#include "physics/PhysicalWorld.h"
#include "physics/PhysicsSimulator.h"
#include "physics/bullet/BulletPhysicsSimulator.h"
#include "rendering/Light.h"
#include "rendering/Renderer.h"
#include "rendering/camera/PerspectiveCamera.h"
#include "rendering/context/RenderContext.h"
#include "rendering/materials/MaterialProperty.h"
#include "scene/Node.h"
#include "scene/Scene.h"
#include "utilities/Color.h"
#include "utilities/CubeImage.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static shared_ptr<Geometry> MakeSkyboxGeometry(shared_ptr<MaterialProperty> materialProperty);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

VisualWorld::VisualWorld(std::shared_ptr<RenderContext> context):
		_background(nullptr),
		_skyboxGeometry(nullptr),
		_fogStartDistance(0.0),
		_fogEndDistance(0.0),
		_fogDensityExponent(0.0),
		_fogColor(nullptr),
		_pointOfView(nullptr),
		_automaticallyAddDefaultLighting(true),
		_renderContext(context),
		_scene(nullptr),
		_willRender(nullptr),
		_didRender(nullptr) {

	_renderContext->attachedToVisualWorld(this);
}

VisualWorld::~VisualWorld() {
	AE_LOG_D("Destroying VisualWorld {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<MaterialProperty> VisualWorld::background() const {
	return _background;
}

void VisualWorld::background(shared_ptr<MaterialProperty> backgroundProperty) {

	if (dynamic_pointer_cast<CubeImage>(backgroundProperty->contents())) {
		auto material = make_shared<Material>(nullptr, nullptr, nullptr, backgroundProperty);

		material->emissive()->wrapS(WRAP_MODE::CLAMP_TO_EDGE);
		material->emissive()->wrapT(WRAP_MODE::CLAMP_TO_EDGE);
		material->emissive()->wrapR(WRAP_MODE::CLAMP_TO_EDGE);

		// generate the skybox geometry if it hasn't already been
		if (!_skyboxGeometry) {
			// MKD: u_s_ptr_aliases
			_skyboxGeometry = MakeSkyboxGeometry(backgroundProperty);
		}
		else {
			// we already have the geometry, just update its material
			_skyboxGeometry->replaceMaterial(0, material);
		}
	}

	_background = backgroundProperty;
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

shared_ptr<Color> VisualWorld::fogColor() const {
	return _fogColor;
}

void VisualWorld::fogColor(shared_ptr<Color> color) {
	_fogColor = color;
}

shared_ptr<Node> VisualWorld::pointOfView() {

	if (_pointOfView) {
		return _pointOfView;
	}
	else {
		// try to assign one from the scene
		for (auto node: _scene->rootNode()->children(true)) {
			if (node->camera()) {
				_pointOfView = node;
				break;
			}
		}
	}
	if (!_pointOfView) {
		// still no POV. add a default one.
		_pointOfView = defaultPointOfView();
	}

	return _pointOfView;
}

void VisualWorld::pointOfView(const shared_ptr<Node> camera) {
	_pointOfView = camera;
}

bool VisualWorld::automaticallyAddDefaultLighting() const {
	return _automaticallyAddDefaultLighting;
}

void VisualWorld::automaticallyAddDefaultLighting(bool enabled) {
	_automaticallyAddDefaultLighting = enabled;
}

shared_ptr<RenderContext> VisualWorld::renderContext() const {
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
	Internal
 *********************************************************************************************/

void VisualWorld::attachedToScene(Scene* scene) {
	_scene = scene;
}

void VisualWorld::checkAddDefaultLighting() {

	if (_automaticallyAddDefaultLighting && _scene) {

		bool hasLights = false;

		for (auto node : _scene->rootNode()->children(true)) {
			if (!node->hidden()) {
				if (node->light()) {
					hasLights = true;
					break;
				}
			}
		}

		if (!hasLights) {
			AE_LOG_I("Adding default lighting.");

			auto ambientNode = Node::LightNode(Light::DefaultAmbient());
			_scene->rootNode()->addChild(ambientNode);

			auto pointNode = Node::LightNode(Light::DefaultPoint());
			// set position based on scene extent...
			auto sceneExtent = _scene->rootNode()->extent();
			pointNode->position({sceneExtent.x + sceneExtent.x/4.0,
								 sceneExtent.y + sceneExtent.y/4.0,
								 sceneExtent.z + sceneExtent.z/4.0});
			_scene->rootNode()->addChild(pointNode);
		}
	}
}

void VisualWorld::draw(const Scene& scene,
					   const PhysicalWorld* physicalWorld,
					   float runT,
					   float deltaRunT,
					   DEBUG_OPTIONS debugOptions,
					   Stats& stats) {

	if (_renderContext) {

		auto renderer = _renderContext->renderer();
		if (renderer) {

			renderer->beginFrame(scene, *_renderContext, debugOptions, stats);

			const auto framebufferWidth = _renderContext->framebufferWidth();
			const auto framebufferHeight = _renderContext->framebufferHeight();

			auto pov = pointOfView();

			auto aspectRatio = (float) framebufferWidth / (float) framebufferHeight;
			static_pointer_cast<PerspectiveCamera>(pov->camera())->aspectRatio(aspectRatio);

			stats.cameraPosition = pov->position();

			if (willRender()) {
				(willRender())(*this, runT);
			}

			renderer->render(scene, debugOptions, stats);

			auto viewMat = pov->worldTransform();
			auto projectionMat = pov->camera()->projection();

			scene.rootNode()->draw(*renderer,
								   viewMat,
								   projectionMat,
								   debugOptions,
								   stats);

			if (physicalWorld) {

				auto physicsSimulator = physicalWorld->simulator();
				auto bulletSimulator = dynamic_pointer_cast<BulletPhysicsSimulator>(physicsSimulator);
				if (bulletSimulator) {
					bulletSimulator->drawDebug(*renderer,
											   viewMat,
											   projectionMat,
											   debugOptions);
				}
			}

			renderer->endFrame(scene, *_renderContext, debugOptions, stats);

			_renderContext->swapBuffers();

			if (_renderContext->recordingGIF()) {
				_renderContext->saveGIFFrame(runT);
			}

			if (didRender()) {
				(didRender())(*this, runT);
			}
		}
		else {
			AE_LOG_E("No Renderer attached to RenderContext {:p}", (void*)_renderContext.get());
		}
	}
	else {
		AE_LOG_E("No RenderContext attached to VisualWorld {:p}", (void*)this);
	}
}

shared_ptr<Geometry> VisualWorld::skyboxGeometry() const {
	return _skyboxGeometry;
}

shared_ptr<Node> VisualWorld::defaultPointOfView() {

	if (_scene) {

		auto cameraNode = make_shared<Node>();
		//_scene->rootNode()->addChild(cameraNode); // done below
		auto camera = make_shared<PerspectiveCamera>();
		camera->name("default camera");
		cameraNode->camera(camera);

		auto aabb = _scene->rootNode()->aabb();

		float fovH = static_pointer_cast<PerspectiveCamera>(cameraNode->camera())->fov();
		float w = _renderContext->width();
		float h = _renderContext->height();
		float aspectRatio = w / h;
		float inverseAspectRatio = 1.0f / aspectRatio;
		float fovV = fovH * inverseAspectRatio;

		// tan(angle) = x/z
		// ztan(angle) = x
		// z = x/tan(angle)

		float maxZ = abs(aabb.max.z);

		float xH = abs(aabb.min.x) + abs(aabb.max.x) / 2.0f;
		float angleH = fovH / 2.0;
		float zH = xH / tan(angleH);

		float xV = abs(aabb.min.y) + abs(aabb.max.y) / 2.0f;
		float angleV = fovV / 2.0;
		float zV = xV / tan(angleV);

		zH += maxZ;
		zV += maxZ;

		float z = fmax(zH, zV);
		float midX = (aabb.min.x + aabb.max.x) / 2.0f;
		float midY = (aabb.min.y + aabb.max.y) / 2.0f;

		vec3 eye = vec3(midX, midY, z / 2.0f); // not sure why z is devided by 2.0, but it seems to work better...
		//vec3 eye = vec3(midX, midY, z);

		mat4 viewMat = translate(mat4(1.0f), eye);
		cameraNode->transform(viewMat);

		_scene->rootNode()->addChild(cameraNode);
		pointOfView(cameraNode);

		return cameraNode;
	}
	else {
		AE_LOG_W("Can't create default camera: scene is null.");
	}

	return nullptr;
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

static shared_ptr<Geometry> MakeSkyboxGeometry(shared_ptr<MaterialProperty> materialProperty) {

	auto geometry = make_shared<Box>(1, 1, 1);
	auto material = make_shared<Material>(nullptr, nullptr, nullptr, materialProperty);
	geometry->insertMaterial(material, 0);

	return geometry;
}
