//
//  Scene.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "ae/scene/Scene.h"

#include <chrono>
#include <filesystem>
//#include <optional>
#include <thread>

#include "fmt/format.h"
//#include "magic_enum.hpp"
#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_NOEXCEPTION
//#define JSON_NOEXCEPTION
#include "tiny_gltf.h"

#include "ae/Image.h"
#include "ae/Utilities.h"
#include "ae/Color.h"
#include "ae/CubeImage.h"
#include "ae/diagnostic/exceptions/UnsupportedFormat.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/geometry/Geometry.h"
#include "ae/geometry/GeometryElement.h"
#include "ae/input/platform/desktop/WindowInputManager.h"
#include "ae/physics/PhysicsBody.h"
#include "ae/physics/PhysicalWorld.h"
#include "ae/rendering/Light.h"
#include "ae/rendering/Renderer.h"
#include "ae/rendering/VisualWorld.h"
#include "ae/rendering/context/RenderContext.h"
#include "ae/rendering/camera/PerspectiveCamera.h"
#include "ae/rendering/materials/Material.h"
#include "ae/rendering/materials/MaterialProperty.h"
#include "ae/scene/Node.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;
using namespace std::filesystem;


constexpr float FRAMETIME_AVERAGING_INTERVAL = .25;


/*********************************************************************************************
	Private Static Prototypes
 *********************************************************************************************/

static void 						LoadGlTF(Scene& scene, const filesystem::path& path);
static void 						GetRunTime(double time, // time since reference
											  bool paused,
											  double& runT, // time since reference excluding paused time
											  double& deltaRunT); // time since last call excluding paused time
static void 						UpdateUserTimeStats(Stats& stats, double startTime, double endTime);
static void							UpdateFrameTimeStats(Stats& stats, double time);
static shared_ptr<Image> 			MissingTextureImage();
//#ifndef ANDROID
//static void 						AddAIGeometryNodes(Scene& scene,
//													  const aiScene* aiScene,
//													  shared_ptr<Node> aeRootNode,
//													  const vector<shared_ptr<GeometryElement>>& importElements,
//													  const vector<shared_ptr<Material>>& importMaterials);
//static void 						AddAIGeometryNodeRec(Scene& scene,
//														const aiScene* aiScene,
//														const aiNode* aiGeometryNode,
//														shared_ptr<Node> aeParentNode,
//														const vector<shared_ptr<GeometryElement>>& importElements,
//														const vector<shared_ptr<Material>>& importMaterials);
//static shared_ptr<MaterialProperty> MaterialPropertyFromAIMaterial(const aiMaterial* aiMaterial,
//																   aiTextureType type,
//																   string basePath);
//static optional<path> 				FilepathFromTextureFilename(const string& filename,
//																  const string& basePath);
//static LIGHT_TYPE 					LightTypeForAILightType(aiLightSourceType aiType);
//
//static vec2 						GLMVec2FromAIVector3D(const aiVector2D& from);
//static vec3 						GLMVec3FromAIVector3D(const aiVector3D& from);
//static mat4 						GLMMat4FromAIMaxtrix4x4(const aiMatrix4x4& from);
//static Color 						ColorFromAIColor3D(const aiColor3D& from);
//static Color 						ColorFromAIColor4D(const aiColor4D& from);

