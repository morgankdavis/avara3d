//
//  Utilities.cc
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/Utilities.h"

//#include <algorithm> // needs to be under windows.h
#include <chrono>
#include <cstring>
#include <ctime>
#include <format>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>

#ifdef POSIX
#include <errno.h>
#include <execinfo.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#ifdef LINUX
#include <libgen.h>
#include <linux/limits.h> // PATH_MAX
#endif

#ifdef MACOS
#include <CoreGraphics/CoreGraphics.h>
#include <mach-o/dyld.h>
#include <sys/syslimits.h> // PATH_MAX
#endif

#ifdef WINDOWS
#include <windows.h> // MAX_PATH
#define PATH_MAX MAX_PATH
#endif

#undef max // windows.h defines a 'max'... (we want std::max())
#include <algorithm> // needs to be under windows.h

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/CubeImage.h"
#include "a3d/Font.h"
#include "a3d/Image.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/Math.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/light/Light.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Private Static Prototypes ///

static void StringFromTreeRec(Node& n, stringstream& ss, unsigned depth);

/// Output ///

string a3d::utils::StringFromTree(const Node& root) {

	stringstream ss;
	string name = (root.name() ? "\"" + *(root.name()) + "\"" : "null");
	ss << "[NODE] (" << static_cast<const void*>(&root) << ", " << name << ")" << endl;

	unsigned depth = 0;

	for (const auto& c : root.children(false)) {
		 StringFromTreeRec(*c, ss, depth+1);
	}

	return ss.str();
}

string a3d::utils::DateTimeString() {
	constexpr size_t BUF_SIZE = 128;
	char buf[BUF_SIZE];
#ifdef WINDOWS
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buf, sizeof(buf), "%Y.%m.%d_%I.%M.%S", timeinfo);
	return string(buf);
#else
	timeval curTime;
	gettimeofday(&curTime, NULL); // gettimeofday() is POSIX
	int milli = curTime.tv_usec / 1000;
	strftime(buf, sizeof(buf), "%Y.%m.%d_%H.%M.%S", localtime(&curTime.tv_sec));
	char msBuf[std::strlen(buf) + 5];
	snprintf(msBuf, sizeof(msBuf), "%s.%03d", buf, milli);
	return string(msBuf);
#endif
}

#ifdef POSIX
string a3d::utils::StackTrace(unsigned dropFunctions) {

	auto traceStr = string();
	static const unsigned MAX_FRAMES = 64;

	void* addrList[MAX_FRAMES];
	unsigned addrLen = backtrace(addrList, sizeof(addrList) / sizeof(void*));

	if (addrLen != 0) {
		char** symbolList = backtrace_symbols(addrList, addrLen);
		for (int x=dropFunctions+1; x<addrLen; ++x) {
			traceStr += string(symbolList[x]) + "\n";
		}

		free(symbolList);
	}
	else {
		traceStr = "No stack trace.\n";
	}

	return traceStr;
}
#endif

/// Numeric ///

int a3d::utils::Uniform(int min, int max) {
	static random_device rd;
	static mt19937 gen(rd());
	uniform_int_distribution<> dis(min, max);
	return dis(gen);
}

float a3d::utils::Uniform(float min, float max) {
	static random_device rd;
	static mt19937 gen(rd());
	uniform_real_distribution<> dis(min, max);
	return dis(gen);
}

/// Time ///

double a3d::utils::Time() {

	auto now = chrono::system_clock::now();
	return chrono::duration<double>(now.time_since_epoch()).count();
}

/// String ///

void a3d::utils::Replace(string &str,
						 const std::string &oldStr,
						 const std::string &newStr) {
	string::size_type pos = 0u;
	while ((pos = str.find(oldStr, pos)) != string::npos) {
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}

vector<string> a3d::utils::Split(const string& s, string delim) {
	// https://stackoverflow.com/a/46931770

	size_t pos_start = 0, pos_end, delim_len = delim.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delim, pos_start)) != string::npos) {
		token = s.substr (pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}
	res.push_back(s.substr (pos_start));

	return res;
}

