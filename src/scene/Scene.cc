//
//  Scene.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "scene/Scene.h"

#include <filesystem>
#include <optional>

#ifndef ANDROID
#include "assimp-3.3.1/include/assimp/cimport.h"
#include "assimp-3.3.1/include/assimp/postprocess.h"
#include "assimp-3.3.1/include/assimp/scene.h"
#include "assimp-3.3.1/include/assimp/version.h"
#endif

#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "geometry/GeometryElement.h"
#include "geometry/primitives/Box.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsWorld.h"
#include "physics/bullet/BulletPhysicsSimulator.h"
#include "rendering/Light.h"
#include "rendering/Renderer.h"
#include "rendering/context/RenderContext.h"
#include "rendering/camera/PerspectiveCamera.h"
#include "rendering/materials/Material.h"
#include "rendering/materials/MaterialProperty.h"
#include "scene/Node.h"
#include "utilities/Image.h"
#include "utilities/Utilities.h"
#include "utilities/Color.h"
#include "utilities/CubeImage.h"


using namespace ae;
using namespace ae::utils;
#ifndef ANDROID
using namespace Assimp;
#endif
using namespace std::filesystem;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

//static shared_ptr<Geometry> 		MakeSkyboxGeometry(shared_ptr<MaterialProperty> materialProperty);
static shared_ptr<Image> 			MissingTextureImage();
#ifndef ANDROID
static void 						LoadFile(Scene& scene, const filesystem::path& importPath);
//static void 						LoadData(Scene& scene, const vector<unsigned char>& data);
static void 						AddAIGeometryNodes(Scene& scene,
													  const aiScene* aiScene,
													  shared_ptr<Node> aeRootNode,
													  const vector<shared_ptr<GeometryElement>>& importElements,
													  const vector<shared_ptr<Material>>& importMaterials);
static void 						AddAIGeometryNodeRec(Scene& scene,
														const aiScene* aiScene,
														const aiNode* aiGeometryNode,
														shared_ptr<Node> aeParentNode,
														const vector<shared_ptr<GeometryElement>>& importElements,
														const vector<shared_ptr<Material>>& importMaterials);
static shared_ptr<MaterialProperty> MaterialPropertyFromAIMaterial(const aiMaterial* aiMaterial,
																   aiTextureType type,
																   string basePath);
static optional<path> 			FilepathFromTextureFilename(const string& filename,
																  const string& basePath);
static LIGHT_TYPE 					LightTypeForAILightType(aiLightSourceType aiType);

static vec2 						GLMVec2FromAIVector3D(const aiVector2D& from);
static vec3 						GLMVec3FromAIVector3D(const aiVector3D& from);
static mat4 						GLMMat4FromAIMaxtrix4x4(const aiMatrix4x4& from);
static Color 						ColorFromAIColor3D(const aiColor3D& from);
static Color 						ColorFromAIColor4D(const aiColor4D& from);

#endif // !ANDROID

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

#ifndef ANDROID
shared_ptr<Scene> Scene::LoadFromFile(const filesystem::path& path) {
	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));
	LoadFile(*scene, path);
	return scene;
}
#endif

//shared_ptr<Scene> Scene::LoadFromData(const vector<unsigned char>& data) {
//	auto scene = make_shared<Scene>();
//	scene->rootNode(make_shared<Node>("Root node"));
//	LoadData(*scene, data);
//	return scene;
//}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Scene::Scene():
	_rootNode(nullptr),
//	_background(nullptr),
//	_fogStartDistance(0.0),
//	_fogEndDistance(0.0),
//	_fogDensityExponent(0.0),
//	_fogColor(nullptr),
	_physicsWorld(nullptr)
/*_renderContext({})*/ { }

Scene::Scene(shared_ptr<VisualWorld> visualWorld, shared_ptr<PhysicsWorld> physicsWorld):
		_rootNode(make_shared<Node>("root")),
		_visualWorld(visualWorld),
		_physicsWorld(physicsWorld) { }

