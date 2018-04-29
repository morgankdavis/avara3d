//
//  RenderContext.cpp
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "RenderContext.h"

//#include <algorithm>
//#include <functional>
//#include <iostream>
//#include <map>
//#include <set>
#include <string>
#include <vector>

#include "gif.h"
//#include <GL/glew.h>
////#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
#include "Camera.h"
//#include "Color.h"
//#include "Geometry.h"
//#include "GeometryElement.h"
//#include "Global.h"
#include "Image.h"
//#include "Light.h"
#include "Logger.h"
//#include "Material.h"
//#include "MaterialProperty.h"
#include "Node.h"
#include "Renderer.h"
//#include "OpenGLRenderer.h"
//#include "PhysicsWorld.h"
//#include "Program.h"
#include "Scene.h"
//#include "SkyboxGeometry.h"


using namespace ae;
using namespace glm;
using namespace std;

/**************************************************************************************
     Internal Members
 **************************************************************************************/

//GifWriter* 		i_gifWriter;

///***************************************************************************************
//     Types
// ***************************************************************************************/
//
//typedef struct {
//	int32_t 	type;
//	float32_t 	PADDING1;
//	float32_t 	PADDING2;
//	float32_t 	PADDING3;
//	vec3 		position_world;
//	float32_t 	PADDING4;
//	vec3 		color;
//	float32_t 	PADDING5;
//	float 		attenuationFactor;
//	float32_t 	PADDING6;
//	float32_t	PADDING7;
//	float32_t 	PADDING8;
//	//	vec3 direction_world;
//	//	float attenuationStart;
//	//	float attenuationEnd;
//	//	float attenuationExponent;
//	//	float innerAngle;
//	//	float outerAngle;
//} LightGLSLStruct;
//
//typedef struct {
//	float32_t 	startDistance;
//	float32_t 	endDistance;
//	float32_t 	densityExponent;
//	float32_t 	PADDING1;
//	vec4 		color;
//	//float32_t 	PADDING2;
//} FogGLSLStruct;

/**************************************************************************************
     Static
 **************************************************************************************/

//static shared_ptr<Program> ProgramForMaterial(Material& material, DEBUG_OPTIONS debugOptions) {
//	
//	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
//		m_program = Program::Wireframe();
//		glEnable(GL_LINE_SMOOTH);
//	}
//	else {
//		m_program = Program::Default();
//		//glDisable(GL_LINE_SMOOTH);
//	}
//	return m_program;
//}

