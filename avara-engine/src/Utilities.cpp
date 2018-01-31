//
//  Utilities.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Utilities.h"

#include <algorithm>
#include <ctime>
#include <sstream>
#include <fstream>
#include <memory>

#ifdef WINDOWS
#include <Windows.h>
#endif

#if defined(MACOS) || defined(LINUX)
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#endif

#ifdef LINUX
#include <libgen.h>
#endif

#include <glm/gtc/quaternion.hpp>

#ifdef MACOS
#include <CoreGraphics/CoreGraphics.h>
#include <mach-o/dyld.h>
#endif

#include <GLFW/glfw3.h>
#ifdef MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include "Color.h"
#include "Image.h"
#include "Logger.h"
#include "Scene.h"
#include "Window.h"



using namespace std;
using namespace glm;
using namespace boost;
using namespace ae;


/***************************************************************************************
 MARK:   Output Utilities
 **************************************************************************************/

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

/***************************************************************************************
 MARK:   Conversion Utilities
 **************************************************************************************/

std::string ae::utils::DateTimeString() {
	
	char buffer[256];
	
#ifdef WINDOWS
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	strftime(buffer, sizeof(buffer), "%Y.%m.%d_%I.%M.%S", timeinfo);
#else
	// gettimeofday() is POSIX
	
	timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000;
	
	strftime(buffer, sizeof(buffer), "%Y.%m.%d_%H.%M.%S", localtime(&curTime.tv_sec));
	sprintf(buffer, "%s.%03d", buffer, milli);
#endif
	
	return string(buffer);
}

vec2 ae::utils::GLMVec2FromAIVector3D(const aiVector2D& from) {
	return vec2(from.x, from.y);
}

vec3 ae::utils::GLMVec3FromAIVector3D(const aiVector3D& from) {
	return vec3(from.x, from.y, from.z);
}

// https://github.com/mruan/gl-exp/blob/master/src/math_util.hpp
mat4 ae::utils::GLMMat4FromAIMaxtrix4x4(const aiMatrix4x4& from) {
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

Color ae::utils::ColorFromAIColor3D(const aiColor3D& from) {
	return Color(from.r, from.g, from.b, 1.0f);
}

Color ae::utils::ColorFromAIColor4D(const aiColor4D& from) {
	return Color(from.r, from.g, from.b, from.a);
}

btVector3 ae::utils::BTVector3FromGLMVec3(glm::vec3& from) {
	return btVector3(from.x, from.y, from.z);
}

btVector4 ae::utils::BTVector4FromGLMVec4(glm::vec4& from) {
	return btVector4(from.x, from.y, from.z, from.w);
}

/***************************************************************************************
 MARK:   Error Utilities
 **************************************************************************************/

void ae::utils::CheckGLError() {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		AE_LOG->warn("*** glGetError: {} ***", err);
	}
}

/***************************************************************************************
 MARK:   Numeric Utilities
 **************************************************************************************/

int ae::utils::Random(int min, int max) {
	return (min + (rand() % static_cast<int>(max - min + 1)));
}

float ae::utils::Random(float min, int max) {
	float random = ((float) rand()) / (float) RAND_MAX;
	float diff = max - min;
	float r = random * diff;
	return (min + r);
}

bool ae::utils::Zero(const vec3& v) {
	static float tolerance = 0.00001f;
	return FloatEqual(v.x, 0, tolerance) && FloatEqual(v.y, 0, tolerance) && FloatEqual(v.z, 0, tolerance);
}

float ae::utils::Max(const vec3& v) {
	return std::max(std::max(v.x, v.y), v.z);
}

bool ae::utils::FloatEqual(float a, float b, float tolerance) {
	return (fabs(a - b) <= tolerance);
}

/***************************************************************************************
 MARK:   File Utilities
 **************************************************************************************/

boost::optional<string> ae::utils::ExecutablePath() {
#if defined(MACOS)
	
	// https://stackoverflow.com/questions/799679/programmatically-retrieving-the-absolute-path-of-an-os-x-command-line-app/1024933#1024933
	// https://developer.apple.com/legacy/library/documentation/Darwin/Reference/ManPages/man3/dyld.3.html
	
	char path[1024];
	uint32_t size = sizeof(path);
	if (_NSGetExecutablePath(path, &size) == 0) {
		return string(path);
	}
	
#elif defined(LINUX)
	
	char path[1024];
	ssize_t count = readlink("/proc/self/exe", path, 1024);
	if (count != -1) {
		//path = dirname(path);
		return string(path);
	}
	
#elif defined(WINDOWS)
	
	// https://stackoverflow.com/questions/18783087/how-to-properly-use-getmodulefilename
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197(v=vs.85).aspx
	
	//wchar_t path[1024];
	char path[1024];
	if (GetModuleFileName(NULL, path, 1024)) {
		//wstring ws(path);
		//return string(ws.begin(), ws.end());
		return string(path);
	}
	
#endif
	
	return boost::none;
}

boost::optional<string> ae::utils::ExecutableDirectory() {
	return boost::none;
}

boost::optional<string> ae::utils::CurrentWorkingDirectory() {
#if defined(MACOS) || defined(LINUX)
	
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd))) {
		return string(cwd);
	}
	
