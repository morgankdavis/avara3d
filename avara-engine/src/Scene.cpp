//
//  Scene.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Scene.h"

#include <iostream>
//#include <memory>


// for light sorting
#include <map>
#include <set>
#include <algorithm>
#include <functional>



#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "Camera.h"
#include "Color.h"
#include "Geometry.h"
#include "GeometryElement.h"
#include "Image.h"
#include "Light.h"
#include "Material.h"
#include "MaterialProperty.h"
#include "Node.h"
#include "SkyboxGeometry.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace Assimp;
//using namespace boost;
using namespace boost::filesystem;
using namespace glm;
using namespace std;


/***************************************************************************************
     MARK:   Types
 **************************************************************************************/

typedef struct {
	int type;
	float PADDING1[3];
	vec3 position_world;
	float PADDING2[1];
	vec3 color;
	float PADDING3[1];
	//	vec3 direction_world;
	//	float attenuationStart;
	//	float attenuationEnd;
	//	float attenuationExponent;
	//	float innerAngle;
	//	float outerAngle;
} LightBlock;

/***************************************************************************************
     MARK:   Static
 **************************************************************************************/

static boost::optional<string> FilepathFromTextureFilename(const string& filename, const string& basePath) {

	string textureName = filename;
	if (textureName.substr(0,1) == "/") {
		textureName = textureName.substr(1, textureName.length()-1);
	}
	else if (textureName.substr(0,2) == "./") {
		textureName = textureName.substr(2, textureName.length()-2);
	}
	
	try {
		path texturePath = canonical(path(textureName), path(basePath));
		return texturePath.string();
	}
	catch (const boost::filesystem::filesystem_error& e) {
		cout << "Error expanding path: " << e.what() << endl;
	}
	
	cout << "*** Missing texture: " << filename << " ***" << endl;
	
	return {};
}

static shared_ptr<Image> MissingTextureImage() {
	static shared_ptr<Image> image = nullptr;
	if (!image) image = make_shared<Image>(ImagesDirectoryPath() + "missing_texture2.png");
	return image;
}

static shared_ptr<MaterialProperty> MaterialPropertyFromAIMaterial(const aiMaterial* aiMaterial,
																   aiTextureType type,
																   string basePath) {
	
	string typeStr = "";
	switch (type) {
		case aiTextureType_AMBIENT:
			typeStr = "ambient";
			break;
		case aiTextureType_DIFFUSE:
			typeStr = "diffuse";
			break;
		case aiTextureType_SPECULAR:
			typeStr = "specular";
			break;
		default:
			cout << "Unsupported material type: " << type << endl;
			return nullptr;
	}
	
	cout << "Reading " << typeStr << " material..." << endl;
	
	if (aiMaterial->GetTextureCount(type)) { // texture
		
		aiString filename;
		if (aiMaterial->GetTexture(type, 0, &filename,
								   NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			boost::optional<string> texturePath = FilepathFromTextureFilename(filename.C_Str(), basePath);
			if (texturePath) {
				cout << "Texture path: " << *texturePath << endl;
				auto textureImage = make_shared<Image>(*texturePath);
				return make_shared<MaterialProperty>(textureImage);
			}
			else {
				return make_shared<MaterialProperty>(MissingTextureImage());
			}
		}
	}
	else { // color
		
		aiColor4D aiColor;
		if (aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, aiColor) == AI_SUCCESS) {
			cout << "Color: ("
			<< aiColor.r << ", "
			<< aiColor.g << ", "
			<< aiColor.b << ", "
			<< aiColor.a << ")" << endl;
			auto aeColor = make_shared<Color>(AIColor4DToColor(aiColor));
			return make_shared<MaterialProperty>(aeColor);
		}
		else {
			cout << "No " << typeStr << " material." << endl;
		}
	}
	
	return nullptr;
}

static LightType LightTypeForAILightType(aiLightSourceType aiType) {
	switch (aiType) {
		case aiLightSource_DIRECTIONAL: return LightType_Directional;
		case aiLightSource_SPOT: return LightType_Spot;
		default: return LightType_Point;
	}
}