//static vector<shared_ptr<Node>> SortedLights(map<shared_ptr<Node>, float> lights) {
//	
//	// http://thispointer.com/how-to-sort-a-map-by-value-in-c/
//	
//	typedef function<bool(pair<shared_ptr<Node>, float>, pair<shared_ptr<Node>, float>)> Comparator;
//	
//	Comparator compFunctor = [](pair<shared_ptr<Node>, float> elem1, pair<shared_ptr<Node>, float> elem2) {
//		return elem1.second < elem2.second;
//	};
//	
//	set<pair<shared_ptr<Node>, float>, Comparator> lightsSorted(lights.begin(),
//																lights.end(),
//																compFunctor);
//	
//	auto sortedVector = vector<shared_ptr<Node>>();
//	for (pair<shared_ptr<Node>, float> element : lightsSorted) {
//		//cout << element.first << " :: " << element.second << endl;
//		sortedVector.emplace_back(element.first);
//	}
//	
//	return sortedVector;
//}
//
//// * TEMPORARY *
//static void PrepareMaterialForRender(const Material& material, Program& program, DEBUG_OPTIONS debugOptions) {
//	AE_LOG->trace("PrepareMaterialForRender()");
//	
//	//if ((unsigned)debugOptions | (unsigned)DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES) {
//	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
//		// can create zbuffer problems
//		// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
//		//glDepthRange(0.1, 1.0);
//		//		glEnable(GL_POLYGON_OFFSET_FILL);
//		//		glPolygonOffset(20.0, 0.0);
//	}
//	
//	//if ((unsigned)debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_WIREFRAMES) {
//	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
//		//m_program = Program::Wireframe();
//		
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	}
//	else {
//		//m_program = Program::Default();
//		
//		//if ((debugOptions & DebugOption_ShowWireframes) || (m_fillMode == FillMode_Lines)) {
//		if (material.fillMode() == FILL_MODE::LINES) {
//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		}
//		else if (material.fillMode() == FILL_MODE::POINTS) {
//			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
//		}
//		else {
//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		}
//		
//		if (material.doubleSided()) {
//			glDisable(GL_CULL_FACE);
//		}
//		else {
//			glEnable(GL_CULL_FACE);
//			glCullFace(GL_BACK);
//		}
//		
//		program.setUniform("specularExponent", material.specularExponent());
//		
//		program.setUniform("uvScale", material.uvScale());
//		//		m_program->setUniform("specularExponent", m_uvScale);
//		
//		// this is a bit of a hack, but since we're sharing programs now this needs to be reset...
//		//MaterialPropertyType_Emissive
//		program.setUniform("emissiveMode", 0); // 0 = MaterialMode_None
//		
//		// only lock for diffuse textures, not colors
//		if (material.locksAmbientWithDiffuse() && material.diffuse() && (material.diffuse()->color() || material.diffuse()->image())) {
//			material.diffuse()->bind(MATERIAL_PROPERTY_TYPE::AMBIENT, program);
//		}
//		else {
//			if (material.ambient()) {
//				material.ambient()->bind(MATERIAL_PROPERTY_TYPE::AMBIENT, program);
//			}
//		}
//		if (material.diffuse()) {
//			material.diffuse()->bind(MATERIAL_PROPERTY_TYPE::DIFFUSE, program);
//		}
//		if (material.specular()) {
//			material.specular()->bind(MATERIAL_PROPERTY_TYPE::SPECULAR, program);
//		}
//		if (material.emissive()) {
//			material.emissive()->bind(MATERIAL_PROPERTY_TYPE::EMISSIVE, program);
//		}
//	}
//}

/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

//RenderContext::RenderContext(shared_ptr<Renderer> renderer):
RenderContext::RenderContext(shared_ptr<Renderer> renderer):
	m_renderer(renderer),
	m_scene(nullptr),
	m_vSyncEnabled(false),
	m_debugOptions(DEBUG_OPTIONS::NONE),
	m_antialiasingMode(ANTIALIASING_MODE::NONE),
	m_pointOfView(nullptr),
	m_updateCallback(nullptr),
	m_didSimulatePhysicsCallback(nullptr),
	m_willRenderCallback(nullptr),
	m_didRenderCallback(nullptr) {
	
}

RenderContext::~RenderContext() {
	
}

/**************************************************************************************
     Public
 **************************************************************************************/

shared_ptr<Renderer> RenderContext::renderer() const {
	return m_renderer;
}

void RenderContext::renderer(shared_ptr<Renderer> renderer) {
	m_renderer = renderer;
}

shared_ptr<Scene> RenderContext::scene() const {
	//return m_renderer->scene();
	return m_scene;
}

void RenderContext::scene(const shared_ptr<Scene> scene) {
	//m_renderer->scene(scene);
	m_scene = scene;
	scene->attachedToRenderContext(shared_from_this());
}

unsigned RenderContext::width() const {
	return m_width;
}

void RenderContext::width(unsigned width) {
	m_width = width;
	framebufferWidth(m_width * m_framebufferScale);
}

unsigned RenderContext::height() const {
	return m_height;
}

void RenderContext::height(unsigned height) {
	m_height = height;
	framebufferHeight(m_height * m_framebufferScale);
}

unsigned RenderContext::framebufferScale() const {
	return m_framebufferScale;
}

void RenderContext::framebufferScale(unsigned scale) {
	m_framebufferScale = scale;
}