//#endif // !ANDROID

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<Scene> Scene::FromFile(const filesystem::path& path) {

	auto extension = path.extension();
	if (extension == ".gltf" || extension == ".glb") {
		auto scene = make_shared<Scene>();
		LoadGlTF(*scene, path);
		return scene;
	}
	else {
		throw UnsupportedFormat(fmt::format("Unsupported scene format: {}", extension.string()));
	}

	return nullptr;
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Scene::Scene():
		_rootNode(make_shared<Node>("root node")),
		_visualWorld(nullptr),
		_physicalWorld(nullptr),
		_inputManager(nullptr),
		_debugOptions(DEBUG_OPTIONS::NONE),
		_stats({}),
		_running(false),
		_paused(false),
		_update(nullptr) {

	_rootNode->attachedToScene(this);
}

Scene::Scene(shared_ptr<VisualWorld> visualWorld,
			 shared_ptr<PhysicalWorld> physicsWorld,
			 shared_ptr<InputManager> inputManager):
		Scene() {

	_visualWorld = visualWorld;
	_physicalWorld = physicsWorld;
	_inputManager = inputManager;

	if (_visualWorld) _visualWorld->attachedToScene(this);
	if (_physicalWorld) _physicalWorld->attachedToScene(this);
	if (_inputManager) _inputManager->attachedToScene(this);
}

//Scene::Scene(shared_ptr<VisualWorld> visualWorld,
//			 shared_ptr<PhysicalWorld> physicsWorld,
//			 shared_ptr<InputManager> inputManager):
//		_rootNode(make_shared<Node>("root node")),
//		_visualWorld(visualWorld),
//		_physicalWorld(physicsWorld),
//		_inputManager(inputManager),
//		_debugOptions(DEBUG_OPTIONS::NONE),
//		_stats({}),
//		_running(false),
//		_paused(false),
//		_update(nullptr) {
//
//	_rootNode->attachedToScene(this);
//	if (_visualWorld) _visualWorld->attachedToScene(this);
//	if (_physicalWorld) _physicalWorld->attachedToScene(this);
//	if (_inputManager) _inputManager->attachedToScene(this);
//}

Scene::~Scene() {
	AE_LOG_D("Destroying Scene {:p}", static_cast<void*>(this));

	if (_rootNode) _rootNode->detachedFromScene(this);
	if (_visualWorld) _visualWorld->detachedFromScene(this);
	if (_physicalWorld) _physicalWorld->detachedFromScene(this);
	if (_inputManager) _inputManager->detachedFromScene(this);
//	rootNode(nullptr);
//	visualWorld(nullptr);
//	physicalWorld(nullptr);
//	inputManager(nullptr);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<Node> Scene::rootNode() const {
	return _rootNode;
}

void Scene::rootNode(shared_ptr<Node> node) {

//	if () // check they are not the same
	if (_rootNode) {
		_rootNode->detachedFromScene(this);
	}

	_rootNode = node;

	if (_rootNode) {
		_rootNode->attachedToScene(this);
	}
}

shared_ptr<VisualWorld> Scene::visualWorld() const {
	return _visualWorld;
}

void Scene::visualWorld(shared_ptr<VisualWorld> world) {

//	if (world != _visualWorld) {

		if (_visualWorld) {

			_visualWorld->detachedFromScene(this);

			if (_rootNode) {
				_rootNode->visualWorldDetachedFromScene(_visualWorld.get(), this);
			}
		}

		_visualWorld = world;

		if (_visualWorld) {

			_visualWorld->attachedToScene(this);

			if (_rootNode) {
				_rootNode->visualWorldAttachedToScene(_visualWorld.get(), this);
			}
		}
//	}
}

shared_ptr<PhysicalWorld> Scene::physicalWorld() const {
	return _physicalWorld;
}

void Scene::physicalWorld(shared_ptr<PhysicalWorld> world) {

	if (_physicalWorld) {

		_physicalWorld->detachedFromScene(this);

		if (_rootNode) {
			_rootNode->physicalWorldDetachedFromScene(_physicalWorld.get(), this);
		}
	}

	_physicalWorld = world;

	if (world) {

		world->attachedToScene(this);

		if (_rootNode) {
			_rootNode->physicalWorldAttachedToScene(world.get(), this);
		}
	}
}

shared_ptr<InputManager> Scene::inputManager() const {
	return _inputManager;
}

void Scene::inputManager(shared_ptr<InputManager> inputManager) {

	if (_inputManager) {
		_inputManager->detachedFromScene(this);
	}

	_inputManager = inputManager;

	if (inputManager) {
		inputManager->attachedToScene(this);
	}
}

double Scene::time() const {
	static auto startTime = chrono::high_resolution_clock::now();
	auto nowTime = chrono::high_resolution_clock::now();
	return (chrono::duration<double>(nowTime - startTime)).count();
}

DEBUG_OPTIONS Scene::debugOptions() const {
	return _debugOptions;
}

void Scene::debugOptions(DEBUG_OPTIONS options) {

#ifdef ANDROID
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		throw Exception("DEBUG_OPTIONS::SHOW_WIREFRAMES not supported on this platform.");
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
		throw Exception("DEBUG_OPTIONS::SHOW_BOUNDING_BOXES not supported on this platform.");
	}
#endif

	_debugOptions = options;
}