static vector<shared_ptr<Node>> SortedLights(map<shared_ptr<Node>, float> lights) {
	
	// http://thispointer.com/how-to-sort-a-map-by-value-in-c/
	
	typedef function<bool(pair<shared_ptr<Node>, float>, pair<shared_ptr<Node>, float>)> Comparator;
	
	Comparator compFunctor = [](pair<shared_ptr<Node>, float> elem1, pair<shared_ptr<Node>, float> elem2) {
		return elem1.second < elem2.second;
	};
	
	set<std::pair<shared_ptr<Node>, float>, Comparator> lightsSorted(lights.begin(),
																		lights.end(),
																		compFunctor);
	
	auto sortedVector = vector<shared_ptr<Node>>();
	for (pair<shared_ptr<Node>, float> element : lightsSorted) {
		//cout << element.first << " :: " << element.second << endl;
		sortedVector.emplace_back(element.first);
	}
	
	return sortedVector;
}

/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Scene::Scene():
	m_rootNode(make_shared<Node>("Root node")) {
		
		uint32 ubo;
		glGenBuffers(1, &ubo);
		m_glLightsUBO = ubo;
}

Scene::Scene(const std::string& path):
	m_rootNode(make_shared<Node>("Root node")) {

		uint32 ubo;
		glGenBuffers(1, &ubo);
		m_glLightsUBO = ubo;
		
	loadFile(path);
}


/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

shared_ptr<Node> Scene::rootNode() const {
	return m_rootNode;
}

//void Scene::rootNode(const std::shared_ptr<Node> node) {
//	m_rootNode = node;
//}

shared_ptr<MaterialProperty> Scene::background() const {
	return m_background;
}

