//
//  Scene.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Scene.h"

#include <iostream>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <boost/filesystem.hpp>
//#include <boost/optional.hpp>

#include "Camera.h"
#include "Color.h"
#include "Geometry.h"
#include "GeometryElement.h"
#include "Image.h"
#include "Light.h"
#include "Material.h"
#include "MaterialProperty.h"
#include "Node.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace Assimp;
//using namespace boost;
using namespace boost::filesystem;
using namespace glm;
using namespace std;


/***************************************************************************************
     MARK:   Static
 **************************************************************************************/

static string FilepathFromTextureFilename(const string& filename, const string& basePath) {

	string textureName = filename;
	if (textureName.substr(0,1) == "/") {
		textureName = textureName.substr(1, textureName.length()-1);
	}
	else if (textureName.substr(0,2) == "./") {
		textureName = textureName.substr(2, textureName.length()-2);
	}
	
	path texturePath = canonical(path(textureName), path(basePath));
	
	return texturePath.string();
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
			string texturePath = FilepathFromTextureFilename(filename.C_Str(), basePath);
			cout << "Texture path: " << texturePath << endl;
			auto textureImage = make_shared<Image>(texturePath);
			return make_shared<MaterialProperty>(textureImage);
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


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Scene::Scene():
	m_rootNode(make_shared<Node>("Root node")),
	m_geometryElements(vector<shared_ptr<GeometryElement>>()),
	m_materials(vector<shared_ptr<Material>>()) {

}

Scene::Scene(const std::string& path):
	m_rootNode(make_shared<Node>("Root node")),
	m_geometryElements(vector<shared_ptr<GeometryElement>>()),
	m_materials(vector<shared_ptr<Material>>()) {

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

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

vector<shared_ptr<GeometryElement>>& Scene::geometryElements() {
	return m_geometryElements;
}

vector<shared_ptr<Material>>& Scene::materials() {
	return m_materials;
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
	for (auto node : rootNode()->allChildNodes()) {
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

	cout << "SCENE boundingPoints: " << endl;
	for (auto const& x : (*boundingPoints)) {
		cout << x.first << ": " << x.second << endl;
	}

	return boundingPoints;
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
		
		// ********** meshes (ae::GeometryElement) **********
		
		int numMeshes = scene->mNumMeshes;
		for (int m=0; m<numMeshes; ++m) {
			aiMesh *mesh = scene->mMeshes[m];
			
			aiString name = mesh->mName;
			if (strcmp(name.C_Str(), "") != 0) {
				cout << "mName: " << name.C_Str() << endl;
			}
			
			auto verts = vector<Vertex>();
			unsigned int numUVChannels = mesh->GetNumUVChannels();
			cout << "numUVChannels: " << numUVChannels << endl;
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
			geometryElements().push_back(element);
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
			m_materials.push_back(material);
		}
		
		cout << "Import num materials: " << m_materials.size() << endl;

		// ********** nodes (ae::Geometry) **********
		
		// in AI terminology, a "node" is what we call a "geometry"
		// also in AI terminology, a "mesh" is what we call a "geometry element"
		
		addAIGeometryNodes(scene, rootNode());

		// ********** lights **********

		for (unsigned int l=0; l<scene->mNumLights; --l) {
			break; // disable light importing, we'll do it ourselves!

			auto light = make_shared<Light>();
			auto lightNode = make_shared<Node>("Light");
			lightNode->light(light);
			rootNode()->addChildNode(lightNode);
			cout << "Adding light: " << light << endl;
		}

		// ********** cameras **********
		
		for (unsigned int c=0; c<scene->mNumCameras; --c) {
			break; // disable camera importing, we'll do it ourselves!

			aiCamera* aiCamera = scene->mCameras[c];

			auto camera = make_shared<Camera>(aiCamera->mClipPlaneNear,
											  aiCamera->mClipPlaneFar,
											  aiCamera->mHorizontalFOV);
			auto cameraNode = make_shared<Node>("Camera");
			cameraNode->camera(camera);

			aiNode* aiCamNode = scene->mRootNode->FindNode(aiCamera->mName);

			auto viewMat = AIMaxtrix4x4ToGLMMat4(aiCamNode->mTransformation);

			cameraNode->transform(viewMat);

			rootNode()->addChildNode(cameraNode);
			cout << "Adding camera: " << camera << endl;
		}
	}
	else {
		cout << "Error importing mesh: " << aiGetErrorString() << endl;
	}
	
	aiReleaseImport(scene);
}

void Scene::addAIGeometryNodes(const aiScene* aiScene, shared_ptr<Node> aeRootNode) {
	aiNode* aiRootGeometryNode = aiScene->mRootNode;
	unsigned int nChildren = aiRootGeometryNode->mNumChildren;
	for (unsigned int i=0; i<nChildren; ++i) {
		aiNode* child = (aiRootGeometryNode->mChildren)[i];
		addAIGeometryNodeRec(aiScene, child, aeRootNode);
	}
}

void Scene::addAIGeometryNodeRec(const aiScene* aiScene,
								 const aiNode* aiGeometryNode,
								 shared_ptr<Node> aeParentNode) {
	
	aiMetadata* md = aiGeometryNode->mMetaData;
	if (md) {
		// not sure what this is for yet
		cout << md->mKeys << endl;
	}

	string name = aiGeometryNode->mName.C_Str();
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
		auto element = m_geometryElements[meshIndex];
		elements.push_back(element);

		unsigned int materialIndex = aiScene->mMeshes[meshIndex]->mMaterialIndex;
		if (m_materials.size() && (m_materials.size()-1 >= materialIndex)) {
			cout << "m_materials count: " << m_materials.size() << endl;
			auto material = m_materials[materialIndex];
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
		addAIGeometryNodeRec(aiScene, child, newNode);
	}
}
