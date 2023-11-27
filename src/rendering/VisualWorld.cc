//
// Created by mkd on 11/25/23.
//

#include "rendering/VisualWorld.h"


#include "glm/glm.hpp"

#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "geometry/primitives/Box.h"
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

VisualWorld::VisualWorld(std::shared_ptr<RenderContext> context)://, RENDER_API api):
		_renderContext(context),
//		_renderAPI(api),
		_background(nullptr),
		_fogStartDistance(0.0),
		_fogEndDistance(0.0),
		_fogDensityExponent(0.0),
		_fogColor(nullptr),
		_pointOfView(nullptr),
		_willRender(nullptr),
		_didRender(nullptr) {

	_renderContext->visualWorld(this);
}

VisualWorld::~VisualWorld() {
	AE_LOG_D("Destroying VisualWorld {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<Node> VisualWorld::pointOfView() {

	if (_pointOfView) {
		return _pointOfView;
	}
	else {
		// try to assign one from the scene
		for (auto node: _scene->rootNode()->children(true)) {
			if (node->camera()) {
				_pointOfView = node;
				return _pointOfView;
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

shared_ptr<RenderContext> VisualWorld::renderContext() const {
	return _renderContext;
}

void VisualWorld::renderContext(shared_ptr<RenderContext> context) {
	_renderContext = context;
}

//RENDER_API VisualWorld::renderAPI() const {
//	return _renderAPI;
//}

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

//weak_ptr<Scene> VisualWorld::scene() const {
//	return _scene;
//}
//
//void VisualWorld::scene(weak_ptr<Scene> scene) {
//	_scene = scene;
//}

Scene* VisualWorld::scene() const {
	return _scene;
}

void VisualWorld::scene(Scene* scene) {
	_scene = scene;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

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