Scene::~Scene() {
	AE_LOG_D("Destroying Scene {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<Node> Scene::rootNode() const {
	return _rootNode;
}

void Scene::rootNode(shared_ptr<Node> node) {
//	node->attachedToScene(shared_from_this());
	_rootNode = node;
}

//shared_ptr<MaterialProperty> Scene::background() const {
//	return _background;
//}
//
//void Scene::background(shared_ptr<MaterialProperty> backgroundProperty) {
//
//	if (dynamic_pointer_cast<CubeImage>(backgroundProperty->contents())) {
//		auto material = make_shared<Material>(nullptr, nullptr, nullptr, backgroundProperty);
//
//		material->emissive()->wrapS(WRAP_MODE::CLAMP_TO_EDGE);
//		material->emissive()->wrapT(WRAP_MODE::CLAMP_TO_EDGE);
//		material->emissive()->wrapR(WRAP_MODE::CLAMP_TO_EDGE);
//
//		// generate the skybox geometry if it hasn't already been
//		if (!_skyboxGeometry) {
//			// MKD: u_s_ptr_aliases
//			_skyboxGeometry = MakeSkyboxGeometry(backgroundProperty);
//		}
//		else {
//			// we already have the geometry, just update its material
//			_skyboxGeometry->replaceMaterial(0, material);
//		}
//	}
//
//	_background = backgroundProperty;
//}
//
//float Scene::fogStartDistance() const {
//	return _fogStartDistance;
//}
//
//void Scene::fogStartDistance(float distance) {
//	_fogStartDistance = distance;
//}
//
//float Scene::fogEndDistance() const {
//	return _fogEndDistance;
//}
//
//void Scene::fogEndDistance(float distance) {
//	_fogEndDistance = distance;
//}
//
//float Scene::fogDensityExponent() const {
//	return _fogDensityExponent;
//}
//
//void Scene::fogDensityExponent(float exponent) {
//	_fogDensityExponent = exponent;
//}
//
//shared_ptr<Color> Scene::fogColor() const {
//	return _fogColor;
//}
//
//void Scene::fogColor(shared_ptr<Color> color) {
//	_fogColor = color;
//}

std::shared_ptr<VisualWorld> Scene::visualWorld() const {
	return _visualWorld;
}

void Scene::visualWorld(std::shared_ptr<VisualWorld> world) {
	_visualWorld = world;
}

shared_ptr<PhysicsWorld> Scene::physicsWorld() const {
	return _physicsWorld;
}

void Scene::physicsWorld(shared_ptr<PhysicsWorld> world) {
	_physicsWorld = world;
//	_physicsWorld->attachedToScene(shared_from_this());
}

shared_ptr<InputManager> Scene::inputManager() {
	if (_inputManager == nullptr) {
		shared_ptr<Window> window = static_pointer_cast<Window>(shared_from_this());
		auto inputManager = make_shared<WindowInputManager>(window);
		_inputManager = static_pointer_cast<InputManager>(inputManager);
	}
	return _inputManager;
}

void Scene::inputManager(std::shared_ptr<InputManager> inputManager) {

}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void Scene::update(Renderer& renderer,
				   unsigned framebufferWidth,
				   unsigned framebufferHeight,
				   Node& pointOfView,
				   const DEBUG_OPTIONS& debugOptions,
				   FrameStats& stats) {

	renderer.render(*this, debugOptions, stats);

	auto renderContext = _renderContext.lock();
	auto physicsSimulator = renderContext->physicsSimulator();
	static auto visited = map<shared_ptr<Node>, bool>();

	if (_physicsWorld) {

		physicsSimulator->beginUpdate(*this);
		physicsSimulator->update(*this);
	}

	visited.clear();
	_rootNode->update(*physicsSimulator,
					  stats,
					  visited);

	if (_physicsWorld) {

		physicsSimulator->step(_renderContext.lock()->sceneTime());

		physicsSimulator->sync(*this);
	}

	visited.clear();
	_rootNode->sync(*physicsSimulator,
					stats,
					visited);

	if (_physicsWorld) {

		physicsSimulator->endUpdate(*this);

		if (renderContext->didSimulatePhysicsCallback()) {
			(renderContext->didSimulatePhysicsCallback())(*renderContext, renderContext->sceneTime());
		}
	}
	
	auto viewMat = pointOfView.worldTransform();
	auto projectionMat = pointOfView.camera()->projection();

	visited.clear();
	_rootNode->draw(renderer,
					viewMat,
					projectionMat,
					debugOptions,
					stats,
					visited);

	if (_physicsWorld) {

		auto bulletSimulator = dynamic_pointer_cast<BulletPhysicsSimulator>(physicsSimulator);
		if (bulletSimulator) {
			bulletSimulator->drawDebug(renderer,
									   viewMat,
									   projectionMat,
									   debugOptions);
		}
	}
}

//shared_ptr<Geometry> Scene::skyboxGeometry() const {
//	return _skyboxGeometry;
//}

//AABB Scene::aabb() const {
//
////	static const float maxFloat = numeric_limits<float>::max();
////	static const float minFloat = numeric_limits<float>::min();
////
////	AABB aabb = { {maxFloat, maxFloat, maxFloat},
////				  {minFloat, minFloat, minFloat} };
////
////	for (const auto node : _rootNode->children(true)) {
////		if (node->geometry()) {
////			auto geoAABB = node->geometry()->aabb(node);
////			aabb.min.x = std::min(aabb.min.x, geoAABB.min.x);
////			aabb.max.x = std::max(aabb.max.x, geoAABB.max.x);
////			aabb.min.y = std::min(aabb.min.y, geoAABB.min.y);
////			aabb.max.y = std::max(aabb.max.y, geoAABB.max.y);
////			aabb.min.z = std::min(aabb.min.z, geoAABB.min.z);
////			aabb.max.z = std::max(aabb.max.z, geoAABB.max.z);
////		}
////	}
////
////	return aabb;
//
//	return _rootNode->aabb();
//}

//vec3 Scene::extent() const {
//	auto aabb = _rootNode->aabb();
//	return {aabb.max.x - aabb.min.x,
//			aabb.max.y - aabb.min.y,
//			aabb.max.z - aabb.min.z};
//}

//void Scene::attachedToRenderContext(shared_ptr<RenderContext> renderContext) {
//	_renderContext = renderContext;
////	if (_physicsWorld) {
////		_physicsWorld->attachedToScene(shared_from_this());
////	}
//}

//weak_ptr<RenderContext> Scene::renderContext() const {
//	return _renderContext;
//}
//
//void Scene::renderContext(shared_ptr<RenderContext> context) {
//	_renderContext = context;
//}

/*********************************************************************************************
	Static
 *********************************************************************************************/

static shared_ptr<Geometry> MakeSkyboxGeometry(shared_ptr<MaterialProperty> materialProperty) {
	
	auto geometry = make_shared<Box>(1, 1, 1);
	auto material = make_shared<Material>(nullptr, nullptr, nullptr, materialProperty);
	geometry->insertMaterial(material, 0);
	
	return geometry;
}

static shared_ptr<Image> MissingTextureImage() {
	static shared_ptr<Image> image = nullptr;
	if (!image) {
		image = ImageNamed("missing_texture", "png");
//		auto imagesDir = ImagesDirectory();
//		if (imagesDir) {
//			auto imagePath = *imagesDir / "missing_texture.png";
//			image = make_shared<Image>(imagePath.string());
//		}
//		else {
//			AE_LOG_W("Couldn't locate images directory.");
//		}
	}
	return image;
}

#ifndef ANDROID
static void LoadFile(Scene& scene, const filesystem::path& importPath) {
	
	AE_LOG_I("Assimp version: {}.{}.{}",
				 aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
	
	AE_LOG_I("Loading scene: {}", importPath.string());
	
	unsigned int assimpFlags = aiProcess_Triangulate
	| aiProcess_SortByPType
	| aiProcess_GenSmoothNormals
	// "This will, in fact, reduce the number of update calls."
	// http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410af5fe0d6ee720c91359dc61cb849f2ebf
	| aiProcess_OptimizeMeshes
	// "If this flag is not specified, no vertices are referenced by more than one face and no index buffer is required for rendering."
	// http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410a444a6c9d8b63e6dc9e1e2e1edd3cbcd4
	| aiProcess_JoinIdenticalVertices
	| aiProcess_ImproveCacheLocality
	| aiProcess_ValidateDataStructure;
	
	const aiScene* aiScene = aiImportFile(importPath.string().c_str(), assimpFlags);
	
	if (aiScene) {
		
		// copy all the meshes and materials out of the aiScene
		// use them to construct our GeometryElements
		// (we are not keeping a master list)
		auto importElements = vector<shared_ptr<GeometryElement>>();
		auto importMaterials = vector<shared_ptr<Material>>();
		
		// ********** meshes (GeometryElement) **********
		
		int numMeshes = aiScene->mNumMeshes;
		for (int m=0; m<numMeshes; ++m) {
			AE_LOG_D("Processing mesh {}...:", m);
			
			aiMesh *mesh = aiScene->mMeshes[m];
			
			// should be set for parent Geometry
			aiString name = mesh->mName;
			if (strcmp(name.C_Str(), "") != 0) {
				AE_LOG_D("Mesh name: {}", name.C_Str());
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
				
				Vertex vert = {GLMVec3FromAIVector3D(position),
					GLMVec3FromAIVector3D(normal),
					vec2(texCoord.x, texCoord.y)};
				verts.push_back(vert);
			}
			
			auto faces = vector<Face>();
			unsigned int numFaces = mesh->mNumFaces;
			for (unsigned int f=0; f<numFaces; ++f) {
				aiFace face = mesh->mFaces[f];
				faces.push_back({static_cast<int>(face.mIndices[0]),
					static_cast<int>(face.mIndices[1]),
					static_cast<int>(face.mIndices[2])});
			}
			
			auto element = make_shared<GeometryElement>(verts, faces);
			//geometryElements().push_back(element);
			importElements.push_back(element);
		}
		
		
		// ********** materials **********
		
		AE_LOG_D("Number of materials: {}", aiScene->mNumMaterials);
		
		for (unsigned int m=0; m < aiScene->mNumMaterials; ++m) {
			
			AE_LOG_D("Processing material {}...:", m);
			
			aiMaterial* aiMaterial = aiScene->mMaterials[m];
			
			string basePath = path(importPath).parent_path().string();
			
			auto ambientProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_AMBIENT, basePath);
			auto diffuseProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_DIFFUSE, basePath);
			auto specularProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_SPECULAR, basePath);
			// not sure why, but some models have emissive colors that are messing everything up...
			//auto emissiveProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_EMISSIVE, basePath);
			
			auto material = make_shared<Material>(ambientProperty, diffuseProperty, specularProperty);
			//material->emissive(emissiveProperty);
			
			aiString name;
			if (aiMaterial->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
				if (strcmp(name.C_Str(), "") != 0) {
					AE_LOG_D("Name: {}", name.C_Str());
					material->name(name.C_Str());
					if (*(material->name()) == AI_DEFAULT_MATERIAL_NAME) {
						// https://sourceforge.net/p/assimp/discussion/817654/thread/0729fb73/
						// it appears that OBJ add a "default material". donno why. it doesn't get used
						// and thus the created MaterialProperties and Material will be deallocated after import
						AE_LOG_I("AI_DEFAULT_MATERIAL_NAME");
					}
				}
			}
			
			// specular exponent
			// https://www.mathworks.com/matlabcentral/mlc-downloads/downloads/
			// submissions/27982/versions/5/previews/help%20file%20format/MTL_format.html
			float shininess = 0;
			if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
				AE_LOG_D("Specular exponent: {}", shininess);
			}
			material->specularExponent(shininess);
			
			AE_LOG_D("ADDING MATERIAL: {:p}", (void*)material.get());
			importMaterials.push_back(material);
		}
		
		AE_LOG_D("DONE WITH MATERIALS. COUNT: {}", importMaterials.size());
		
		// ********** nodes (Geometry) **********
		
		// in AI terminology, a "node" is what we call a "geometry"
		// also in AI terminology, a "mesh" is what we call a "geometry element"
		
		auto filename = importPath.filename().string();
		scene.rootNode()->name(filename + " ROOT");
		
		AddAIGeometryNodes(scene, aiScene, scene.rootNode(), importElements, importMaterials);
		
		// ********** lights **********
		
		for (unsigned int l=0; l<aiScene->mNumLights; --l) {
			break; // disabling for now...
			
			aiLight* aiLight = aiScene->mLights[l];
			
			Color color = ColorFromAIColor3D(aiLight->mColorDiffuse);
			
			auto light = make_shared<Light>(LightTypeForAILightType(aiLight->mType),
											make_shared<Color>(color));
			
			aiString name = aiLight->mName;
			if (strcmp(name.C_Str(), "") != 0) {
				light->name(name.C_Str());
			}
			
			//cout << "Adding light: " << light << endl;
			
			auto lightNode = make_shared<Node>("Light");
			lightNode->light(light);
			//rootNode()->addChild(lightNode);
			scene.rootNode()->addChild(lightNode);
		}
		
		// ********** cameras **********
		
		for (unsigned int c=0; c<aiScene->mNumCameras; --c) {
			break; // disabling for now...
			
			aiCamera* aiCamera = aiScene->mCameras[c];

			auto camera = make_shared<PerspectiveCamera>(aiCamera->mName.C_Str(),
														 aiCamera->mClipPlaneNear,
														 aiCamera->mClipPlaneFar,
														 aiCamera->mHorizontalFOV);
			
			aiString name = aiCamera->mName;
			if (strcmp(name.C_Str(), "") != 0) {
				camera->name(name.C_Str());
			}
			
			//cout << "Adding camera: " << camera << endl;
			
			auto cameraNode = make_shared<Node>("Camera");
			cameraNode->camera(camera);
			
			aiNode* aiCamNode = aiScene->mRootNode->FindNode(aiCamera->mName);
			
			auto viewMat = GLMMat4FromAIMaxtrix4x4(aiCamNode->mTransformation);
			
			cameraNode->transform(viewMat);
			
			scene.rootNode()->addChild(cameraNode);
		}
	}
	else {
		//AE_LOG_E("Error importing scene: {}", aiGetErrorString());
		
		char errMsg[1024];
		sprintf(errMsg, "Error importing scene: %s\n",  aiGetErrorString());
		throw Exception(errMsg);
	}
	
	aiReleaseImport(aiScene);
}

