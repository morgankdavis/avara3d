//
//  Utilities.cc
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Utilities.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>

#if defined(MACOS) || defined(LINUX)
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#endif

#ifdef MACOS
#include <CoreGraphics/CoreGraphics.h>
#include <mach-o/dyld.h>
#endif

#ifdef LINUX
#include <libgen.h>
#endif

#ifdef WINDOWS
#include <windows.h>
#endif

#ifdef ANDROID
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <NDKHelper.h>
#endif

#include <glm/gtc/quaternion.hpp>

#include "Camera.h"
#include "Color.h"
#include "CubeImage.h"
#include "Geometry.h"
#include "GeometryElement.h"
#include "Image.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "MaterialProperty.h"
#include "Node.h"
#include "RenderContext.h"
#include "Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
 Private Static Prototypes
 ***************************************************************************************/

static void StringFromTreeRec(Node& n, stringstream& ss, unsigned depth);

/***************************************************************************************
 	Output Utilities
 ***************************************************************************************/

ostream& ae::utils::operator<<(ostream& os, const glm::vec3& v) {
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

ostream& ae::utils::operator<<(ostream& os, const glm::vec4& v) {
	os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return os;
}

ostream& ae::utils::operator<<(ostream& os, const glm::quat& q) {
	os << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
	return os;
}

ostream& ae::utils::operator<<(ostream& os, const mat4& m) {
	// "GLM uses column major ordering, so the addressing is m[col][row]"
	// http://stackoverflow.com/questions/26454838/glm-multiplication-order
	
	char str[1024];
	snprintf(str, sizeof(str),
			 "%.2f\t%.2f\t%.2f\t%.2f\n%.2f\t%.2f\t%.2f\t%.2f\n%.2f\t%.2f\t%.2f\t%.2f\n%.2f\t%.2f\t%.2f\t%.2f",
			 m[0][0], m[1][0], m[2][0], m[3][0], // column major, OpenGL/GLM style
			 m[0][1], m[1][1], m[2][1], m[3][1],
			 m[0][2], m[1][2], m[2][2], m[3][2],
			 m[0][3], m[1][3], m[2][3], m[3][3]);
	
	return (os << str);
}

ostream& ae::utils::operator<<(ostream& os, const Color& c) {
	os << "(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
	return os;
}

string ae::utils::StringFromGLMVec3(const vec3& v) {
	ostringstream stringStream;
	stringStream << v;
	return stringStream.str();
}

string ae::utils::StringFromGLMVec4(const vec4& v) {
	ostringstream stringStream;
	stringStream << v;
	return stringStream.str();
}

string ae::utils::StringFromGLMQuat(const quat& q) {
	ostringstream stringStream;
	stringStream << q;
	return stringStream.str();
}

string ae::utils::StringFromGLMMat4(const mat4& m) {
	ostringstream stringStream;
	stringStream << m;
	return stringStream.str();
}

string ae::utils::StringFromColor(const Color& c) {
	ostringstream stringStream;
	stringStream << c;
	return stringStream.str();
}

string ae::utils::StringFromTree(Node& root) {

	std::stringstream ss;
	string name = (root.name() ? "\"" + *(root.name()) + "\"" : "null");
	ss << "[NODE] (" << static_cast<const void*>(&root) << ", " << name << ")" << endl;
	 
	unsigned depth = 0;

	for (auto& c : root.children(false)) {
		 StringFromTreeRec(*c, ss, depth+1);
	}
	
	return ss.str();
}

string ae::utils::DateTimeString() {
	char buffer[256];
#ifdef WINDOWS
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%Y.%m.%d_%I.%M.%S", timeinfo);
#else
	timeval curTime;
	gettimeofday(&curTime, NULL); // gettimeofday() is POSIX
	int milli = curTime.tv_usec / 1000;
	strftime(buffer, sizeof(buffer), "%Y.%m.%d_%H.%M.%S", localtime(&curTime.tv_sec));
	sprintf(buffer, "%s.%03d", buffer, milli);
#endif
	return string(buffer);
}

/***************************************************************************************
 	Numeric Utilities
 ***************************************************************************************/

int ae::utils::Uniform(int min, int max) {
	static random_device rd;
	static mt19937 gen(rd());
	uniform_int_distribution<> dis(min, max);
	return dis(gen);
}

float ae::utils::Uniform(float min, int max) {
	static random_device rd;
	static mt19937 gen(rd());
	uniform_real_distribution<> dis(min, max);
	return dis(gen);
}

bool ae::utils::Zero(const vec3& v, float tolerance) {
	return Equal(v.x, 0, tolerance) 
	&& Equal(v.y, 0, tolerance) 
	&& Equal(v.z, 0, tolerance);
}

float ae::utils::Max(const vec3& v) {
	return std::max(std::max(v.x, v.y), v.z);
}

bool ae::utils::Equal(float a, float b, float tolerance) {
	return (fabs(a - b) <= tolerance);
}

bool ae::utils::Equal(const glm::vec3& a, const glm::vec3& b, float tolerance) {
	return Equal(a.x, b.x, tolerance) 
	&& Equal(a.y, b.y, tolerance) 
	&& Equal(a.z, b.z, tolerance);
}

bool ae::utils::Equal(const glm::vec4& a, const glm::vec4& b, float tolerance) {
	return Equal(a.x, b.x, tolerance) 
	&& Equal(a.y, b.y, tolerance) 
	&& Equal(a.z, b.z, tolerance) 
	&& Equal(a.w, b.w, tolerance);
}

/**************************************************************************************
     String Utilities
 **************************************************************************************/

void ae::utils::StringReplace(string& str,
							  const string& oldStr,
							  const string& newStr) {
	string::size_type pos = 0u;
	while ((pos = str.find(oldStr, pos)) != string::npos) {
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}

/***************************************************************************************
 	File Utilities
 ***************************************************************************************/

// *** executable and working directories ***

#ifndef ANDROID

boost::optional<boost::filesystem::path> ae::utils::ExecutablePath() {
#if defined(MACOS)
	char path[1024];
	uint32_t size = sizeof(path);
	if (_NSGetExecutablePath(path, &size) == 0) {
		return boost::filesystem::path(path);
	}
#elif defined(LINUX)
	char path[1024];
	ssize_t count = readlink("/proc/self/exe", path, 1024);
	if (count != -1) {
		return boost::filesystem::path(path);
	}
#elif defined(WINDOWS)
	char path[1024];
	if (GetModuleFileName(NULL, path, 1024)) {
		return boost::filesystem::path(path);
	}
#endif
	return boost::none;
}

boost::optional<boost::filesystem::path> ae::utils::ExecutableDirectory() {
	auto execPathStr = ExecutablePath();
	if (execPathStr) {
		auto execPath = boost::filesystem::path(*execPathStr);
		return execPath.parent_path();
	}
	return boost::none;
}

boost::optional<boost::filesystem::path> ae::utils::ExecutableName() {
	auto execPathStr = ExecutablePath();
	if (execPathStr) {
		auto execPath = boost::filesystem::path(*execPathStr);
		if (is_regular_file(execPath)) {
			return execPath.filename();
		}
	}
	return boost::none;
}

boost::optional<boost::filesystem::path> ae::utils::CurrentWorkingDirectory() {
#if defined(MACOS) || defined(LINUX) || defined(ANDROID)
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd))) {
		return boost::filesystem::path(cwd);
	}
