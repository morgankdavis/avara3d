//
//  OpenGLRenderer.cpp
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "OpenGLRenderer.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>

#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

//#include "Camera.h"
#include "Color.h"
//#include "Geometry.h"
#include "GeometryElement.h"
#include "Global.h"
#include "Image.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "MaterialProperty.h"
#include "Node.h"
//#include "PhysicsWorld.h"
#include "Program.h"
#include "RenderContext.h"
#include "Scene.h"
//#include "SkyboxGeometry.h"

using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
     Types
 ***************************************************************************************/

typedef struct {
	int32_t 	type;
	float32_t 	PADDING1;
	float32_t 	PADDING2;
	float32_t 	PADDING3;
	vec3 		position_world;
	float32_t 	PADDING4;
	vec3 		color;
	float32_t 	PADDING5;
	float 		attenuationFactor;
	float32_t 	PADDING6;
	float32_t	PADDING7;
	float32_t 	PADDING8;
	//	vec3 direction_world;
	//	float attenuationStart;
	//	float attenuationEnd;
	//	float attenuationExponent;
	//	float innerAngle;
	//	float outerAngle;
} LightGLSLStruct;

typedef struct {
	float32_t 	startDistance;
	float32_t 	endDistance;
	float32_t 	densityExponent;
	float32_t 	PADDING1;
	vec4 		color;
	//float32_t 	PADDING2;
} FogGLSLStruct;

/**************************************************************************************
     Static
 **************************************************************************************/

static vector<shared_ptr<Node>> SortedLights(map<shared_ptr<Node>, float> lights) {
	
	// http://thispointer.com/how-to-sort-a-map-by-value-in-c/
	
	typedef function<bool(pair<shared_ptr<Node>, float>, pair<shared_ptr<Node>, float>)> Comparator;
	
	Comparator compFunctor = [](pair<shared_ptr<Node>, float> elem1, pair<shared_ptr<Node>, float> elem2) {
		return elem1.second < elem2.second;
	};
	
	set<pair<shared_ptr<Node>, float>, Comparator> lightsSorted(lights.begin(),
																lights.end(),
																compFunctor);
	
	auto sortedVector = vector<shared_ptr<Node>>();
	for (pair<shared_ptr<Node>, float> element : lightsSorted) {
		//cout << element.first << " :: " << element.second << endl;
		sortedVector.emplace_back(element.first);
	}
	
	return sortedVector;
}

// * TEMPORARY *
static void PrepareMaterialForRender(const Material& material, Program& program, DEBUG_OPTIONS debugOptions) {
	AE_LOG->trace("PrepareMaterialForRender()");
	
	//if ((unsigned)debugOptions | (unsigned)DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES) {
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
		// can create zbuffer problems
		// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
		//glDepthRange(0.1, 1.0);
		//		glEnable(GL_POLYGON_OFFSET_FILL);
		//		glPolygonOffset(20.0, 0.0);
	}
	
	//if ((unsigned)debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_WIREFRAMES) {
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		//m_program = Program::Wireframe();
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		//m_program = Program::Default();
		
		//if ((debugOptions & DebugOption_ShowWireframes) || (m_fillMode == FillMode_Lines)) {
		if (material.fillMode() == FILL_MODE::LINES) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (material.fillMode() == FILL_MODE::POINTS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		if (material.doubleSided()) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		
		program.setUniform("specularExponent", material.specularExponent());
		
		program.setUniform("uvScale", material.uvScale());
		//		m_program->setUniform("specularExponent", m_uvScale);
		
		// this is a bit of a hack, but since we're sharing programs now this needs to be reset...
		//MaterialPropertyType_Emissive
		program.setUniform("emissiveMode", 0); // 0 = MaterialMode_None
		
		// only lock for diffuse textures, not colors
		if (material.locksAmbientWithDiffuse() && material.diffuse() && (material.diffuse()->color() || material.diffuse()->image())) {
			material.diffuse()->bind(MATERIAL_PROPERTY_TYPE::AMBIENT, program);
		}
		else {
			if (material.ambient()) {
				material.ambient()->bind(MATERIAL_PROPERTY_TYPE::AMBIENT, program);
			}
		}
		if (material.diffuse()) {
			material.diffuse()->bind(MATERIAL_PROPERTY_TYPE::DIFFUSE, program);
		}
		if (material.specular()) {
			material.specular()->bind(MATERIAL_PROPERTY_TYPE::SPECULAR, program);
		}
		if (material.emissive()) {
			material.emissive()->bind(MATERIAL_PROPERTY_TYPE::EMISSIVE, program);
		}
	}
}

