//
// Created by mkd on 11/25/23.
//

#include "ae/rendering/VisualWorld.h"


#include "glm/glm.hpp"

#include "ae/Color.h"
#include "ae/CubeImage.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/geometry/Geometry.h"
#include "ae/geometry/primitives/Box.h"
#include "ae/physics/PhysicalWorld.h"
#include "ae/physics/bullet/BulletWorldProxy.h"
#include "ae/rendering/Light.h"
#include "ae/rendering/Renderer.h"
#include "ae/rendering/camera/PerspectiveCamera.h"
#include "ae/rendering/context/RenderContext.h"
#include "ae/rendering/materials/MaterialProperty.h"
#include "ae/scene/Node.h"
#include "ae/scene/Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static shared_ptr<Geometry> MakeSkyboxGeometry(shared_ptr<MaterialProperty> materialProperty);
static void UpdateTimeStats(Stats& stats, double startTime, double endTime);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

VisualWorld::VisualWorld(shared_ptr<RenderContext> context):
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
	AE_LOG_D("Destroying VisualWorld {:p}", static_cast<void*>(this));

	if (_renderContext) _renderContext->detachedFromVisualWorld(this);
	//renderContext(nullptr);
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

		material->emission()->wrapS(WrapMode::ClampToEdge);
		material->emission()->wrapT(WrapMode::ClampToEdge);
		material->emission()->wrapR(WrapMode::ClampToEdge);

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
		_pointOfView  = defaultPointOfView();
	}

	return _pointOfView;
}

void VisualWorld::pointOfView(const shared_ptr<Node> cameraNode) {
	_pointOfView = cameraNode;
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
	AE_LOG_T("scene: {:p}", static_cast<void*>(scene));

	_scene = scene;
}

void VisualWorld::detachedFromScene(Scene* scene) {
	AE_LOG_T("scene: {:p}", static_cast<void*>(scene));

	_scene = nullptr;
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
					   double runT,
					   double deltaRunT,
					   DebugOptions debugOptions,
					   Stats& stats) {

	if (_renderContext) {

		if (auto renderer = _renderContext->renderer()) {

			if (auto willRender = VisualWorld::willRender()) {
				willRender(*this, runT);
			}

			auto startTime = Scene::Time();

			renderer->beginFrame(scene, *_renderContext, debugOptions, stats);

			auto pov = pointOfView();
			stats.cameraPosition = pov->position();

			auto aspectRatio = (float)_renderContext->framebufferWidth()
							   / (float)_renderContext->framebufferHeight();
			static_pointer_cast<PerspectiveCamera>(pov->camera())->aspectRatio(aspectRatio);

			renderer->render(scene, debugOptions, stats);

			auto viewMat = pov->worldTransform();
			auto projectionMat = pov->camera()->projection();
			scene.rootNode()->draw(*renderer,
								   viewMat,
								   projectionMat,
								   debugOptions,
								   stats);

			if (physicalWorld) {

				if (auto bulletWorldProxy = dynamic_cast<BulletWorldProxy*>(physicalWorld->proxy())) {
					bulletWorldProxy->drawDebug(*renderer,
												viewMat,
												projectionMat,
												debugOptions);
				}
			}

			UpdateTimeStats(stats, startTime, Scene::Time());

			renderer->endFrame(scene, *_renderContext, debugOptions, stats);

			_renderContext->swapBuffers();

			if (auto didRender = VisualWorld::didRender()) {
				didRender(*this, runT);
			}

			if (_renderContext->recordingGIF()) {
				_renderContext->saveGIFFrame(deltaRunT);
			}
		}
		else {
			AE_LOG_E("No Renderer attached to RenderContext {:p}", static_cast<void*>(_renderContext.get()));
		}
	}
	else {
		AE_LOG_E("No RenderContext attached to VisualWorld {:p}", static_cast<void*>(this));
	}
}

shared_ptr<Geometry> VisualWorld::skyboxGeometry() const {
	return _skyboxGeometry;
}

shared_ptr<Node> VisualWorld::defaultPointOfView() {

	if (_scene) {

		auto cameraNode = make_shared<Node>();
		auto camera = make_shared<PerspectiveCamera>();
		camera->name("default camera");
		cameraNode->camera(camera);

		auto aabb = _scene->rootNode()->aabb();

		float fovH = static_pointer_cast<PerspectiveCamera>(camera)->yFov();
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

	auto geometry = make_shared<Box>(1, 1, 1, 1, 1, 1);
	auto material = make_shared<Material>(nullptr, nullptr, nullptr, materialProperty);
	material->doubleSided(false);
	geometry->addMaterial(material);

	return geometry;
}

void UpdateTimeStats(Stats& stats, double startTime, double endTime) {

	// current
	auto drawTime = endTime - startTime;
	stats.currentDrawtime = drawTime * 1000.0f;



	static const double FRAMETIME_AVERAGING_INTERVAL = .25; // TEMPORARY

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
//	stats.averagingInterval = FRAMETIME_AVERAGING_INTERVAL;
}