void Scene::background(shared_ptr<MaterialProperty> backgroundProperty) {
	
	if (backgroundProperty->cube()) {
		//auto ambientProperty = make_shared<MaterialProperty>(background);
		auto material = make_shared<Material>(backgroundProperty, nullptr, nullptr, "skybox");
		//material->ambient(background); // this is a hack...

		// generate the skybox geometry if it hasn't already been
		if (!m_skyboxGeometry) {
			m_skyboxGeometry = make_shared<SkyboxGeometry>(material);
		}
		else {
			// we already have the geometry, just update its material
			m_skyboxGeometry->material(material);
		}
	}

	m_background = backgroundProperty;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

unsigned Scene::draw(std::shared_ptr<Node> pointOfView) const {

	unsigned numPolygons = 0;
	
	auto viewMat = pointOfView->worldTransform();
	auto projectionMat = pointOfView->camera()->projection();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (m_background) {
		if (m_background->cube()) {
			mat4 skyboxViewMat = lookAt(vec3(0.0f, 0.0f, 0.0f), // eye - location
										pointOfView->worldForward(), // center - look at
										pointOfView->worldUp()); // up
			
			m_skyboxGeometry->draw(skyboxViewMat, projectionMat);
		}
		else if (m_background->color()) {
			auto color = *(m_background->color());
			glClearColor(color.r, color.g, color.b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
	
	
	
	
	auto allNodes = m_rootNode->allChildNodes();
	
	// clean this up into one helper
	
	shared_ptr<Node> ambientLight = nullptr;
	auto lightsUnsorted = map<shared_ptr<Node>, float>();
	
	// find all lights in the scene and their distances from the camera
	vec3 cameraPos_world = pointOfView->worldPosition();
	for (auto node: allNodes) {
		if (!node->hidden()) {
			auto light = node->light();
			if (light != nullptr) {
				if (light->type() == LightType_Point) {
					auto lightPos_world = node->worldPosition();
					auto lightToCamera = lightPos_world - cameraPos_world;
					auto lightToCameraDistance = length(lightToCamera);
					lightsUnsorted[node] = lightToCameraDistance;
					//cout << "lightToCameraDistance: " << lightToCameraDistance << endl;
				}
				else if (light->type() == LightType_Ambient) {
					ambientLight = node;
				}
			}
		}
	}
	
	auto lightsSorted = SortedLights(lightsUnsorted);
	//cout << "lightsSorted: " << lightsSorted << endl;
	
#define MAX_LIGHTS 7
	
	unsigned endIndex = std::min((unsigned)lightsSorted.size(), (unsigned)(MAX_LIGHTS-1));
	vector<shared_ptr<Node>>::const_iterator first = lightsSorted.begin() + 0;
	vector<shared_ptr<Node>>::const_iterator last = lightsSorted.begin() + endIndex;
	vector<shared_ptr<Node>> lightsSlice(first, last);
	// TODO: if no ambient light we're only using 7 other lights
	lightsSlice.emplace_back(ambientLight);
	//cout << "lightsSlice: " << lightsSlice << endl;
	//cout << "lightsSlice size: " << lightsSlice.size() << endl;
	
	
	
	unsigned numLights = lightsSlice.size();
	LightBlock lightBlock[numLights];
	
	for (int l=0; l<numLights; ++l) {
		auto node = lightsSlice[l];
		auto light = node->light();
		
		lightBlock[l].type = light->type();
		lightBlock[l].position_world = node->worldPosition();
		
//		cout << "lightBlock[" << l << "].position_world: ("
//		<< lightBlock[l].position_world[0] << ", "
//		<< lightBlock[l].position_world[1] << ", "
//		<< lightBlock[l].position_world[2] << ")" << endl;

		
		auto color = *light->color();
		lightBlock[l].color = vec3(color.r, color.g, color.b);
//		cout << "[" << l << "] lightBlock[l].color: ("
//		<< lightBlock[l].color[0] << ", "
//		<< lightBlock[l].color[1] << ", "
//		<< lightBlock[l].color[2] << ")" << endl;
	}

	typedef struct {
		int numLights;
		float PADDING1;
		float PADDING2;
		float PADDING3;
		LightBlock lights[MAX_LIGHTS];
	} LightBlockBlock;
	
	
	
	LightBlockBlock lightsBlockBlock;
	lightsBlockBlock.numLights = numLights;
	memcpy(&lightsBlockBlock.lights, &lightBlock,  sizeof(lightBlock));
	
	
	glBindBuffer(GL_UNIFORM_BUFFER, m_glLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lightsBlockBlock), &lightsBlockBlock, GL_DYNAMIC_DRAW);
	
	
	// setup lights UBO
	
//	const int NUM_LIGHTS = 8;
//
//	LightBlock lightBlock[NUM_LIGHTS];
//
//	lightBlock[0].type = LightType_Ambient;
//	lightBlock[0].color = vec3(0.2, 0.2, 0.2);
//
//	lightBlock[1].type = LightType_Point;
//	lightBlock[1].position_world = vec3(70.0, 70.0, 70.0);
//	lightBlock[1].color = vec3(1.0, 1.0, 1.0);
//
//	lightBlock[2].type = LightType_Point;
//	lightBlock[2].position_world = vec3(-50.0, 50.0, -50.0);
//	lightBlock[2].color = vec3(1.0, 0.0, 0.5);
//
//	lightBlock[3].type = LightType_Point;
//	lightBlock[3].position_world = vec3(40.0, -20.0, -30.0);
//	lightBlock[3].color = vec3(0.0, 0.5, 1.0);
//
//	lightBlock[4].type = LightType_Point;
//	lightBlock[4].position_world = vec3(40.0, -30.0, 100.0);
//	lightBlock[4].color = vec3(0.0, 1.0, 1.0);
//
//	lightBlock[5].type = LightType_Point;
//	lightBlock[5].position_world = vec3(0.0, -500.0, 0.0);
//	lightBlock[5].color = vec3(1.0, 1.0, 0.0);
//
//	lightBlock[6].type = LightType_Point;
//	lightBlock[6].position_world = vec3(-50.0, 50.0, -50.0);
//	lightBlock[6].color = vec3(1.0, 0.0, 1.0);
//
//	lightBlock[7].type = LightType_Point;
//	lightBlock[7].position_world = vec3(60.0, -10.0, 70.0);
//	lightBlock[7].color = vec3(0.0, 0.0, 1.0);
//
//	typedef struct {
//		int numLights;
//		float PADDING1;
//		float PADDING2;
//		float PADDING3;
//		LightBlock lights[8];
//	} LightBlockBlock;
	
//	LightBlockBlock lightsBlockBlock;
//	lightsBlockBlock.numLights = NUM_LIGHTS;
//	memcpy(&lightsBlockBlock.lights, &lightBlock,  sizeof(lightBlock));
	
//	glBindBuffer(GL_UNIFORM_BUFFER, m_glLightsUBO);
//	glBufferData(GL_UNIFORM_BUFFER, sizeof(lightsBlockBlock), &lightsBlockBlock, GL_DYNAMIC_DRAW);

	for (auto node: allNodes) {
		if (!node->hidden()) {
			auto geometry = node->geometry();
			if (geometry != nullptr) {
				auto modelMat = node->worldTransform();
				numPolygons += geometry->draw(modelMat, viewMat, projectionMat, m_glLightsUBO);
			}
		}
	}
	
	return numPolygons;
}

shared_ptr<map<string, vec3>> Scene::boundingPoints() const {

	float maxFloat = numeric_limits<float>::max();
	float minFloat = numeric_limits<float>::min();

	auto boundingPoints = make_shared<map<string, vec3>>();
	(*boundingPoints)["xMin"] = vec3(maxFloat, 0, 0);
	(*boundingPoints)["xMax"] = vec3(minFloat, 0, 0);
	(*boundingPoints)["xMin"] = vec3(0, maxFloat, 0);
	(*boundingPoints)["yMax"] = vec3(0, minFloat, 0);
	(*boundingPoints)["zMin"] = vec3(0, 0, maxFloat);
	(*boundingPoints)["zMax"] = vec3(0, 0, minFloat);

	vector<shared_ptr<Geometry>> geometries;
	for (auto node : m_rootNode->allChildNodes()) {
		if (node->geometry()) {
			geometries.push_back(node->geometry());
		}
	}

	for (auto geometry : geometries) {
		auto points = geometry->boundingPoints();

		if ((*points)["xMin"].x < (*boundingPoints)["xMin"].x) (*boundingPoints)["xMin"] = (*points)["xMin"];
		if ((*points)["xMax"].x > (*boundingPoints)["xMax"].x) (*boundingPoints)["xMax"] = (*points)["xMax"];

		if ((*points)["yMin"].y < (*boundingPoints)["yMin"].y) (*boundingPoints)["yMin"] = (*points)["yMin"];
		if ((*points)["yMax"].y > (*boundingPoints)["yMax"].y) (*boundingPoints)["yMax"] = (*points)["yMax"];

		if ((*points)["zMin"].z < (*boundingPoints)["zMin"].z) (*boundingPoints)["zMin"] = (*points)["zMin"];
		if ((*points)["zMax"].z > (*boundingPoints)["zMax"].z) (*boundingPoints)["zMax"] = (*points)["zMax"];
	}

	//cout << "SCENE boundingPoints: " << endl;
//	for (auto const& x : (*boundingPoints)) {
//		cout << x.first << ": " << x.second << endl;
//	}

	return boundingPoints;
}

vec3 Scene::extent() const{
	
	auto bp = *boundingPoints();
	return vec3(bp["xMax"].x - bp["xMin"].x,
				bp["yMax"].y - bp["yMin"].y,
				bp["zMax"].z - bp["zMin"].z);
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

void Scene::loadFile(const string& importPath) {
	
	cout << "Loading scene: " << importPath << endl;

	unsigned int assimpFlags = aiProcess_Triangulate
		| aiProcess_SortByPType
		| aiProcess_GenSmoothNormals
		// "This will, in fact, reduce the number of draw calls."
	    // http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410af5fe0d6ee720c91359dc61cb849f2ebf
		| aiProcess_OptimizeMeshes
		// "If this flag is not specified, no vertices are referenced by more than one face and no index buffer is required for rendering."
		// http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410a444a6c9d8b63e6dc9e1e2e1edd3cbcd4
		| aiProcess_JoinIdenticalVertices
		| aiProcess_ImproveCacheLocality
		| aiProcess_ValidateDataStructure;

	const aiScene* scene = aiImportFile(importPath.c_str(), assimpFlags);
	
	if (scene) {
		
		// copy all the meshes and materials out of the aiScene
		// use them to construct our GeometryElements
		// (we are not keeping a master list)
		auto importElements = vector<shared_ptr<GeometryElement>>();
		auto importMaterials = vector<shared_ptr<Material>>();
		
		// ********** meshes (ae::GeometryElement) **********
		
		int numMeshes = scene->mNumMeshes;
		for (int m=0; m<numMeshes; ++m) {
			aiMesh *mesh = scene->mMeshes[m];
			
			// should be set for parent Geometry
			aiString name = mesh->mName;
			if (strcmp(name.C_Str(), "") != 0) {
				cout << "mName: " << name.C_Str() << endl;
			}
			
			auto verts = vector<Vertex>();

			bool hasNormals = mesh->HasNormals();
			bool hasTextureCoordinates = mesh->HasTextureCoords(0);
			
			unsigned int numVerts = mesh->mNumVertices;
			for (unsigned int v=0; v<numVerts; ++v) {
				aiVector3D position = mesh->mVertices[v];
				aiVector3D normal = aiVector3D(0, 0, 0);
				aiVector3D texCoord = aiVector3D(0, 0, 0);
				
				if (hasNormals) normal = mesh->mNormals[v];
				if (hasTextureCoordinates) texCoord = mesh->mTextureCoords[0][v];
				
				Vertex vert = {AIVector3DToGLMVec3(position),
							   AIVector3DToGLMVec3(normal),
							   vec2(texCoord.x, texCoord.y)};
				verts.push_back(vert);
			}

			auto faces = vector<Face>();
			unsigned int numFaces = mesh->mNumFaces;
			for (unsigned int f=0; f<numFaces; ++f) {
				aiFace face = mesh->mFaces[f];
				faces.push_back({face.mIndices[0], face.mIndices[1], face.mIndices[2]});
			}
			
			auto element = make_shared<GeometryElement>(verts, faces);
			//geometryElements().push_back(element);
			importElements.push_back(element);
		}
		

		// ********** materials **********
		
		cout << "mNumMaterials: " << scene->mNumMaterials << endl;
		for (unsigned int m=0; m<scene->mNumMaterials; ++m) {
			printf("material[%d]\n", m);
			aiMaterial* aiMaterial = scene->mMaterials[m];

			string basePath = path(importPath).parent_path().string();
			
			auto ambientProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_AMBIENT, basePath);
			auto diffuseProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_DIFFUSE, basePath);
			auto specularProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_SPECULAR, basePath);
			
			auto material = make_shared<Material>(ambientProperty, diffuseProperty, specularProperty);
			
			// specular exponent
			// https://www.mathworks.com/matlabcentral/mlc-downloads/downloads/
			// submissions/27982/versions/5/previews/help%20file%20format/MTL_format.html
			float shininess = 0;
			if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
				cout << "Specular exponent: " << shininess << endl;
			}
			material->specularExponent(shininess);
			
			aiString name;
			if (aiMaterial->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
				if (strcmp(name.C_Str(), "") != 0) {
					cout << "Name: " << name.C_Str() << endl;
					material->name(name.C_Str());
				}
			}

			importMaterials.push_back(material);
		}
		
		// ********** nodes (ae::Geometry) **********
		
		// in AI terminology, a "node" is what we call a "geometry"
		// also in AI terminology, a "mesh" is what we call a "geometry element"
		
		addAIGeometryNodes(scene, m_rootNode, importElements, importMaterials);

		// ********** lights **********

		for (unsigned int l=0; l<scene->mNumLights; --l) {
			break; // disable light importing, we'll do it ourselves!
			
			aiLight* aiLight = scene->mLights[l];
			
			Color color = AIColor3DToColor(aiLight->mColorDiffuse);

			auto light = make_shared<Light>(LightTypeForAILightType(aiLight->mType),
											make_shared<Color>(color));

			aiString name = aiLight->mName;
			if (strcmp(name.C_Str(), "") != 0) {
				cout << "Name: " << name.C_Str() << endl;
				light->name(name.C_Str());
			}
			
			cout << "Adding light: " << light << endl;
			
			auto lightNode = make_shared<Node>("Light");
			lightNode->light(light);
			rootNode()->addChildNode(lightNode);
		}

		// ********** cameras **********
		
		for (unsigned int c=0; c<scene->mNumCameras; --c) {
			break; // disable camera importing, we'll do it ourselves!

			aiCamera* aiCamera = scene->mCameras[c];
			
			auto camera = make_shared<Camera>(aiCamera->mClipPlaneNear,
											  aiCamera->mClipPlaneFar,
											  aiCamera->mHorizontalFOV);
			
			aiString name = aiCamera->mName;
			if (strcmp(name.C_Str(), "") != 0) {
				cout << "Name: " << name.C_Str() << endl;
				camera->name(name.C_Str());
			}
			
			cout << "Adding camera: " << camera << endl;

			auto cameraNode = make_shared<Node>("Camera");
			cameraNode->camera(camera);

			aiNode* aiCamNode = scene->mRootNode->FindNode(aiCamera->mName);

			auto viewMat = AIMaxtrix4x4ToGLMMat4(aiCamNode->mTransformation);

			cameraNode->transform(viewMat);

			rootNode()->addChildNode(cameraNode);
		}
	}
	else {
		cout << "Error importing mesh: " << aiGetErrorString() << endl;
	}
	
	aiReleaseImport(scene);
}