/**************************************************************************************
     Internal
 **************************************************************************************/

shared_ptr<Image> OpenGLRenderer::snapshot(unsigned framebufferWidth,
										   unsigned framebufferHeight) const {
	
	unsigned char *buf = (unsigned char*)malloc(framebufferWidth * framebufferHeight * 4);
	glReadPixels(0, 0, framebufferWidth, framebufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	auto image = make_shared<Image>(buf, framebufferWidth, framebufferHeight);
	free(buf);
	return image;
}

void OpenGLRenderer::render(const Scene& scene) {
	
	//m_renderStats = (RenderStats){};
	
	Renderer::render(scene);
	
	//	auto viewMat = m_pointOfView->worldTransform();
	//	auto projectionMat = m_pointOfView->camera()->projection();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (scene.background()) {
		if (scene.background()->cube()) {
			//			mat4 skyboxViewMat = lookAt(vec3(0.0f, 0.0f, 0.0f), // eye - location
			//										m_pointOfView->worldForward(), // center - look at
			//										m_pointOfView->worldUp()); // up
			
			//			SkyboxGeometry skyboxGeometry = *(*(scene.skyboxGeometry()));
			//			SkyboxGeometryElement& element = static_cast<SkyboxGeometryElement&>(geoElem);
			
			//m_skyboxGeometry->draw(skyboxViewMat, projectionMat, stats);
			//render(*(scene.skyboxGeometry()));
		}
		else if (scene.background()->color()) {
			auto color = *(scene.background()->color());
			glClearColor(color.r, color.g, color.b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
	
	if (m_glEnvironmentUBO < 0) {
		uint32 ubo;
		glGenBuffers(1, &ubo);
		m_glEnvironmentUBO = ubo;
	}
	bindEnvironment(scene, renderStats());
	
}

void OpenGLRenderer::render(const GeometryElement& geometryElement,
							const Material& material,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions) {
	
	//auto program = material.program();
	//auto program = material.selectProgram(m_debugOptions);
	
	auto program = Program::Default();
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		program = Program::Wireframe();
		glEnable(GL_LINE_SMOOTH);
	}
	
	// gl config
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// use shader program
	
	program->use();
	
	// uniforms
	
	program->setUniform("model", modelMat);
	program->setUniform("view", inverse(viewMat));
	program->setUniform("projection", projectionMat);
	
	// * TEMPORARY *
	//material.prepareToRender(m_debugOptions);
	PrepareMaterialForRender(material, *program, debugOptions);
	
	//	if (!((unsigned)debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
	//		program->bindUniformBlock("EnvironmentBlock", glEnvironmentUBO);
	//	}
	
	// draw
	
	glBindVertexArray(geometryElement.GLVAO());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometryElement.GLIBO());
	unsigned int numFaces = geometryElement.faces().size();
	renderStats().polygons += numFaces;
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)0);
	
	program->unuse();
}

/**************************************************************************************
     Private
 **************************************************************************************/