void Scene::run() {

	if (_rootNode) {

		_running = true;

		if (_visualWorld) {
			_visualWorld->checkAddDefaultLighting();
		}

		double deltaT, runT, deltaRunT;

		do {

			GetRunTime(time(),
					   _paused,
					   runT,
					   deltaRunT);

			memset(&_stats, 0, sizeof(Stats));
			UpdateFrameTimeStats(_stats, runT);

			if (_inputManager) {
				_inputManager->update();
			}

			if (_update) {

				auto updateStartTime = time();
				(_update)(*this, runT);
				UpdateUserTimeStats(_stats, updateStartTime, time());
			}

			if (!_paused) {

				if (_physicalWorld) {

					_physicalWorld->step(*this,
										 runT,
										 deltaRunT,
										 _stats);
				}

				if (_visualWorld) {

					_visualWorld->draw(*this,
									   (_physicalWorld ? _physicalWorld.get() : nullptr),
									   runT,
									   deltaRunT,
									   _debugOptions,
									   _stats);
				}
			}
			else {
				this_thread::sleep_for(chrono::microseconds(16667));
			}

		} while (_running);
	}
	else {
		AE_LOG_E("No root node attached to Scene {:p}", static_cast<void*>(this));
	}
}

void Scene::stop() {

	if (_running) {
		_running = false;
	}
	else {
		AE_LOG_W("Attempting to stop when Scene not running.");
	}
}

bool Scene::running() const {
	return _running;
}

bool Scene::paused() const {
	return _paused;
}

void Scene::paused(bool flag) {
	_paused = flag;
}

const Stats& Scene::stats() const {
	return _stats;
}

Scene::UpdateCallback Scene::update() const {
	return _update;
}

void Scene::update(UpdateCallback function) {
	_update = function;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

//void Scene::updateTimeStats(Stats& stats, float time) {
//
//	static float fpsAvg = 0.0;
//	static float msAvg = 0.0;
//
//	static int elapsedFramesThisSample = 0;
//
//	static float lastSampleStartTime = time;
//
//	float elapsedSecondsSinceLastFrame = time - lastSampleStartTime;
//
//	float timeSinceBeginSample = time - lastSampleStartTime;
//	if (timeSinceBeginSample >= FRAMETIME_AVERAGING_INTERVAL) {
//
//		fpsAvg = (float)elapsedFramesThisSample / timeSinceBeginSample;
//		msAvg = (elapsedSecondsSinceLastFrame * 1000.0f) / elapsedFramesThisSample;
//
//		elapsedFramesThisSample = 0;
//		lastSampleStartTime = time;
//	}
//	else {
//		++elapsedFramesThisSample;
//	}
//
//	stats.averageFramerate = fpsAvg;
//	stats.averageFrametime = msAvg;
//
//	stats.currentFramerate = 60.0f / elapsedSecondsSinceLastFrame;
//	stats.currentFrametime = elapsedSecondsSinceLastFrame * 1000.0f; // is this wrong?
//
//	stats.averagingInterval = FRAMETIME_AVERAGING_INTERVAL;
//}

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

//static void LoadObj(Scene& scene, const filesystem::path& path) {
//
////	std::string inputfile = "cornell_box.obj";
//	tinyobj::ObjReaderConfig reader_config;
//	reader_config.mtl_search_path = "./"; // Path to material files
//
//	tinyobj::ObjReader reader;
//
//	if (!reader.ParseFromFile(path, reader_config)) {
//		if (!reader.Error().empty()) {
//			std::cerr << "TinyObjReader: " << reader.Error();
//		}
//		exit(1);
//	}
//
//	if (!reader.Warning().empty()) {
//		std::cout << "TinyObjReader: " << reader.Warning();
//	}
//
//	auto& attrib = reader.GetAttrib();
//	auto& shapes = reader.GetShapes();
//	auto& materials = reader.GetMaterials();
//
//// Loop over shapes
//	for (size_t s = 0; s < shapes.size(); s++) {
//		// Loop over faces(polygon)
//		size_t index_offset = 0;
//		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
//			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
//
//			// Loop over vertices in the face.
//			for (size_t v = 0; v < fv; v++) {
//				// access to vertex
//				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
//				tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
//				tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
//				tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
//
//				// Check if `normal_index` is zero or positive. negative = no normal data
//				if (idx.normal_index >= 0) {
//					tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
//					tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
//					tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
//				}
//
//				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
//				if (idx.texcoord_index >= 0) {
//					tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
//					tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
//				}
//
//				// Optional: vertex colors
//				// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
//				// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
//				// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
//			}
//			index_offset += fv;
//
//			// per-face material
//			shapes[s].mesh.material_ids[f];
//		}
//	}
//}

void LoadGlTF(Scene& scene, const filesystem::path& path) {

	using namespace tinygltf;

	TinyGLTF loader;
	Model model;
	string err;
	string warn;

	auto extension = path.extension();
	bool res = false;
	if (extension == ".gltf") {
		res = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());
	}
	else if (extension == ".glb") {
		res = loader.LoadBinaryFromFile(&model, &err, &warn, path.string());
	}
	else {
		AE_LOG_E("Unsupported file extension: {}", extension.string());
	}

	if (res) {

		if (!warn.empty()) {
			cout << "WARN: " << warn << endl;
		}

		cout << "Loaded glTF: " << path << endl;
	}
	else {
		cout << "Failed to load glTF: " << path << endl;

		if (!err.empty()) {
			cout << "ERR: " << err << endl;
		}
	}
}