/// Filesystem ///

// *** executable and working directories ***

std::optional<std::filesystem::path> a3d::utils::ExecutablePath() {
#if defined(MACOS)
	char path[PATH_MAX];
	uint32_t size = sizeof(path);
	if (_NSGetExecutablePath(path, &size) == 0) {
		return std::filesystem::path(path);
	}
#elif defined(LINUX)
	// https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
	char path[PATH_MAX];
	ssize_t count = std::min(size_t(readlink("/proc/self/exe", path, PATH_MAX)),
							 size_t(PATH_MAX - 1));
	if (count >= 0) {
		path[count] = '\0';
		return std::filesystem::path(path);
	}
#elif defined(WINDOWS)
	char path[PATH_MAX];
	if (GetModuleFileName(NULL, path, PATH_MAX)) {
		return std::filesystem::path(path);
	}
#endif
	return std::nullopt;
}

std::optional<std::filesystem::path> a3d::utils::ExecutableDirectory() {
	auto execPathStr = ExecutablePath();
	if (execPathStr) {
		auto execPath = std::filesystem::path(*execPathStr);
		return execPath.parent_path();
	}
	return std::nullopt;
}

std::optional<std::string> a3d::utils::ExecutableName() {
	auto execPathStr = ExecutablePath();
	if (execPathStr) {
		auto execPath = std::filesystem::path(*execPathStr);
		//if (is_regular_file(execPath)) {
			return execPath.filename().string();
		//}
	}
	return std::nullopt;
}

std::optional<std::filesystem::path> a3d::utils::CurrentWorkingDirectory() {
#ifdef POSIX
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd))) {
		return std::filesystem::path(cwd);
	}
#else
	char path[PATH_MAX];
	if (GetModuleFileName(NULL, path, PATH_MAX)) {
		return std::filesystem::path(path);
	}
#endif
	return std::nullopt;
}

// *** search paths ***

vector<std::filesystem::path> a3d::utils::BaseSearchPaths() {
	// build a list of common directories where "shader", "scene", "images", "fonts", etc
	// subdirectories may live.
	// clients will use this to append those subdirectory names to search for specific resources.
	// clients should first check "local" locations first, then "engine" locations.
	
	auto basePaths = vector<std::filesystem::path>();
	auto execDir = ExecutableDirectory();
	
	if (execDir) {
		// [local] archived
		// [local] cmake installed ("packaged")
		auto path = (*execDir) / "data";
		basePaths.push_back(path);
		
		// [local] xcode debug
		path = (*execDir).parent_path().parent_path().parent_path().parent_path() / "tests" / "data";
		basePaths.push_back(path);
		
		// [local] unix/msys debug
		path = (*execDir).parent_path().parent_path().parent_path() / "tests" / "data";
		basePaths.push_back(path);
		
		// [engine] archived
		path = (*execDir).parent_path() / "data";
		basePaths.push_back(path);
		
		// [engine] cmake installed ("packaged")
		// [engine] xcode debug
		path = (*execDir).parent_path().parent_path().parent_path().parent_path() / "data";
		basePaths.push_back(path);
		
		// [engine] unix/msys debug
		path = (*execDir).parent_path().parent_path().parent_path() / "data";
		basePaths.push_back(path);

		// [local] macos bundle
		path = (*execDir).parent_path().parent_path().parent_path().parent_path().parent_path().parent_path() / "tests" / "data";
		basePaths.push_back(path);

		// engine
		path = (*execDir).parent_path().parent_path().parent_path().parent_path().parent_path().parent_path() / "data";
		basePaths.push_back(path);
		
		// fallback
		path = (*execDir);
		basePaths.push_back(path);
	}
	
	return basePaths;
}