unsigned RenderContext::framebufferWidth() const {
	return m_framebufferWidth;
}

void RenderContext::framebufferWidth(unsigned width) {
	m_framebufferWidth = width;
}

unsigned RenderContext::framebufferHeight() const {
	return m_framebufferHeight;
}

void RenderContext::framebufferHeight(unsigned height) {
	m_framebufferHeight = height;
}

bool RenderContext::vSyncEnabled() const {
	return m_vSyncEnabled;
}

void RenderContext::enableVSync(bool enabled) {
	m_vSyncEnabled = enabled;
}

DEBUG_OPTIONS RenderContext::debugOptions() const {
	return m_debugOptions;
}

void RenderContext::debugOptions(DEBUG_OPTIONS options) {
	m_debugOptions = options;
	
//	if (m_scene && m_scene->physicsWorld()) {
//		m_scene->physicsWorld()->debugOptions(m_debugOptions);
//	}
}

shared_ptr<Node> RenderContext::pointOfView() {
	
	if (m_pointOfView) {
		return m_pointOfView;
	}
	else {
		// try to assign one from the scene
		for (auto node: m_scene->rootNode()->childNodes(true)) {
			if (node->camera()) {
				m_pointOfView = node;
				return m_pointOfView;
			}
		}
	}
	if (!m_pointOfView) {
		// still no POV. add a default one.
		m_pointOfView = defaultPointOfView();
	}
	
	return m_pointOfView;
}

void RenderContext::pointOfView(const shared_ptr<Node> camera) {
	m_pointOfView = camera;
}

ANTIALIASING_MODE RenderContext::antialiasingMode() const {
	return m_antialiasingMode;
}

shared_ptr<InputManager> RenderContext::inputManager() {
	AE_LOG->critical("RenderContext::inputManager() should be overidden in derived class.");
	return nullptr;
}

float RenderContext::sceneTime() {
	// should probably override in subclass to use library's time utilities (GLFW, for example)
	static auto startDate = chrono::high_resolution_clock::now();
	auto nowDate = chrono::high_resolution_clock::now();
	return (chrono::duration<float>(nowDate - startDate)).count();
}

shared_ptr<Image> RenderContext::snapshot() const {
	if (m_renderer) {
		return m_renderer->snapshot(*this);
	}
	return nullptr;
}

bool RenderContext::recordingGIF() const {
	return m_recordingGIF;
}

void RenderContext::startGIFRecording(const boost::filesystem::path& path,
							   unsigned maxHeight, unsigned maxFramerate) {
//	if (!m_recordingGIF) {
//		AE_LOG->info("Starting GIF recording...");
//		
//		m_gifRecordingMaxFramerate = maxFramerate;
//		m_gifRecordedFrames = 0;
//		
//		m_gifRecordingHeight = m_framebufferHeight;
//		m_gifRecordingWidth = m_framebufferWidth;
//		if (m_gifRecordingHeight > maxHeight) {
//			float scale = (float)maxHeight / (float)m_framebufferHeight;
//			m_gifRecordingHeight = m_framebufferHeight * scale;
//			m_gifRecordingWidth = m_framebufferWidth * scale;
//		}
//		
//		unsigned frameTimeMS = 1000.0 /* (ms/sec) */ / m_gifRecordingMaxFramerate /* (frames/sec) */;
//		// -> ms/frame
//		unsigned frameTimeHS = frameTimeMS / 10.0; // 100th sec/frame
//		
//		//m_gifWriter = (GifWriter *)malloc(sizeof(GifWriter));
//		m_gifWriter = make_shared<GifWriter>();
//
//		// gif-h frame time is in 100ths of a second
//		GifWriter* writer = m_gifWriter.get();
//		GifBegin(writer, path.string().c_str(), 
//				 m_gifRecordingWidth, m_gifRecordingHeight, 
//				 frameTimeHS);
//		
//		m_recordingGIF = true;
//	}
}