//#ifndef ANDROID
//static void LoadFile(Scene& scene, const filesystem::path& importPath) {
//
//	AE_LOG_I("Assimp version: {}.{}.{}",
//			 aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
//
//	AE_LOG_I("Loading scene: {}", importPath.string());
//
//	unsigned int assimpFlags = aiProcess_Triangulate
//							   | aiProcess_SortByPType
//							   | aiProcess_GenSmoothNormals
//							   // "This will, in fact, reduce the number of update calls."
//							   // http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410af5fe0d6ee720c91359dc61cb849f2ebf
//							   | aiProcess_OptimizeMeshes
//							   // "If this flag is not specified, no vertices are referenced by more than one face and no index buffer is required for rendering."
//							   // http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410a444a6c9d8b63e6dc9e1e2e1edd3cbcd4
//							   | aiProcess_JoinIdenticalVertices
//							   | aiProcess_ImproveCacheLocality
//							   | aiProcess_ValidateDataStructure;
//
//	const aiScene* aiScene = aiImportFile(importPath.string().c_str(), assimpFlags);
//
//	if (aiScene) {
//
//		// copy all the meshes and materials out of the aiScene
//		// use them to construct our GeometryElements
//		// (we are not keeping a master list)
//		auto importElements = vector<shared_ptr<GeometryElement>>();
//		auto importMaterials = vector<shared_ptr<Material>>();
//
//		// ********** meshes (GeometryElement) **********
//
//		int numMeshes = aiScene->mNumMeshes;
//		for (int m=0; m<numMeshes; ++m) {
//			AE_LOG_D("Processing mesh {}...:", m);
//
//			aiMesh *mesh = aiScene->mMeshes[m];
//
//			// should be set for parent Geometry
//			aiString name = mesh->mName;
//			if (strcmp(name.C_Str(), "") != 0) {
//				AE_LOG_D("Mesh name: {}", name.C_Str());
//			}
//
//			auto verts = vector<Vertex>();
//
//			bool hasNormals = mesh->HasNormals();
//			bool hasTextureCoordinates = mesh->HasTextureCoords(0);
//
//			unsigned int numVerts = mesh->mNumVertices;
//			for (unsigned int v=0; v<numVerts; ++v) {
//				aiVector3D position = mesh->mVertices[v];
//				aiVector3D normal = aiVector3D(0, 0, 0);
//				aiVector3D texCoord = aiVector3D(0, 0, 0);
//
//				if (hasNormals) normal = mesh->mNormals[v];
//				if (hasTextureCoordinates) texCoord = mesh->mTextureCoords[0][v];
//
//				Vertex vert = {GLMVec3FromAIVector3D(position),
//							   GLMVec3FromAIVector3D(normal),
//							   vec2(texCoord.x, texCoord.y)};
//				verts.push_back(vert);
//			}
//
//			auto faces = vector<Face>();
//			unsigned int numFaces = mesh->mNumFaces;
//			for (unsigned int f=0; f<numFaces; ++f) {
//				aiFace face = mesh->mFaces[f];
//				faces.push_back({static_cast<int>(face.mIndices[0]),
//								 static_cast<int>(face.mIndices[1]),
//								 static_cast<int>(face.mIndices[2])});
//			}
//
//			auto element = make_shared<GeometryElement>(verts, faces);
//			//geometryElements().push_back(element);
//			importElements.push_back(element);
//		}
//
//
//		// ********** materials **********
//
//		AE_LOG_D("Number of materials: {}", aiScene->mNumMaterials);
//
//		for (unsigned int m=0; m < aiScene->mNumMaterials; ++m) {
//
//			AE_LOG_D("Processing material {}...:", m);
//
//			aiMaterial* aiMaterial = aiScene->mMaterials[m];
//
//			string basePath = path(importPath).parent_path().string();
//
//			auto ambientProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_AMBIENT, basePath);
//			auto diffuseProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_DIFFUSE, basePath);
//			auto specularProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_SPECULAR, basePath);
//			// not sure why, but some models have emissive colors that are messing everything up...
//			//auto emissiveProperty = MaterialPropertyFromAIMaterial(aiMaterial, aiTextureType_EMISSIVE, basePath);
//
//			auto material = make_shared<Material>(ambientProperty, diffuseProperty, specularProperty);
//			//material->emissive(emissiveProperty);
//
//			aiString name;
//			if (aiMaterial->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
//				if (strcmp(name.C_Str(), "") != 0) {
//					AE_LOG_D("Name: {}", name.C_Str());
//					material->name(name.C_Str());
//					if (*(material->name()) == AI_DEFAULT_MATERIAL_NAME) {
//						// https://sourceforge.net/p/assimp/discussion/817654/thread/0729fb73/
//						// it appears that OBJ add a "default material". donno why. it doesn't get used
//						// and thus the created MaterialProperties and Material will be deallocated after import
//						AE_LOG_I("AI_DEFAULT_MATERIAL_NAME");
//					}
//				}
//			}
//
//			// specular exponent
//			// https://www.mathworks.com/matlabcentral/mlc-downloads/downloads/
//			// submissions/27982/versions/5/previews/help%20file%20format/MTL_format.html
//			float shininess = 0;
//			if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
//				AE_LOG_D("Specular exponent: {}", shininess);
//			}
//			material->specularExponent(shininess);
//
//			AE_LOG_D("ADDING MATERIAL: {:p}", static_cast<void*>(material.get()));
//			importMaterials.push_back(material);
//		}
//
//		AE_LOG_D("DONE WITH MATERIALS. COUNT: {}", importMaterials.size());
//
//		// ********** nodes (Geometry) **********
//
//		// in AI terminology, a "node" is what we call a "geometry"
//		// also in AI terminology, a "mesh" is what we call a "geometry element"
//
//		auto filename = importPath.filename().string();
//		scene.rootNode()->name(filename + " ROOT");
//
//		AddAIGeometryNodes(scene, aiScene, scene.rootNode(), importElements, importMaterials);
//
//		// ********** lights **********
//
//		for (unsigned int l=0; l<aiScene->mNumLights; --l) {
//			break; // disabling for now...
//
//			aiLight* aiLight = aiScene->mLights[l];
//
//			Color color = ColorFromAIColor3D(aiLight->mColorDiffuse);
//
//			auto light = make_shared<Light>(LightTypeForAILightType(aiLight->mType),
//											make_shared<Color>(color));
//
//			aiString name = aiLight->mName;
//			if (strcmp(name.C_Str(), "") != 0) {
//				light->name(name.C_Str());
//			}
//
//			//cout << "Adding light: " << light << endl;
//
//			auto lightNode = make_shared<Node>("Light");
//			lightNode->light(light);
//			//rootNode()->addChild(lightNode);
//			scene.rootNode()->addChild(lightNode);
//		}
//
//		// ********** cameras **********
//
//		for (unsigned int c=0; c<aiScene->mNumCameras; --c) {
//			break; // disabling for now...
//
//			aiCamera* aiCamera = aiScene->mCameras[c];
//
//			auto camera = make_shared<PerspectiveCamera>(aiCamera->mName.C_Str(),
//														 aiCamera->mClipPlaneNear,
//														 aiCamera->mClipPlaneFar,
//														 aiCamera->mHorizontalFOV);
//
//			aiString name = aiCamera->mName;
//			if (strcmp(name.C_Str(), "") != 0) {
//				camera->name(name.C_Str());
//			}
//
//			//cout << "Adding camera: " << camera << endl;
//
//			auto cameraNode = make_shared<Node>("Camera");
//			cameraNode->camera(camera);
//
//			aiNode* aiCamNode = aiScene->mRootNode->FindNode(aiCamera->mName);
//
//			auto viewMat = GLMMat4FromAIMaxtrix4x4(aiCamNode->mTransformation);
//
//			cameraNode->transform(viewMat);
//
//			scene.rootNode()->addChild(cameraNode);
//		}
//	}
//	else {
//		//AE_LOG_E("Error importing scene: {}", aiGetErrorString());
//
//		char errMsg[1024];
//		sprintf(errMsg, "Error importing scene: %s\n",  aiGetErrorString());
//		throw Exception(errMsg);
//	}
//
//	aiReleaseImport(aiScene);
//}

