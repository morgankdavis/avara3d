
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <errno.h>
#include <iostream>
#include <jni.h>
#include <memory>

#include <glm/glm.hpp>

#include "ae.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;
using namespace std::placeholders;


struct android_app;


static void renderContextUpdateCallback(RenderContext& renderContext, float time);
static void renderContextWillRenderCallback(RenderContext& renderContext, float time);
static void renderContextDidRenderCallback(RenderContext& renderContext, float time);


void android_main(android_app* app) {

	AE_INIT(app);

	Logger::Level(LOG_LEVEL::TRACE);

	auto renderer = make_shared<OpenGLRenderer>();
	auto activity = make_shared<Activity>(static_pointer_cast<Renderer>(renderer));

	activity->updateCallback(renderContextUpdateCallback);
	activity->willRenderCallback(renderContextWillRenderCallback);
	activity->didRenderCallback(renderContextDidRenderCallback);

	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));

	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, Color::DarkGray());
	auto ambientLightNode = Node::LightNode(ambientLight);
	scene->rootNode()->addChildNode(ambientLightNode);

	auto sphereGeo = make_shared<Sphere>(0.5, 24);
	auto sphereNode = make_shared<Node>();
	sphereGeo->name("sphere");
	sphereNode->geometry(sphereGeo);
	scene->rootNode()->addChildNode(sphereNode);
	sphereNode->position({0.0f, 0.0f, 0.0f});

	auto gridImage = TestImageNamed("plant", string("jpg"));
	auto sphereMaterialProperty = make_shared<MaterialProperty>(gridImage);
	sphereMaterialProperty->wrapS(WRAP_MODE::REPEAT);
	sphereMaterialProperty->wrapT(WRAP_MODE::REPEAT);
	auto sphereMaterial = make_shared<Material>(nullptr, sphereMaterialProperty, nullptr);
	sphereNode->geometry()->addMaterial(sphereMaterial);

	auto background = make_shared<MaterialProperty>(TestCubeImageNamed("nebula1_blue", "png"));
//	auto background = make_shared<MaterialProperty>(Color::Lime());
	scene->background(background);


	activity->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);

	//auto inputManager = activity->inputManager(); // enable it

	activity->scene(scene);
	activity->display(app);
}

void renderContextUpdateCallback(RenderContext& renderContext, float time) {
	AE_LOG->trace("renderContextUpdateCallback({})", time);
}

void renderContextWillRenderCallback(RenderContext& renderContext, float time) {
	AE_LOG->trace("renderContextWillRenderCallback({})", time);
}

void renderContextDidRenderCallback(RenderContext& renderContext, float time) {
	AE_LOG->trace("renderContextDidRenderCallback({})", time);
}