void RenderContext::stopGIFRecording() {
//	if (m_recordingGIF) {
//		m_recordingGIF = false;
//		
//		GifWriter* writer = m_gifWriter.get();
//		GifEnd(writer);
//		// crashing... but it doesn't look like GifEnd() frees everything,
//		// just the main buffer.
//		//free(i_gifWriter);
//		
//		AE_LOG->info("Stopped GIF recording.");
//	}
}

RenderContextUpdateFuction RenderContext::updateCallback() {
	return m_updateCallback;
}

void RenderContext::updateCallback(RenderContextUpdateFuction function) {
	m_updateCallback = function;
}

RenderContextDidSimulatePhysicsFuction RenderContext::didSimulatePhysicsCallback() {
	return m_didSimulatePhysicsCallback;
}

void RenderContext::didSimulatePhysicsCallback(RenderContextDidSimulatePhysicsFuction function) {
	m_didSimulatePhysicsCallback = function;
}

RenderContextWillRenderFuction RenderContext::willRenderCallback() {
	return m_willRenderCallback;
}

void RenderContext::willRenderCallback(RenderContextWillRenderFuction function) {
	m_willRenderCallback = function;
}

RenderContextDidRenderFuction RenderContext::didRenderCallback() {
	return m_didRenderCallback;
}

void RenderContext::didRenderCallback(RenderContextDidRenderFuction function) {
	m_didRenderCallback = function;
}

/**************************************************************************************
     Internal
 **************************************************************************************/

//void RenderContext::drawLoop() {
//	
//	AE_LOG->trace("-------------------------------------------------------------------------------");
//	
//	float time = sceneTime();
//	static double previousSeconds = time;
//	float deltaSeconds = time - previousSeconds;
//	previousSeconds = time;
//	
//	
//	////	float time = glfwGetTime();
//	////	static double previousSeconds = time;
//	////	float deltaSeconds = time - previousSeconds;
//	////	previousSeconds = time;
//	////	
//	//	if (m_updateCallback) m_updateCallback(*this, sceneTime());
//	//	
//	//	auto pov = pointOfView();
//	//	float aspectRatio = (float)m_framebufferWidth/(float)m_framebufferHeight;
//	//	pov->camera()->aspectRatio(aspectRatio);
//	//	
//	//	RenderStats stats = {};
//	//	//stats.cameraPosition = pov->worldPosition();
//	//	stats.cameraPosition = pov->position();
//	//	
//	//	// simulate physics
//	//	auto physicsWorld = m_scene->physicsWorld();
//	//	if (physicsWorld) {
//	//		physicsWorld->step();
//	//		
//	//		if (m_didSimulatePhysicsCallback) {
//	//			m_didSimulatePhysicsCallback(*this, glfwGetTime());
//	//		}
//	//	}
//	//	
//	//	CheckGLError();
//	//	
//	//	if (m_willRenderCallback) m_willRenderCallback(*this, glfwGetTime());
//	//	
//	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	//	
//	//	glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
//	//
//	//	//m_scene->draw(pov, m_debugOptions, stats);
//	//	render(m_scene);
//	//	
//	//	if ((unsigned)m_debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_STATS_OVERLAY) updateStatsOverlay(stats);
//	//	
//	//	glfwSwapBuffers(i_glfwWindow);
//	//	
//	//	if (m_recordingGIF) saveGIFFrame(deltaSeconds);
//	//	
//	//	if (m_didRenderCallback) m_didRenderCallback(*this, glfwGetTime());
//	//	
//	//	glfwPollEvents();
//	//	if (m_inputManager) m_inputManager->update();
//}