/*********************************************************************************************
	Private Static
 *********************************************************************************************/

static void GetRunTime(double time, // time since reference
					   bool paused,
					   double& runT, // time since reference excluding paused time
					   double& deltaRunT) {//, // time since last call excluding paused time

	const double t = time;
	static double prevT = t;
	double deltaT = t - prevT;
	prevT = t;

	static double pauseTime = 0;
	runT = t - pauseTime;

	static double prevRunT = runT;
	deltaRunT = runT - prevRunT;
	prevRunT = runT;

	if (paused) pauseTime += deltaT;
}

void UpdateUserTimeStats(Stats& stats, double startTime, double endTime) {

	// current
	auto updateTime = endTime - startTime;
	stats.currentUsertime = updateTime * 1000.0f;

	static const double FRAMETIME_AVERAGING_INTERVAL = .25; // TEMPORARY

	// average
	static double avg = 0.0;
	static double sampleStartTime = startTime;
	static unsigned updatesSinceSampleStart = 0;
	static double accumulatedUpdateTimeSinceSampleStart = 0;
	double elapsedTimeSinceSampleStart = endTime - sampleStartTime;
	if (elapsedTimeSinceSampleStart >= FRAMETIME_AVERAGING_INTERVAL) {

		avg = (accumulatedUpdateTimeSinceSampleStart * 1000.0f) / updatesSinceSampleStart;

		sampleStartTime = startTime;
		updatesSinceSampleStart = 0;
		accumulatedUpdateTimeSinceSampleStart = 0;
	}
	else {
		++updatesSinceSampleStart;
		accumulatedUpdateTimeSinceSampleStart += updateTime;
	}

	stats.averageUsertime = avg;
//	stats.averagingInterval = FRAMETIME_AVERAGING_INTERVAL;
}