static void AddAIGeometryNodes(Scene& scene,
							   const aiScene* aiScene,
							   shared_ptr<Node> aeRootNode,
							   const vector<shared_ptr<GeometryElement>>& importElements,
							   const vector<shared_ptr<Material>>& importMaterials) {

	aiNode* aiRootGeometryNode = aiScene->mRootNode;
	unsigned int nChildren = aiRootGeometryNode->mNumChildren;
	for (unsigned int i=0; i<nChildren; ++i) {
		aiNode* child = (aiRootGeometryNode->mChildren)[i];
		AddAIGeometryNodeRec(scene, aiScene, child, aeRootNode, importElements, importMaterials);
	}
}

static void AddAIGeometryNodeRec(Scene& scene,
								 const aiScene* aiScene,
								 const aiNode* aiGeometryNode,
								 shared_ptr<Node> aeParentNode,
								 const vector<shared_ptr<GeometryElement>>& importElements,
								 const vector<shared_ptr<Material>>& importMaterials) {

	string name = aiGeometryNode->mName.C_Str();
	
	mat4 transform = GLMMat4FromAIMaxtrix4x4(aiGeometryNode->mTransformation);
	
	AE_LOG_D("Adding '{}' with transform:\n{}", name, StringFromGLMMat4(transform));
	
	auto elements = vector<shared_ptr<GeometryElement>>();
	auto materials = vector<shared_ptr<Material>>();
	int numMeshes = aiGeometryNode->mNumMeshes;
	
	AE_LOG_D("Number of meshes: {}", numMeshes);
	
	for (int m=0; m<numMeshes; ++m) {
		AE_LOG_D("Reading mesh {}...", m);
		
		unsigned int meshIndex = aiGeometryNode->mMeshes[m];
		auto element = importElements[meshIndex];
		elements.push_back(element);
		
		unsigned int materialIndex = aiScene->mMeshes[meshIndex]->mMaterialIndex;
		if (importMaterials.size() && (importMaterials.size()-1 >= materialIndex)) {
			auto material = importMaterials[materialIndex];
			AE_LOG_D("Adding material at index: {}", materialIndex);
			materials.push_back(material);
		}
	}
	
	// *** some nodes only have cameras and lights, which we are throwing out.
	// so don't add a node with nothing in it. ***
	
	shared_ptr<Node> newNode = nullptr;
	
	if (numMeshes > 0) {
		AE_LOG_D("Adding node WITH geometry...");
		auto geometry = make_shared<Geometry>(elements, materials);
		geometry->name(name);
		
		//newNode = make_shared<Node>(name, transform, geometry);
		newNode = make_shared<Node>(name);
		newNode->transform(transform);
		newNode->geometry(geometry);
		aeParentNode->addChild(newNode);
	}
	else {
		AE_LOG_D("Adding node WITHOUT geometry...");
		//newNode = make_shared<Node>(name, transform);
		newNode = make_shared<Node>(name);
		newNode->transform(transform);
		aeParentNode->addChild(newNode);
	}
	
	unsigned int nChildren = aiGeometryNode->mNumChildren;
	for (unsigned int i = 0; i < nChildren; ++i) {
		aiNode *child = (aiGeometryNode->mChildren)[i];
		AddAIGeometryNodeRec(scene, aiScene, child, newNode, importElements, importMaterials);
	}
}