////void RenderContext::render(const Scene& scene) {
////	
////	m_renderStats = (RenderStats){};
////	
////	//	auto viewMat = m_pointOfView->worldTransform();
////	//	auto projectionMat = m_pointOfView->camera()->projection();
////	
////	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
////	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////	
////	if (scene.background()) {
////		if (scene.background()->cube()) {
////			//			mat4 skyboxViewMat = lookAt(vec3(0.0f, 0.0f, 0.0f), // eye - location
////			//										m_pointOfView->worldForward(), // center - look at
////			//										m_pointOfView->worldUp()); // up
////			
////			//			SkyboxGeometry skyboxGeometry = *(*(scene.skyboxGeometry()));
////			//			SkyboxGeometryElement& element = static_cast<SkyboxGeometryElement&>(geoElem);
////			
////			//m_skyboxGeometry->draw(skyboxViewMat, projectionMat, stats);
////			//render(*(scene.skyboxGeometry()));
////		}
////		else if (scene.background()->color()) {
////			auto color = *(scene.background()->color());
////			glClearColor(color.r, color.g, color.b, 1.0f);
////			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////		}
////	}
////	
////	if (m_glEnvironmentUBO < 0) {
////		uint32 ubo;
////		glGenBuffers(1, &ubo);
////		m_glEnvironmentUBO = ubo;
////	}
////	bindEnvironment(scene, m_renderStats);
////	
////}
//
//void RenderContext::render(const GeometryElement& geometryElement,
//						  const Material& material,
//						  const mat4& modelMat,
//						  const glm::mat4& viewMat,
//						  const glm::mat4& projectionMat) {
//	
//	//auto program = material.program();
//	//auto program = material.selectProgram(m_debugOptions);
//	
//	auto program = Program::Default();
//	if (DEBUG_OPTIONS_CONTAINS(m_debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
//		program = Program::Wireframe();
//		glEnable(GL_LINE_SMOOTH);
//	}
//	
//	// gl config
//	
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	glDepthMask(GL_TRUE);
//	
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	
//	// use shader program
//	
//	program->use();
//	
//	// uniforms
//	
//	program->setUniform("model", modelMat);
//	program->setUniform("view", inverse(viewMat));
//	program->setUniform("projection", projectionMat);
//	
//	// * TEMPORARY *
//	//material.prepareToRender(m_debugOptions);
//	PrepareMaterialForRender(material, *program, m_debugOptions);
//	
//	//	if (!((unsigned)debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
//	//		program->bindUniformBlock("EnvironmentBlock", glEnvironmentUBO);
//	//	}
//	
//	// draw
//	
//	glBindVertexArray(geometryElement.GLVAO());
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometryElement.GLIBO());
//	unsigned int numFaces = geometryElement.faces().size();
//	m_renderStats.polygons += numFaces;
//	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)0);
//	
//	program->unuse();
//}

/**************************************************************************************
     Protected
 **************************************************************************************/

void RenderContext::saveGIFFrame(float deltaSeconds) {
	
	//	static float secondsAccum = 0;
	//	secondsAccum += deltaSeconds;
	//	
	//	unsigned frameTimeMS = 1000.0 /* (ms/sec) */ / m_gifRecordingMaxFramerate /* (frames/sec) */;
	//	// -> ms/frame
	//	//unsigned frameTimeHS = frameTimeMS / 10.0; // 100th sec/frame
	//	
	//	//unsigned frameTime = 1000.0/m_gifRecordingMaxFramerate; // ms/frame
	//	
	//	if (secondsAccum >= frameTimeMS/1000.0) {
	//		
	//		auto frame = snapshot();
	//		
	//		unsigned char* resizedFrameData = (unsigned char*)malloc(m_gifRecordingWidth * m_gifRecordingHeight * 4);
	//		stbir_resize_uint8(frame->data(), frame->width(), frame->height(), 0,
	//						   resizedFrameData, m_gifRecordingWidth, m_gifRecordingHeight, 0, 4);
	//		
	//		// gif-h frame time is in 100ths of a second
	//		GifWriteFrame(m_gifWriter.get(), resizedFrameData,
	//					  m_gifRecordingWidth, m_gifRecordingHeight,
	//					  (secondsAccum*1000.0)/10.0);
	//		
	//		++m_gifRecordedFrames;
	//		
	//		//secondsAccum = secondsAccum - frameTimeMS/1000.0;
	//		secondsAccum = 0;
	//	}
}