void Scene::addAIGeometryNodes(const aiScene* aiScene,
							   shared_ptr<Node> aeRootNode,
							   const vector<shared_ptr<GeometryElement>>& importElements,
							   const vector<shared_ptr<Material>>& importMaterials) {
	aiNode* aiRootGeometryNode = aiScene->mRootNode;
	unsigned int nChildren = aiRootGeometryNode->mNumChildren;
	for (unsigned int i=0; i<nChildren; ++i) {
		aiNode* child = (aiRootGeometryNode->mChildren)[i];
		addAIGeometryNodeRec(aiScene, child, aeRootNode, importElements, importMaterials);
	}
}

void Scene::addAIGeometryNodeRec(const aiScene* aiScene,
								 const aiNode* aiGeometryNode,
								 shared_ptr<Node> aeParentNode,
								 const vector<shared_ptr<GeometryElement>>& importElements,
								 const vector<shared_ptr<Material>>& importMaterials) {

	string name = aiGeometryNode->mName.C_Str();
	
//	aiString name = aiGeometryNode->mName;
//	if (strcmp(name.C_Str(), "") != 0) {
//		cout << "Name: " << name.C_Str() << endl;
//	}
	
	mat4 transform = AIMaxtrix4x4ToGLMMat4(aiGeometryNode->mTransformation);
	cout << "Adding '" << name << "' with transform: " << endl;
	cout << transform << endl;
	
	auto elements = vector<shared_ptr<GeometryElement>>();
	auto materials = vector<shared_ptr<Material>>();
	int numMeshes = aiGeometryNode->mNumMeshes;
	cout << "numMeshes: " << numMeshes << endl;
	for (int m=0; m<numMeshes; ++m) {
		cout << "Reading mesh " << m << endl;
		
		unsigned int meshIndex = aiGeometryNode->mMeshes[m];
		auto element = importElements[meshIndex];
		elements.push_back(element);

		unsigned int materialIndex = aiScene->mMeshes[meshIndex]->mMaterialIndex;
		if (importMaterials.size() && (importMaterials.size()-1 >= materialIndex)) {
			cout << "importMaterials count: " << importMaterials.size() << endl;
			auto material = importMaterials[materialIndex];
			materials.push_back(material);
		}
	}

	// *** some nodes only have cameras and lights, which we are throwing out.
	// so don't add a node with nothing in it. ***

	shared_ptr<Node> newNode = nullptr;

	if (numMeshes > 0) {
		cout << "Adding node WITH geometry..." << endl;
		auto geometry = make_shared<Geometry>(elements, materials);
		geometry->name(name);

		newNode = make_shared<Node>(name, transform, geometry);
		aeParentNode->addChildNode(newNode);
	}
	else {
		cout << "Adding node WITHOUT geometry..." << endl;
		newNode = make_shared<Node>(name, transform);
		aeParentNode->addChildNode(newNode);
	}

	unsigned int nChildren = aiGeometryNode->mNumChildren;
	for (unsigned int i = 0; i < nChildren; ++i) {
		aiNode *child = (aiGeometryNode->mChildren)[i];
		addAIGeometryNodeRec(aiScene, child, newNode, importElements, importMaterials);
	}
}