vector<std::filesystem::path> a3d::utils::ShaderSearchPaths() {
	auto searchPaths = vector<std::filesystem::path>();
	for (auto& path : BaseSearchPaths()) {
		searchPaths.push_back(path / "shaders");
	}
	return searchPaths;
}

vector<std::filesystem::path> a3d::utils::SceneSearchPaths() {
	auto searchPaths = vector<std::filesystem::path>();
	for (auto& path : BaseSearchPaths()) {
		searchPaths.push_back(path / "scenes");
	}
	return searchPaths;
}

vector<std::filesystem::path> a3d::utils::ModelSearchPaths() {
	auto searchPaths = vector<std::filesystem::path>();
	for (auto& path : BaseSearchPaths()) {
		searchPaths.push_back(path / "models");
	}
	return searchPaths;
}

vector<std::filesystem::path> a3d::utils::ImageSearchPaths() {
	auto searchPaths = vector<std::filesystem::path>();
	for (auto& path : BaseSearchPaths()) {
		searchPaths.push_back(path / "images");
	}
	return searchPaths;
}

vector<std::filesystem::path> a3d::utils::FontSearchPaths() {
	auto searchPaths = vector<std::filesystem::path>();
	for (auto& path : BaseSearchPaths()) {
		searchPaths.push_back(path / "fonts");
	}
	return searchPaths;
}

std::optional<std::filesystem::path> a3d::utils::SearchInPaths(const string& filename,
															  vector<std::filesystem::path> paths) {
	for (auto& searchPath : paths) {
		if (std::filesystem::is_directory(searchPath)) {
			auto path = searchPath / filename;
			if (std::filesystem::is_regular_file(path)) {
				//A3D_LOG_D("Found '{}' at '{}'", searchPath.string(), filename);
				return path;
			}
		}
	}
	A3D_LOG_W("'{}' not found.", filename);
	return std::nullopt;
}

// *** binary and text files ***

std::optional<string> a3d::utils::TextFile(const std::filesystem::path& path) {
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
	return std::nullopt;
}

// *** shaders ***

std::optional<std::string> a3d::utils::ShaderSource(const string& name,
													 const string& type) {
	std::optional<string> rawSource = std::nullopt;
	auto path = SearchInPaths((name + "." + type), ShaderSearchPaths());
	if (path) {
		A3D_LOG_D("Found shader at path: {}", (*path).string());
		rawSource = TextFile(*path);
	}
	return rawSource;
}

// *** fonts ***

//unique_ptr<a3d::Font> a3d::utils::FontNamed(const string& filename) {
//	auto path = filesystem::path(filename);
//	return FontNamed(path.stem(), path.extension());
//}

unique_ptr<Font> a3d::utils::FontNamed(const string& name,
									  const string& type) {
	auto path = SearchInPaths((name + "." + type), FontSearchPaths());
	if (path) {
		A3D_LOG_T("Found font at path: {}", (*path).string());
		return make_unique<Font>(*path);
	}
	return nullptr;
}


// ***  images ***

unique_ptr<Image> a3d::utils::ImageNamed(const string& name,
										bool flipHorizontal,
										bool flipVertical) {

	return ImageNamed(name, "png", flipHorizontal, flipVertical);
}

unique_ptr<Image> a3d::utils::ImageNamed(const string& name,
										const string& type,
										bool flipHorizontal,
										bool flipVertical) {
	auto path = SearchInPaths((name + "." + type), ImageSearchPaths());
	if (path) {
		A3D_LOG_D("Found image at path: {}", (*path).string());
		return make_unique<Image>(*path, flipHorizontal, flipVertical);
	}
	return nullptr;
}

unique_ptr<CubeImage> a3d::utils::CubeImageNamed(const string& name) {
	return CubeImageNamed(name, "png");
}