//shared_ptr<Node> RenderContext::defaultPointOfView() {
//	AE_LOG->critical("RenderContext::defaultPointOfView() should be overidden in derived class.");
//	return nullptr;
//}

shared_ptr<Node> RenderContext::defaultPointOfView() {
	
	auto cameraNode = make_shared<Node>();
	//m_scene->rootNode()->addChildNode(cameraNode); // done below
	auto camera = make_shared<Camera>();
	camera->name("default camera");
	cameraNode->camera(camera);
	
	auto boundingPoints = (*scene()->boundingPoints());
	
	float fovH = cameraNode->camera()->fov();
	float w = width();
	float h = height();
	float aspectRatio = w/h;
	float inverseAspectRatio = 1.0f/aspectRatio;
	float fovV = fovH * inverseAspectRatio;
	
	// tan(angle) = x/z
	// ztan(angle) = x
	// z = x/tan(angle)
	
	float maxZ = abs(boundingPoints["zMax"].z);
	
	float xH = abs(boundingPoints["xMin"].x) + abs(boundingPoints["xMax"].x) / 2.0f;
	float angleH = fovH / 2.0;
	float zH = xH / tan(angleH);
	
	float xV = abs(boundingPoints["yMin"].y) + abs(boundingPoints["yMax"].y) / 2.0f;
	float angleV = fovV / 2.0;
	float zV = xV / tan(angleV);
	
	zH += maxZ;
	zV += maxZ;
	
	float z = fmax(zH, zV);
	float midX = (boundingPoints["xMin"].x + boundingPoints["xMax"].x) / 2.0f;
	float midY = (boundingPoints["yMin"].y + boundingPoints["yMax"].y) / 2.0f;
	
	vec3 eye = vec3(midX, midY, z / 2.0f); // not sure why z is devided by 2.0, but it seems to work better...
	
	mat4 viewMat = translate(mat4(1.0f), eye);
	cameraNode->transform(viewMat);
	
	scene()->rootNode()->addChildNode(cameraNode);
	pointOfView(cameraNode);
	
	return cameraNode;
}

//void RenderContext::saveGIFFrame(float deltaSeconds) {
//	AE_LOG->critical("RenderContext::saveGIFFrame() must be overidden in derived class.");
//}
//
//RenderStats& RenderContext::renderStats() {
//	return m_renderStats;
//}

/**************************************************************************************
     Private
 **************************************************************************************/