void UpdateFrameTimeStats(Stats& stats, double time) {

	// current
	static double previousTime = time;
	double deltaTime = time - previousTime;
	previousTime = time;
	stats.currentFramerate = 60.0f / deltaTime;
	stats.currentFrametime = deltaTime * 1000.0f;

	// *** every 5 seconds something slows a frame down significantly ***
//	if (stats.currentFrametime > 15) {
//		AE_LOG_W("currentFrametime: {}", stats.currentFrametime);
//	}

	// average
	static double fpsAvg = 0.0;
	static double msAvg = 0.0;
	static unsigned framesSinceSampleStart = 0;
	static double sampleStartTime = time;
	double elapsedTimeSinceSampleStart = time - sampleStartTime;
	if (elapsedTimeSinceSampleStart >= FRAMETIME_AVERAGING_INTERVAL) {

		fpsAvg = (double)framesSinceSampleStart / elapsedTimeSinceSampleStart;
		msAvg = (elapsedTimeSinceSampleStart * 1000.0f) / framesSinceSampleStart;

		sampleStartTime = time;
		framesSinceSampleStart = 0;
	}
	else {
		++framesSinceSampleStart;
	}
	stats.averageFramerate = fpsAvg;
	stats.averageFrametime = msAvg;
	stats.averagingInterval = FRAMETIME_AVERAGING_INTERVAL;
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

//static void AddAIGeometryNodes(Scene& scene,
//							   const aiScene* aiScene,
//							   shared_ptr<Node> aeRootNode,
//							   const vector<shared_ptr<GeometryElement>>& importElements,
//							   const vector<shared_ptr<Material>>& importMaterials) {
//
//	aiNode* aiRootGeometryNode = aiScene->mRootNode;
//	unsigned int nChildren = aiRootGeometryNode->mNumChildren;
//	for (unsigned int i=0; i<nChildren; ++i) {
//		aiNode* child = (aiRootGeometryNode->mChildren)[i];
//		AddAIGeometryNodeRec(scene, aiScene, child, aeRootNode, importElements, importMaterials);
//	}
//}
//
//static void AddAIGeometryNodeRec(Scene& scene,
//								 const aiScene* aiScene,
//								 const aiNode* aiGeometryNode,
//								 shared_ptr<Node> aeParentNode,
//								 const vector<shared_ptr<GeometryElement>>& importElements,
//								 const vector<shared_ptr<Material>>& importMaterials) {
//
//	string name = aiGeometryNode->mName.C_Str();
//
//	mat4 transform = GLMMat4FromAIMaxtrix4x4(aiGeometryNode->mTransformation);
//
//	AE_LOG_D("Adding '{}' with transform:\n{}", name, StringFromGLMMat4(transform));
//
//	auto elements = vector<shared_ptr<GeometryElement>>();
//	auto materials = vector<shared_ptr<Material>>();
//	int numMeshes = aiGeometryNode->mNumMeshes;
//
//	AE_LOG_D("Number of meshes: {}", numMeshes);
//
//	for (int m=0; m<numMeshes; ++m) {
//		AE_LOG_D("Reading mesh {}...", m);
//
//		unsigned int meshIndex = aiGeometryNode->mMeshes[m];
//		auto element = importElements[meshIndex];
//		elements.push_back(element);
//
//		unsigned int materialIndex = aiScene->mMeshes[meshIndex]->mMaterialIndex;
//		if (importMaterials.size() && (importMaterials.size()-1 >= materialIndex)) {
//			auto material = importMaterials[materialIndex];
//			AE_LOG_D("Adding material at index: {}", materialIndex);
//			materials.push_back(material);
//		}
//	}
//
//	// *** some nodes only have cameras and lights, which we are throwing out.
//	// so don't add a node with nothing in it. ***
//
//	shared_ptr<Node> newNode = nullptr;
//
//	if (numMeshes > 0) {
//		AE_LOG_D("Adding node WITH geometry...");
//		auto geometry = make_shared<Geometry>(elements, materials);
//		geometry->name(name);
//
//		//newNode = make_shared<Node>(name, transform, geometry);
//		newNode = make_shared<Node>(name);
//		newNode->transform(transform);
//		newNode->geometry(geometry);
//		aeParentNode->addChild(newNode);
//	}
//	else {
//		AE_LOG_D("Adding node WITHOUT geometry...");
//		//newNode = make_shared<Node>(name, transform);
//		newNode = make_shared<Node>(name);
//		newNode->transform(transform);
//		aeParentNode->addChild(newNode);
//	}
//
//	unsigned int nChildren = aiGeometryNode->mNumChildren;
//	for (unsigned int i = 0; i < nChildren; ++i) {
//		aiNode *child = (aiGeometryNode->mChildren)[i];
//		AddAIGeometryNodeRec(scene, aiScene, child, newNode, importElements, importMaterials);
//	}
//}
//
//static shared_ptr<MaterialProperty> MaterialPropertyFromAIMaterial(const aiMaterial* aiMaterial,
//																   aiTextureType type,
//																   string basePath) {
//	// TODO: Clean up
//	// this is hacky...
//	//#define AI_MATKEY_COLOR_DIFFUSE "$clr.diffuse",0,0
//	//#define AI_MATKEY_COLOR_AMBIENT "$clr.ambient",0,0
//	//#define AI_MATKEY_COLOR_SPECULAR "$clr.specular",0,0
//	//#define AI_MATKEY_COLOR_EMISSIVE "$clr.emissive",0,0
//
//	char colorType[1024];
//	string typeStr = "";
//	switch (type) {
//		case aiTextureType_AMBIENT:
//			strcpy(colorType, "$clr.ambient");
//			typeStr = "ambient";
//			break;
//		case aiTextureType_DIFFUSE:
//			strcpy(colorType, "$clr.diffuse");
//			typeStr = "diffuse";
//			break;
//		case aiTextureType_SPECULAR:
//			strcpy(colorType, "$clr.specular");
//			typeStr = "specular";
//			break;
//		case aiTextureType_EMISSIVE:
//			strcpy(colorType, "$clr.emissive");
//			typeStr = "emissive";
//			break;
//		default:
//			AE_LOG_W("Unsupported material type: {}", magic_enum::enum_name(type));
//			return nullptr;
//	}
//
//	AE_LOG_I("Reading {} material...", typeStr);
//
//	if (aiMaterial->GetTextureCount(type)) { // texture
//
//		aiString filename;
//		if (aiMaterial->GetTexture(type, 0, &filename,
//								   NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
//			auto texturePath = FilepathFromTextureFilename(filename.C_Str(), basePath);
//			if (texturePath) {
//				AE_LOG_D("Texture path: {}", texturePath->string());
//				auto textureImage = make_shared<Image>(*texturePath);
//				return make_shared<MaterialProperty>(textureImage);
//			}
//			else {
//				return make_shared<MaterialProperty>(MissingTextureImage());
//			}
//		}
//	}
//	else { // color
//
//		aiColor4D aiColor;
//		if (aiMaterial->Get(colorType, 0, 0, aiColor) == AI_SUCCESS) {
//			auto aeColor = make_shared<Color>(ColorFromAIColor4D(aiColor));
//			AE_LOG_D("Color: {}", StringFromColor(*aeColor));
//			return make_shared<MaterialProperty>(aeColor);
//		}
//		else {
//			AE_LOG_D("No {} material...", typeStr);
//		}
//	}
//
//	return nullptr;
//}
//
//static optional<filesystem::path> FilepathFromTextureFilename(const string& filename,
//																		const string& basePath) {
//
//	string textureName = filename;
//	if (textureName.substr(0, 1) == "/") {
//		textureName = textureName.substr(1, textureName.length()-1);
//	}
//	else if (textureName.substr(0, 2) == "./") {
//		textureName = textureName.substr(2, textureName.length()-2);
//	}
//
//	try {
//		//path texturePath = canonical(path(textureName), path(basePath)); // boost
//		path combined = path(basePath) / path(textureName);
//		path texturePath = canonical(combined);
//		return texturePath;
//	}
//	//catch (const filesystem::filesystem_error& e) {
//	catch (const exception& e) {
//		AE_LOG_E("Error expanding path: {}", e.what());
//	}
//
//	AE_LOG_W("Missing texture: {}", filename);
//
//	return {};
//}
//
//static LIGHT_TYPE LightTypeForAILightType(aiLightSourceType aiType) {
//	switch (aiType) {
//		case aiLightSource_DIRECTIONAL: return LIGHT_TYPE::DIRECTIONAL;
//		case aiLightSource_SPOT: return LIGHT_TYPE::SPOT;
//		default: return LIGHT_TYPE::POINT;
//	}
//}
//
//vec2 GLMVec2FromAIVector3D(const aiVector2D& from) {
//	return vec2(from.x, from.y);
//}
//
//vec3 GLMVec3FromAIVector3D(const aiVector3D& from) {
//	return vec3(from.x, from.y, from.z);
//}
//
//mat4 GLMMat4FromAIMaxtrix4x4(const aiMatrix4x4& from) {
//	mat4 to;
//	to[0][0] = from.a1; to[1][0] = from.a2;
//	to[2][0] = from.a3; to[3][0] = from.a4;
//	to[0][1] = from.b1; to[1][1] = from.b2;
//	to[2][1] = from.b3; to[3][1] = from.b4;
//	to[0][2] = from.c1; to[1][2] = from.c2;
//	to[2][2] = from.c3; to[3][2] = from.c4;
//	to[0][3] = from.d1; to[1][3] = from.d2;
//	to[2][3] = from.d3; to[3][3] = from.d4;
//	return to;
//}
//
//Color ColorFromAIColor3D(const aiColor3D& from) {
//	return Color(from.r, from.g, from.b, 1.0f);
//}
//
//Color ColorFromAIColor4D(const aiColor4D& from) {
//	return Color(from.r, from.g, from.b, from.a);
//}
//
//#endif // !ANDROID