void OpenGLRenderer::bindEnvironment(const Scene& scene, RenderStats& stats) const {
	
	// lights
	
	auto lights = vector<shared_ptr<Node>>();
	shared_ptr<Node> ambientLight = nullptr;
	
	// find all lights in the scene
	for (auto node: scene.rootNode()->childNodes(true)) {
		if (!node->hidden()) {
			auto light = node->light();
			if (light != nullptr) {
				if (light->type() == LIGHT_TYPE::POINT) {
					lights.emplace_back(node);
				}
				else if (light->type() == LIGHT_TYPE::AMBIENT) {
					ambientLight = node;
				}
			}
		}
	}
	
	if (lights.size() > MAX_DYNAMIC_LIGHTS) {
		
		// find all light distances from the camera
		
		auto lightsUnsorted = map<shared_ptr<Node>, float>();
		//vec3 cameraPos_world = m_pointOfView->worldPosition();
		vec3 cameraPos_world = scene.renderContext().lock()->pointOfView()->worldPosition();
		for (auto lightNode: lights) {
			auto lightPos_world = lightNode->worldPosition();
			auto lightToCamera = lightPos_world - cameraPos_world;
			auto lightToCameraDistance = length(lightToCamera);
			lightsUnsorted[lightNode] = lightToCameraDistance;
			//cout << "lightToCameraDistance: " << lightToCameraDistance << endl;
		}
		
		lights = SortedLights(lightsUnsorted);
		
		unsigned endIndex = std::min((unsigned)lights.size(), (unsigned)(MAX_DYNAMIC_LIGHTS));
		vector<shared_ptr<Node>>::const_iterator first = lights.begin() + 0;
		vector<shared_ptr<Node>>::const_iterator last = lights.begin() + endIndex;
		vector<shared_ptr<Node>> lightsSlice(first, last);
		
		lights = lightsSlice;
	}
	
	// check for default lighting
	
	if (lights.size() == 0) {
		auto detaultPoint = Light::DefaultPointNode();
		// set position based on scene extent...
		static vec3 sceneExtent = scene.extent(); // only doing this once or it runs reallll slow
		detaultPoint->position({sceneExtent.x + sceneExtent.x/4.0,
			sceneExtent.y + sceneExtent.y/4.0,
			sceneExtent.z + sceneExtent.z/4.0});
		lights.emplace_back(detaultPoint);
	}
	if (!ambientLight) ambientLight = Light::DefaultAmbientNode();
	
	lights.emplace_back(ambientLight);
	
	unsigned numLights = lights.size();
	LightGLSLStruct lightStruct[numLights];
	
	stats.lights = numLights - 1; // not counting ambient
	
	for (int l=0; l<numLights; ++l) {
		auto node = lights[l];
		auto light = node->light();
		
		lightStruct[l].type = (unsigned)(light->type());
		lightStruct[l].position_world = node->worldPosition();
		lightStruct[l].attenuationFactor = light->attenuationFactor();
		
		auto color = *light->color();
		lightStruct[l].color = vec3(color.r, color.g, color.b);
	}
	
	// fog
	
	//	FogGLSLStruct fogStruct;
	//	fogStruct.startDistance = m_fogStartDistance;
	//	fogStruct.endDistance = m_fogEndDistance;
	//	fogStruct.densityExponent = m_fogDensityExponent;
	//	fogStruct.startDistance = m_fogStartDistance;
	//	if (m_fogColor) fogStruct.color = vec4(m_fogColor->r, m_fogColor->g, m_fogColor->b, m_fogColor->a);
	//	else fogStruct.color = vec4(0.0, 0.0, 0.0, 0.0);
	
	FogGLSLStruct fogStruct;
	fogStruct.startDistance = scene.fogStartDistance();
	fogStruct.endDistance = scene.fogEndDistance();
	fogStruct.densityExponent = scene.fogDensityExponent();
	fogStruct.startDistance = scene.fogStartDistance();
	if (scene.fogColor()) fogStruct.color = vec4(scene.fogColor()->r, scene.fogColor()->g, scene.fogColor()->b,
												 scene.fogColor()->a);
	else fogStruct.color = vec4(0.0, 0.0, 0.0, 0.0);
	
	// block
	
	typedef struct {
		int32_t 			numLights;
		float32_t 			PADDING1;
		float32_t 			PADDING2;
		float32_t 			PADDING3;
		LightGLSLStruct 	lights[MAX_DYNAMIC_LIGHTS+1]; // +1 ambient
		FogGLSLStruct		fog;
	} EnvironmentBlock;
	
	EnvironmentBlock environmentBlock;
	environmentBlock.numLights = numLights;
	memcpy(&environmentBlock.lights, &lightStruct, sizeof(lightStruct));
	memcpy(&environmentBlock.fog, &fogStruct, sizeof(fogStruct));
	
	glBindBuffer(GL_UNIFORM_BUFFER, m_glEnvironmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(environmentBlock), &environmentBlock, GL_DYNAMIC_DRAW);
}

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

OpenGLRenderer::OpenGLRenderer() {
	
}

OpenGLRenderer::~OpenGLRenderer() {
	
}