unique_ptr<CubeImage> a3d::utils::CubeImageNamed(const string& name,
												const string& type) {
	
	// panorama to cubemap: https://jaxry.github.io/panorama-to-cubemap/

	return make_unique<CubeImage>(ImageNamed(name + "_posx", type, false, true),
								  ImageNamed(name + "_negx", type, false, true),
								  ImageNamed(name + "_posy", type, true, false),
								  ImageNamed(name + "_negy", type, true, false),
								  ImageNamed(name + "_posz", type, false, true),
								  ImageNamed(name + "_negz", type, false, true));
}

// *** scenes ***

unique_ptr<Scene> a3d::utils::SceneNamed(const string& name,
										SceneImportOptions options) {

	return SceneNamed(name, "gltf", options);
}

unique_ptr<Scene> a3d::utils::SceneNamed(const string& name,
										const string& type,
										SceneImportOptions options) {
	
	auto path = SearchInPaths((name + "." + type), SceneSearchPaths());
	if (path) {
		A3D_LOG_T("Found scene at path: {}", (*path).string());
		return Scene::FromFile(*path, options);
	}
	return nullptr;
}

shared_ptr<Mesh> a3d::utils::MeshNamed(const string& name,
									  MeshImportOptions options) {

	return MeshNamed(name, "gltf", options);
}

shared_ptr<Mesh> a3d::utils::MeshNamed(const string& name,
									  const string& type,
									  MeshImportOptions options) {

	auto path = SearchInPaths((name + "." + type), ModelSearchPaths());
	if (path) {
		A3D_LOG_T("Found scene at path: {}", (*path).string());
		return Mesh::FromFile(*path, options);
	}
	return nullptr;
}

/// Misc ///

void a3d::utils::SaveSnapshot(RenderContext& context) {

	auto execDir = ExecutableDirectory();
	if (execDir) {
		auto filename = std::format("Snapshot_{}.png", DateTimeString());
		A3D_LOG_I("Saving snapshot to '{}'", (*execDir/filesystem::path(filename)).string());
		auto image = context.snapshot();
		auto fullPath = *execDir / filename;
		image->writePNG(fullPath);
	}
	else {
		A3D_LOG_E("Failed to save snapshot.  Couldn't locate executable directory.");
	}
}

void a3d::utils::StartGIFRecording(RenderContext& context,
								   uvec2 fitInside,
								   unsigned maxFramerate) {

	auto execDir = ExecutableDirectory();
	if (execDir) {
		auto filename = std::format("Recording_{}.gif", DateTimeString());
		A3D_LOG_I("Starting GIF recording at '{}'", (*execDir/filesystem::path(filename)).string());
		auto fullPath = *execDir / filename;
		context.startGIFRecording(fullPath.string(), fitInside, maxFramerate);
	}
	else {
		A3D_LOG_W("Couldn't locate executable directory.");
	}
}

void a3d::utils::StopGIFRecording(RenderContext& context) {

	context.stopGIFRecording();
}

/// Private ///

void StringFromTreeRec(Node& n, stringstream& ss, unsigned depth) {

	string padding = "";
	for (unsigned d=0; d<depth; ++d) {
		padding += "\t";
	}
	string nodeName = (n.name() ? "\"" + *(n.name()) + "\"" : "null");
	ss << padding << "[NODE] (" << static_cast<const void*>(&n)
	<< ", " << nodeName << ")" << endl;

	auto mesh = n.mesh();
	if (mesh) {
		string meshName = (mesh->name() ? "\"" + *(mesh->name()) + "\"" : "null");
		ss << padding << "\t[MESH] (" << static_cast<const void*>(mesh.get())
		   << ", " << meshName << ")" << endl;

		for (auto& element : mesh->elements()) {
			ss << padding << "\t\t[ELEMENT] (" << static_cast<const void*>(element.get())<< ")" << endl;
		}

		for (auto& material : mesh->materials()) {

			string properties = "";
			if (!holds_alternative<monostate>(material->ambient())) properties += "a";
			if (!holds_alternative<monostate>(material->diffuse())) properties += "d";
			if (!holds_alternative<monostate>(material->specular())) properties += "s";
			if (!holds_alternative<monostate>(material->emission())) properties += "e";

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