#else
	char path[1024];
	if (GetModuleFileName(NULL, path, 1024)) {
		return boost::filesystem::path(path);
	}
#endif
	return boost::none;
}

#endif // !ANDROID

// *** search paths ***

#ifndef ANDROID

vector<boost::filesystem::path> ae::utils::BaseSearchPaths() {
	// build a list of common directories where "shader", "scene", "images", "fonts" etc
	// subdirectories may live.
	// clients will use this to append those subdirectory names to search for specific resources.
	// clients should first check "local" locations first, then "engine" locations.
	
	auto basePaths = vector<boost::filesystem::path>();
	auto execDir = ExecutableDirectory();
	
	if (execDir) {
		// [local] archived
		// [local] cmake installed ("packaged")
		auto path = (*execDir) / "data";
		basePaths.emplace_back(path);
		
		// [local] xcode debug
		path = (*execDir).parent_path().parent_path().parent_path().parent_path() / "tests" / "data";
		basePaths.emplace_back(path);
		
		// [local] unix/msys debug
		path = (*execDir).parent_path().parent_path().parent_path() / "tests" / "data";
		basePaths.emplace_back(path);
		
		// [engine] archived
		path = (*execDir).parent_path() / "avara-engine";
		basePaths.emplace_back(path);
		
		// [engine] cmake installed ("packaged")
		// [engine] xcode debug
		path = (*execDir).parent_path().parent_path().parent_path().parent_path() / "avara-engine";
		basePaths.emplace_back(path);
		
		// [engine] unix/msys debug
		path = (*execDir).parent_path().parent_path().parent_path() / "avara-engine";
		basePaths.emplace_back(path);
		
		// fallback
		path = (*execDir);
		basePaths.emplace_back(path);
	}
	
	return basePaths;
}