#else
	
	// https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
	
	//wchar_t path[1024];
	char path[1024];
	if (GetModuleFileName(NULL, path, 1024)) {
		//wstring ws(path);
		//return string(ws.begin(), ws.end());
		return string(path);
	}
	
#endif
	
	return boost::none;
}

optional<string> ae::utils::LoadTextFile(const string &path) {
	string line;
	string source = "";
	ifstream infile;
	const char *path_cstr = path.c_str();
	infile.open(path_cstr);
	if (infile.is_open()) {
		while (!infile.eof()) {
			getline(infile, line);
			source += line;
			source += "\n";
		}
		infile.close();
		return source;
	}
	return {};
}

std::string ae::utils::ShaderSourceDirectoryPath() {
#ifdef XCODE
	return "../../../../avara-engine/shaders/";
#else
	return "../../../avara-engine/shaders/";
#endif
}

string ae::utils::ShaderPath(const string& name, const string& type) {
	return ShaderSourceDirectoryPath() + name + "." + type;
}

std::shared_ptr<std::string> ae::utils::ShaderSourceNamed(const std::string& name, const std::string& type) {
	string fullPath = ShaderPath(name, type);
	
	auto source = LoadTextFile(fullPath);
	if (source) {
		return make_shared<string>(*source);
	}
	return nullptr;
}

std::string ae::utils::ImagesDirectoryPath() {
#ifdef XCODE
	return "../../../../avara-engine/images/";
#else
	return "../../../avara-engine/images/";
#endif
}

std::shared_ptr<Image> ae::utils::ImageNamed(const std::string& name, const std::string& type) {
	string fullPath = ImagePath(name, type);
	
	return make_shared<Image>(fullPath);
}

std::string ae::utils::ImagePath(const std::string& name, const std::string& type) {
	return ImagesDirectoryPath() + name + "." + type;
}

string ae::utils::TestDataDirectoryPath() {
#ifdef XCODE
	return "../../../../tests/testdata/";
#else
	return "../../../tests/testdata/";
#endif
}

std::shared_ptr<Scene> ae::utils::TestSceneNamed(const string& name) { // why is shared_ptr scoped?
	return TestSceneNamed(name, "dae");
}

std::shared_ptr<Scene> ae::utils::TestSceneNamed(const string& name,
												 const string& type) {
	
	string fullPath = TestDataDirectoryPath() + "scenes/" + name + "." + type;
	return make_shared<Scene>(fullPath);
}

std::shared_ptr<Image> ae::utils::TestImageNamed(const std::string& name,
												 bool flipHorizontal) {
	return TestImageNamed(name, "png", flipHorizontal);
}

std::shared_ptr<Image> ae::utils::TestImageNamed(const std::string& name,
												 const std::string& type,
												 bool flipHorizontal) {
	
	string fullPath = TestDataDirectoryPath() + "images/" + name + "." + type;
	return make_shared<Image>(fullPath, flipHorizontal);
}

std::shared_ptr<std::vector<std::shared_ptr<Image>>> ae::utils::TestCubeNamed(const std::string& name,
																			  const std::string& type) {
	
	auto cube = make_shared<vector<std::shared_ptr<Image>>>();
	
	cube->push_back(TestImageNamed(name + "_posx", type, false));
	cube->push_back(TestImageNamed(name + "_negx", type, false));
	cube->push_back(TestImageNamed(name + "_posy", type, false));
	cube->push_back(TestImageNamed(name + "_negy", type, false));
	cube->push_back(TestImageNamed(name + "_posz", type, false));
	cube->push_back(TestImageNamed(name + "_negz", type, false));
	
	return cube;
}


std::string ae::utils::FontsDirectoryPath() {
#ifdef XCODE
	return "../../../../avara-engine/fonts/";
#else
	return "../../../avara-engine/fonts/";
#endif
}

std::string ae::utils::FontPath(const std::string& name, const std::string& type) {
	return FontsDirectoryPath() + name + "." + type;
}

/***************************************************************************************
 MARK:   Misc Utilities
 **************************************************************************************/

void ae::utils::SaveSnapshot(Window& window) {
	
	auto image = window.snapshot();
	
	string dateTime = DateTimeString();
	
	char filename[256] = "";
	sprintf(filename, "Snapshot_%s.png", dateTime.c_str());
	
	AE_LOG->info("Saving snapshot '{}'...", filename);
	
	image->writePNG(filename);
}

void ae::utils::StartGIFRecording(Window& window, unsigned maxHeight, unsigned maxFramerate) {
	char filename[256] = "";
	sprintf(filename, "Recording_%s.gif", DateTimeString().c_str());
	window.startGIFRecording(filename, maxHeight, maxFramerate);
}

void ae::utils::StopGIFRecording(Window& window) {
	window.stopGIFRecording();
}

float ae::utils::GetScreenScaleFactor(GLFWmonitor* monitor) {

#ifdef MACOS
    
    //GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    //GLFWmonitor* monitor = glfwGetWindowMonitor(glfwWindow);
    CGDirectDisplayID cgDisplayID = glfwGetCocoaMonitor(monitor);
    CGDisplayModeRef currentModeRef = CGDisplayCopyDisplayMode(cgDisplayID);
    
    
    Size width = CGDisplayModeGetWidth(currentModeRef);
    Size pixelWidth = CGDisplayModeGetPixelWidth(currentModeRef);
    return (float)pixelWidth / (float)width;
    
#endif
    
    return 1.0;
}
