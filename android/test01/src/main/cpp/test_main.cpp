
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


static shared_ptr<Node> s_cameraNode = nullptr;
static shared_ptr<MaterialProperty> s_cubeBackground = nullptr;
static shared_ptr<Logger> s_logger = nullptr;


void android_main(android_app* app) {

	AE_INIT(app);

	s_logger = make_shared<Logger>("test", Logger::MainLogger()->sinks());

	LOG_I(s_logger, "********** info msg **********");
	LOG_I(s_logger, "********** info format: %d %s **********", 2, "dicks");

	LOG_W(s_logger, "********** warn msg **********");
	LOG_W(s_logger, "********** warn format: %d %s **********", 2, "dicks");


	auto activity = make_shared<Activity>(RENDER_API::OPENGL);

	activity->updateCallback(renderContextUpdateCallback);
	activity->willRenderCallback(renderContextWillRenderCallback);
	activity->didRenderCallback(renderContextDidRenderCallback);

	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));

	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, Color::DarkGray());
	auto ambientLightNode = Node::LightNode(ambientLight);
	scene->rootNode()->addChild(ambientLightNode);

	auto sphereGeo = make_shared<Sphere>(0.5, 24);
	auto sphereNode = make_shared<Node>();
	sphereGeo->name("sphere");
	sphereNode->geometry(sphereGeo);
	scene->rootNode()->addChild(sphereNode);
	sphereNode->position({0.0f, 0.0f, 0.0f});

	auto gridImage = ImageNamed("plant", string("jpg"));
	auto sphereMaterialProperty = make_shared<MaterialProperty>(gridImage);
	sphereMaterialProperty->wrapS(WRAP_MODE::REPEAT);
	sphereMaterialProperty->wrapT(WRAP_MODE::REPEAT);
	auto sphereMaterial = make_shared<Material>(nullptr, sphereMaterialProperty, nullptr);
	sphereNode->geometry()->addMaterial(sphereMaterial);

	s_cubeBackground = make_shared<MaterialProperty>(CubeImageNamed("nebula1_blue", "png"));
//	auto background = make_shared<MaterialProperty>(Color::Lime());
	scene->background(s_cubeBackground);


	activity->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);

	//auto inputManager = activity->inputManager(); // enable it

	activity->scene(scene);
	activity->display(app);
}

void renderContextUpdateCallback(RenderContext& renderContext, float time) {
	LOG_T(s_logger, "renderContextUpdateCallback({})", time);

	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	//auto activity = static_cast<Activity&>(renderContext);



	// get input

	auto inputManager = renderContext.inputManager();

	//auto keysPressed = activity.inputManager()->keysPressed();
	auto keysPressed = inputManager->keysPressed();


	if (keysPressed.count(KEY::F)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
	}
	if (keysPressed.count(KEY::B)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
	}
	if (keysPressed.count(KEY::I)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
	}





	auto mouseButtonsPressed = inputManager->mouseButtonsPressed();
	auto mouseButtonsDown = inputManager->mouseButtonsDown();


	if (mouseButtonsPressed.count(MOUSE_BUTTON::ONE)) {
		LOG_T(s_logger, "PRESSED MOUSE_BUTTON::ONE");
		auto background = make_shared<MaterialProperty>(Color::Lime());
		renderContext.scene()->background(background);
	}

	if (mouseButtonsPressed.count(MOUSE_BUTTON::TWO)) {
		LOG_T(s_logger, "PRESSED MOUSE_BUTTON::TWO");
		auto background = make_shared<MaterialProperty>(Color::Purple());
		renderContext.scene()->background(background);
	}

	if (mouseButtonsPressed.count(MOUSE_BUTTON::THREE)) {
		LOG_T(s_logger, "PRESSED MOUSE_BUTTON::THREE");
		renderContext.scene()->background(s_cubeBackground);
	}


	if (mouseButtonsDown.count(MOUSE_BUTTON::ONE)) {
		LOG_T(s_logger, "DOWN MOUSE_BUTTON::ONE");
	}

	if (mouseButtonsDown.count(MOUSE_BUTTON::TWO)) {
		LOG_T(s_logger, "DOWN MOUSE_BUTTON::TWO");
	}

	if (mouseButtonsDown.count(MOUSE_BUTTON::THREE)) {
		LOG_T(s_logger, "DOWN MOUSE_BUTTON::THREE");
	}





	vec2 mouseScrollWheelDelta = inputManager->mouseScrollWheelDelta();
	if (mouseScrollWheelDelta.y) {
		//AE_LOG->debug("Mouse scroll Y delta: {}", mouseScrollWheelDelta.y);

		static float FOV_SPEED = 2.5; // degrees/roll
		if (s_cameraNode) {
			auto fov = s_cameraNode->camera()->fov();
			fov += mouseScrollWheelDelta.y * -radians(FOV_SPEED);
			s_cameraNode->camera()->fov(fov);
		}
	}





	// move camera

	vec2 mousePositionDelta = inputManager->mousePositionDelta();

	const static float mouseSensitivity = (1.0f / 0.5f);

	if (!s_cameraNode) {
		for (auto n : renderContext.scene()->rootNode()->children(false)) {
			if (n->camera()) {
				s_cameraNode = n;
				break;
			}
		}
	}

	if (s_cameraNode) {

		//cout << "Camera distance: " << length(m_cameraNode->position()) << endl;

		// look

		vec3 camForward = s_cameraNode->worldForward();
		vec3 camRight = s_cameraNode->worldRight();
		vec3 camUp = s_cameraNode->worldUp();

		float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
		float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);

		vec3 angles = s_cameraNode->eulerAngles();
		s_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));

		// move

		auto keysDown = inputManager->keysDown();

		//		const static float MOVE_SPEED = 5.0f; // units/sec
		static float MOVE_SPEED = 0;
		if (!MOVE_SPEED) MOVE_SPEED = Max(renderContext.scene()->extent());

		if(keysDown.count(KEY::W)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
			s_cameraNode->position(s_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(KEY::S)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
			s_cameraNode->position(s_cameraNode->position() + positionDelta);
		}

		if(keysDown.count(KEY::A)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
			s_cameraNode->position(s_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(KEY::D)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
			s_cameraNode->position(s_cameraNode->position() + positionDelta);
		}

		if(keysDown.count(KEY::SPACE)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			s_cameraNode->position(s_cameraNode->position() + positionDelta);
		}
	}



}

void renderContextWillRenderCallback(RenderContext& renderContext, float time) {
	LOG_T(s_logger, "renderContextWillRenderCallback({})", time);
}

void renderContextDidRenderCallback(RenderContext& renderContext, float time) {
	LOG_T(s_logger, "renderContextDidRenderCallback({})", time);
}