vector<boost::filesystem::path> ae::utils::ShaderSearchPaths() {
	auto searchPaths = vector<boost::filesystem::path>();
	for (auto& path : BaseSearchPaths()) {
		searchPaths.emplace_back(path / "shaders");
	}
	return searchPaths;
}

vector<boost::filesystem::path> ae::utils::SceneSearchPaths() {
	auto searchPaths = vector<boost::filesystem::path>();
	for (auto& path : BaseSearchPaths()) {
		searchPaths.emplace_back(path / "scenes");
	}
	return searchPaths;
}

vector<boost::filesystem::path> ae::utils::ImageSearchPaths() {
	auto searchPaths = vector<boost::filesystem::path>();
	for (auto& path : BaseSearchPaths()) {
		searchPaths.emplace_back(path / "images");
	}
	return searchPaths;
}

vector<boost::filesystem::path> ae::utils::FontSearchPaths() {
	auto searchPaths = vector<boost::filesystem::path>();
	for (auto& path : BaseSearchPaths()) {
		searchPaths.emplace_back(path / "fonts");
	}
	return searchPaths;
}

boost::optional<boost::filesystem::path> ae::utils::SearchInPaths(const string& filename,
																  vector<boost::filesystem::path> paths) {
	AE_LOG->trace("Searching for '{}' in...", filename);
	for (auto& searchPath : paths) {
		AE_LOG->trace("\t...'{}", searchPath.string());
		if (boost::filesystem::is_directory(searchPath)) {
			auto path = searchPath / filename;
			if (boost::filesystem::is_regular_file(path)) {
				return path;
			}
		}
	}
	return boost::none;
}

#endif // !ANDROID

// *** binary and text files ***

vector<unsigned char> ae::utils::Buffer(unsigned char* buf, unsigned len) {
	auto ret = vector<unsigned char>();
	ret.resize(len);
	for (int i=0; i<len; ++i) {
		ret.push_back(buf[i]);
	}
	return ret;
}

unsigned ae::utils::Buffer(vector<unsigned char>& inBuf, unsigned char* outBuf) {
	// outBuf = pre-allocated
	unsigned index = 0;
	for (unsigned char b : inBuf) {
		outBuf[index] = b;
		++index;
	}
	return index;
}

#ifdef ANDROID

boost::optional<boost::filesystem::path> ae::utils::InternalFilesDirectory() {
	auto helper = ndk_helper::JNIHelper::GetInstance();
	string filesDir = helper->GetFilesDir();
	if (filesDir.length()) return boost::filesystem::path(filesDir);
	return boost::none;
}

boost::optional<string> ae::utils::TextAsset(const string& relPath) {
	vector<unsigned char> buffer = BinaryAsset(relPath);
	if (buffer.size()) {
		return string(buffer.begin(), buffer.end());
	}
	return boost::none;
}

vector<unsigned char> ae::utils::BinaryAsset(const string& relPath) {
	auto helper = ndk_helper::JNIHelper::GetInstance();
	auto buffer = vector<unsigned char>();
	helper->ReadFile(relPath.c_str(), &buffer);
	return buffer;
}

#else

boost::optional<string> ae::utils::TextFile(const boost::filesystem::path& path) {
	string line;
	string source = "";
	ifstream infile;
	infile.open(path.string());
	if (infile.is_open()) {
		while (!infile.eof()) {
			getline(infile, line);
			source += line;
			source += "\n";
		}
		infile.close();
		return source;
	}
	return boost::none;
}