static shared_ptr<MaterialProperty> MaterialPropertyFromAIMaterial(const aiMaterial* aiMaterial,
																   aiTextureType type,
																   string basePath) {
	// TODO: Clean up
	// this is hacky...
	//#define AI_MATKEY_COLOR_DIFFUSE "$clr.diffuse",0,0
	//#define AI_MATKEY_COLOR_AMBIENT "$clr.ambient",0,0
	//#define AI_MATKEY_COLOR_SPECULAR "$clr.specular",0,0
	//#define AI_MATKEY_COLOR_EMISSIVE "$clr.emissive",0,0
	
	char colorType[1024];
	string typeStr = "";
	switch (type) {
		case aiTextureType_AMBIENT:
			strcpy(colorType, "$clr.ambient");
			typeStr = "ambient";
			break;
		case aiTextureType_DIFFUSE:
			strcpy(colorType, "$clr.diffuse");
			typeStr = "diffuse";
			break;
		case aiTextureType_SPECULAR:
			strcpy(colorType, "$clr.specular");
			typeStr = "specular";
			break;
		case aiTextureType_EMISSIVE:
			strcpy(colorType, "$clr.emissive");
			typeStr = "emissive";
			break;
		default:
			AE_LOG_W("Unsupported material type: {}", type);
			return nullptr;
	}
	
	AE_LOG_I("Reading {} material...", typeStr);
	
	if (aiMaterial->GetTextureCount(type)) { // texture
		
		aiString filename;
		if (aiMaterial->GetTexture(type, 0, &filename,
								   NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			auto texturePath = FilepathFromTextureFilename(filename.C_Str(), basePath);
			if (texturePath) {
				AE_LOG_D("Texture path: {}", texturePath->string());
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
		if (aiMaterial->Get(colorType, 0, 0, aiColor) == AI_SUCCESS) {
			auto aeColor = make_shared<Color>(ColorFromAIColor4D(aiColor));
			AE_LOG_D("Color: {}", StringFromColor(*aeColor));
			return make_shared<MaterialProperty>(aeColor);
		}
		else {
			AE_LOG_D("No {} material...", typeStr);
		}
	}
	
	return nullptr;
}

static optional<filesystem::path> FilepathFromTextureFilename(const string& filename,
																		const string& basePath) {
	
	string textureName = filename;
	if (textureName.substr(0, 1) == "/") {
		textureName = textureName.substr(1, textureName.length()-1);
	}
	else if (textureName.substr(0, 2) == "./") {
		textureName = textureName.substr(2, textureName.length()-2);
	}
	
	try {
		//path texturePath = canonical(path(textureName), path(basePath)); // boost
		path combined = path(basePath) / path(textureName);
		path texturePath = canonical(combined);
		return texturePath;
	}
	//catch (const filesystem::filesystem_error& e) {
	catch (const exception& e) {
		AE_LOG_E("Error expanding path: {}", e.what());
	}
	
	AE_LOG_W("Missing texture: {}", filename);
	
	return {};
}

static LIGHT_TYPE LightTypeForAILightType(aiLightSourceType aiType) {
	switch (aiType) {
		case aiLightSource_DIRECTIONAL: return LIGHT_TYPE::DIRECTIONAL;
		case aiLightSource_SPOT: return LIGHT_TYPE::SPOT;
		default: return LIGHT_TYPE::POINT;
	}
}

vec2 GLMVec2FromAIVector3D(const aiVector2D& from) {
	return vec2(from.x, from.y);
}

vec3 GLMVec3FromAIVector3D(const aiVector3D& from) {
	return vec3(from.x, from.y, from.z);
}

mat4 GLMMat4FromAIMaxtrix4x4(const aiMatrix4x4& from) {
	mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2;
	to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2;
	to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2;
	to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2;
	to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

Color ColorFromAIColor3D(const aiColor3D& from) {
	return Color(from.r, from.g, from.b, 1.0f);
}

Color ColorFromAIColor4D(const aiColor4D& from) {
	return Color(from.r, from.g, from.b, from.a);
}

#endif // !ANDROID
