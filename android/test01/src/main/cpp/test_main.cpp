
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <errno.h>
#include <iostream>
#include <jni.h>
#include <memory>

#include <glm/glm.hpp>
#include <NDKHelper.h>

#include "ae.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


struct android_app;


void android_main(android_app* app) {

	Logger::Init();
	Logger::Level(spdlog::level::trace);


	ndk_helper::JNIHelper::Init(app->activity, "com/mkdinteractive/helper/NDKHelper");




	auto renderer = make_shared<OpenGLRenderer>();
	auto activity = make_shared<Activity>(static_pointer_cast<Renderer>(renderer));

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

	//auto gridImage = TestImageNamed("grid10_512");
	auto gridImage = TestImageNamed("plant", string("jpg"));
	auto sphereMaterialProperty = make_shared<MaterialProperty>(gridImage);
	sphereMaterialProperty->wrapS(WRAP_MODE::REPEAT);
	sphereMaterialProperty->wrapT(WRAP_MODE::REPEAT);
	auto sphereMaterial = make_shared<Material>(nullptr, sphereMaterialProperty, nullptr);
	sphereNode->geometry()->addMaterial(sphereMaterial);

//	auto background = make_shared<MaterialProperty>(Color::Lime());
//	scene->background(background);

	auto background = make_shared<MaterialProperty>(TestCubeImageNamed("nebula1_blue", "png"));
	scene->background(background);


	activity->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);

	auto inputManager = activity->inputManager(); // enable it

	activity->scene(scene);
	activity->display(app);
}