vector<unsigned char> ae::utils::BinaryFile(const boost::filesystem::path& path) {
	ifstream inStream(path.string(), ios::binary | ios::ate); // ate == initial position at eof
    ifstream::pos_type pos = inStream.tellg();
    vector<unsigned char> result(pos);
    inStream.seekg(0, ios::beg);
    inStream.read((char*)&result[0], pos);
    return result;
}

unsigned ae::utils::BinaryFile(const boost::filesystem::path& path, std::vector<unsigned char> buffer) {
	ofstream outStrearm(path.string(), ios::out | ios::binary | ios::app); // app = all ops happed at oef
	unsigned written = 0;
	for (unsigned char byte : buffer) {
		outStrearm.write((char*)&byte, sizeof(unsigned char));
		++written;
	}
	return written;
}

#endif // ANDROID

// *** shaders ***

boost::optional<std::string> ae::utils::ShaderSource(const string& name,
													 const string& type) {
	boost::optional<string> rawSource = boost::none;
#ifdef ANDROID
	rawSource = TextAsset("shaders/" + name + "." + type);
#else
	auto path = SearchInPaths((name + "." + type), ShaderSearchPaths());
	if (path) {
		AE_LOG->trace("Found shader at path: {}", (*path).string());
		rawSource = TextFile(*path);
	}
#endif
	
	if (rawSource) {
		static const string HEADER_PLACEHOLDER = "<#HEADER#>";
#ifdef ANDROID
		static const string PLATFORM_HEADER = "#version 300 es\n\nprecision mediump int;\nprecision mediump float;";
#else
		static const string PLATFORM_HEADER = "#version 330";
#endif
		auto replaced = *rawSource;
		StringReplace(replaced, HEADER_PLACEHOLDER, PLATFORM_HEADER);
		return replaced;
	}
	
	return boost::none;
}

// *** fonts ***

vector<unsigned char> ae::utils::FontData(const string& name,
										  const string& type) {
#ifdef ANDROID
	return BinaryAsset("fonts/" + name + "." + type);
#else
	auto path = SearchInPaths((name + "." + type), FontSearchPaths());
	if (path) {
		AE_LOG->trace("Found font at path: {}", (*path).string());
		return BinaryFile(*path);
	}
	return vector<unsigned char>();
#endif
}

// ***  images ***

shared_ptr<Image> ae::utils::ImageNamed(const string& name,
										bool flipHorizontal) {

	return ImageNamed(name, "png", flipHorizontal);
}

shared_ptr<Image> ae::utils::ImageNamed(const string& name,
										const string& type,
										bool flipHorizontal) {
	
#ifdef ANDROID
	auto data = BinaryAsset("testdata/images/" + (name + "." + type));
	return make_shared<Image>(data);
#else
	auto path = SearchInPaths((name + "." + type), ImageSearchPaths());
	if (path) {
		AE_LOG->trace("Found image at path: {}", (*path).string());
		return make_shared<Image>(*path, flipHorizontal);
	}
#endif
	return nullptr;
}

shared_ptr<CubeImage> ae::utils::CubeImageNamed(const string& name) {
	
	return CubeImageNamed(name, "png");
}

shared_ptr<CubeImage> ae::utils::CubeImageNamed(const string& name,
												const string& type) {
	
	return make_shared<CubeImage>(ImageNamed(name + "_posx", type, false),
								  ImageNamed(name + "_negx", type, false),
								  ImageNamed(name + "_posy", type, false),
								  ImageNamed(name + "_negy", type, false),
								  ImageNamed(name + "_posz", type, false),
								  ImageNamed(name + "_negz", type, false));
}

// *** scenes ***

#ifndef ANDROID
shared_ptr<Scene> ae::utils::SceneNamed(const string& name) {
	
	return SceneNamed(name, "dae");
}

shared_ptr<Scene> ae::utils::SceneNamed(const string& name,
										const string& type) {
	
	auto path = SearchInPaths((name + "." + type), SceneSearchPaths());
	if (path) {
		AE_LOG->trace("Found scene at path: {}", (*path).string());
		return Scene::LoadFromFile(*path);
	}
	return nullptr;
}
#endif

/***************************************************************************************
 	Misc Utilities
 ***************************************************************************************/