//void RenderContext::bindEnvironment(const Scene& scene, RenderStats& stats) const {
//	
//	// lights
//	
//	auto lights = vector<shared_ptr<Node>>();
//	shared_ptr<Node> ambientLight = nullptr;
//	
//	// find all lights in the scene
//	for (auto node: scene.rootNode()->childNodes(true)) {
//		if (!node->hidden()) {
//			auto light = node->light();
//			if (light != nullptr) {
//				if (light->type() == LIGHT_TYPE::POINT) {
//					lights.emplace_back(node);
//				}
//				else if (light->type() == LIGHT_TYPE::AMBIENT) {
//					ambientLight = node;
//				}
//			}
//		}
//	}
//	
//	if (lights.size() > MAX_DYNAMIC_LIGHTS) {
//		
//		// find all light distances from the camera
//		
//		auto lightsUnsorted = map<shared_ptr<Node>, float>();
//		vec3 cameraPos_world = m_pointOfView->worldPosition();
//		for (auto lightNode: lights) {
//			auto lightPos_world = lightNode->worldPosition();
//			auto lightToCamera = lightPos_world - cameraPos_world;
//			auto lightToCameraDistance = length(lightToCamera);
//			lightsUnsorted[lightNode] = lightToCameraDistance;
//			//cout << "lightToCameraDistance: " << lightToCameraDistance << endl;
//		}
//		
//		lights = SortedLights(lightsUnsorted);
//		
//		unsigned endIndex = std::min((unsigned)lights.size(), (unsigned)(MAX_DYNAMIC_LIGHTS));
//		vector<shared_ptr<Node>>::const_iterator first = lights.begin() + 0;
//		vector<shared_ptr<Node>>::const_iterator last = lights.begin() + endIndex;
//		vector<shared_ptr<Node>> lightsSlice(first, last);
//		
//		lights = lightsSlice;
//	}
//	
//	// check for default lighting
//	
//	if (lights.size() == 0) {
//		auto detaultPoint = Light::DefaultPointNode();
//		// set position based on scene extent...
//		static vec3 sceneExtent = scene.extent(); // only doing this once or it runs reallll slow
//		detaultPoint->position({sceneExtent.x + sceneExtent.x/4.0,
//			sceneExtent.y + sceneExtent.y/4.0,
//			sceneExtent.z + sceneExtent.z/4.0});
//		lights.emplace_back(detaultPoint);
//	}
//	if (!ambientLight) ambientLight = Light::DefaultAmbientNode();
//	
//	lights.emplace_back(ambientLight);
//	
//	unsigned numLights = lights.size();
//	LightGLSLStruct lightStruct[numLights];
//	
//	stats.lights = numLights - 1; // not counting ambient
//	
//	for (int l=0; l<numLights; ++l) {
//		auto node = lights[l];
//		auto light = node->light();
//		
//		lightStruct[l].type = (unsigned)(light->type());
//		lightStruct[l].position_world = node->worldPosition();
//		lightStruct[l].attenuationFactor = light->attenuationFactor();
//		
//		auto color = *light->color();
//		lightStruct[l].color = vec3(color.r, color.g, color.b);
//	}
//	
//	// fog
//	
//	//	FogGLSLStruct fogStruct;
//	//	fogStruct.startDistance = m_fogStartDistance;
//	//	fogStruct.endDistance = m_fogEndDistance;
//	//	fogStruct.densityExponent = m_fogDensityExponent;
//	//	fogStruct.startDistance = m_fogStartDistance;
//	//	if (m_fogColor) fogStruct.color = vec4(m_fogColor->r, m_fogColor->g, m_fogColor->b, m_fogColor->a);
//	//	else fogStruct.color = vec4(0.0, 0.0, 0.0, 0.0);
//	
//	FogGLSLStruct fogStruct;
//	fogStruct.startDistance = scene.fogStartDistance();
//	fogStruct.endDistance = scene.fogEndDistance();
//	fogStruct.densityExponent = scene.fogDensityExponent();
//	fogStruct.startDistance = scene.fogStartDistance();
//	if (scene.fogColor()) fogStruct.color = vec4(scene.fogColor()->r, scene.fogColor()->g, scene.fogColor()->b,
//												 scene.fogColor()->a);
//	else fogStruct.color = vec4(0.0, 0.0, 0.0, 0.0);
//	
//	// block
//	
//	typedef struct {
//		int32_t 			numLights;
//		float32_t 			PADDING1;
//		float32_t 			PADDING2;
//		float32_t 			PADDING3;
//		LightGLSLStruct 	lights[MAX_DYNAMIC_LIGHTS+1]; // +1 ambient
//		FogGLSLStruct		fog;
//	} EnvironmentBlock;
//	
//	EnvironmentBlock environmentBlock;
//	environmentBlock.numLights = numLights;
//	memcpy(&environmentBlock.lights, &lightStruct, sizeof(lightStruct));
//	memcpy(&environmentBlock.fog, &fogStruct, sizeof(fogStruct));
//	
//	glBindBuffer(GL_UNIFORM_BUFFER, m_glEnvironmentUBO);
//	glBufferData(GL_UNIFORM_BUFFER, sizeof(environmentBlock), &environmentBlock, GL_DYNAMIC_DRAW);
//}
