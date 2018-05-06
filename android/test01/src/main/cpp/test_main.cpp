
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <errno.h>
#include <iostream>
#include <jni.h>
#include <memory>

#include <glm/glm.hpp>

#include "ae.h"
#include "NDKHelper.h"


using namespace ae;
using namespace glm;
using namespace std;


struct android_app;


void android_main(android_app* app) {

	auto renderer = make_shared<OpenGLRenderer>();
	auto activity = make_shared<Activity>(static_pointer_cast<Renderer>(renderer));

	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));

	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.3, 0.3, 0.3, 1.0));
	auto ambientLightNode = Node::LightNode(ambientLight);
	scene->rootNode()->addChildNode(ambientLightNode);

	auto sphereGeo = make_shared<Sphere>(0.5, 24);
	auto sphereNode = make_shared<Node>();
	sphereGeo->name("sphere");
	sphereNode->geometry(sphereGeo);
	scene->rootNode()->addChildNode(sphereNode);
	sphereNode->position({0.0f, 0.0f, 0.0f});

	auto background = make_shared<MaterialProperty>(Color::Lime());
	scene->background(background);

	activity->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);

	activity->scene(scene);
	activity->display(app);
}