void ae::utils::SaveSnapshot(RenderContext& context) {
#ifdef ANDROID
	throw Exception("SaveSnapshot() not supported on Android.");
#else
	auto image = context.snapshot();
	
	string dateTime = DateTimeString();
	
	char filename[256] = "";
	sprintf(filename, "Snapshot_%s.png", dateTime.c_str());
	
	AE_LOG->info("Saving snapshot '{}'...", filename);
	
	auto execDir = ExecutableDirectory();
	if (execDir) {
		auto fullPath = *execDir / filename;
		image->writePNG(fullPath);
	}
	else {
		AE_LOG->warn("Couldn't locate executable directory.");
	}
#endif
}

void ae::utils::StartGIFRecording(RenderContext& context,
								  unsigned maxHeight, unsigned maxFramerate) {
#ifdef ANDROID
	throw Exception("StartGIFRecording() not supported on Android.");
#else
	char filename[256] = "";
	sprintf(filename, "Recording_%s.gif", DateTimeString().c_str());
	auto execDir = ExecutableDirectory();
	if (execDir) {
		auto fullPath = *execDir / filename;
		context.startGIFRecording(fullPath.string(), maxHeight, maxFramerate);
	}
	else {
		AE_LOG->warn("Couldn't locate executable directory.");
	}
#endif
}

void ae::utils::StopGIFRecording(RenderContext& context) {
#ifdef ANDROID
	throw Exception("StopGIFRecording() not supported on Android.");
#else
	context.stopGIFRecording();
#endif
}

/***************************************************************************************
 Private Static
 ***************************************************************************************/

void StringFromTreeRec(Node& n, stringstream& ss, unsigned depth) {
	
	string padding = "";
	for (unsigned d=0; d<depth; ++d) {
		padding += "\t";
	}
	string nodeName = (n.name() ? "\"" + *(n.name()) + "\"" : "null");
	ss << padding << "[NODE] (" << static_cast<const void*>(&n)
	<< ", " << nodeName << ")" << endl;
	
	auto geometry = n.geometry();
	if (geometry) {
		string geometryName = (geometry->name() ? "\"" + *(geometry->name()) + "\"" : "null");
		ss << padding << "\t[GEOMETRY] (" << static_cast<const void*>(geometry.get())
		<< ", " << geometryName << ")" << endl;
		
		for (auto& element : geometry->elements()) {
			ss << padding << "\t\t[ELEMENT] (" << static_cast<const void*>(element.get())<< ")" << endl;
		}
		
		for (auto& material : geometry->materials()) {
			
			string properties = "";
			if (material->ambient()) properties += "a";
			if (material->diffuse()) properties += "d";
			if (material->specular()) properties += "s";
			if (material->emissive()) properties += "e";
			
			string materialName = (material->name() ? "\"" + *(material->name()) + "\"" : "null");
			ss << padding << "\t\t[MATERIAL] (" << static_cast<const void*>(material.get())
			<< ", " << materialName
			<< ", " << properties << ")" << endl;
			
//			auto ambient = material->ambient();
//			if (ambient) {
//				ss << padding << "\t\t\tambient (" << static_cast<const void*>(ambient.get()) << ")" << endl;
//			}
//			
//			auto diffuse = material->diffuse();
//			if (diffuse) {
//				ss << padding << "\t\t\tdiffuse (" << static_cast<const void*>(diffuse.get()) << ")" << endl;
//			}
//			
//			auto specular = material->specular();
//			if (specular) {
//				ss << padding << "\t\t\tspecular (" << static_cast<const void*>(specular.get()) << ")" << endl;
//			}
//			
//			auto emissive = material->emissive();
//			if (emissive) {
//				ss << padding << "\t\t\temissive (" << static_cast<const void*>(emissive.get()) << ")" << endl;
//			}
		}
	}
	
	auto light = n.light();
	if (light) {
		string lightName = (light->name() ? "\"" + *(light->name()) + "\"" : "null");
		ss << padding << "\t[LIGHT] (" << static_cast<const void*>(light.get())
		<< ", " << lightName << ")" << endl;
	}
	
	auto camera = n.camera();
	if (camera) {
		string cameraName = (camera->name() ? "\"" + *(camera->name()) + "\"" : "null");
		ss << padding << "\t[CAMERA] (" << static_cast<const void*>(camera.get())
		<< ", " << cameraName << ")" << endl;
	}

	for (auto& c : n.children(false)) {
		StringFromTreeRec(*c, ss, depth+1);
	}
}
