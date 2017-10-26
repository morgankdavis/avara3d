//
//  Utilities.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Utilities.h"

#include <fstream>
#include <memory>

#include <assimp/cimport.h>
#include <glm/gtc/quaternion.hpp>

#include "Scene.h"


using namespace std;
using namespace glm;
using namespace boost;
using namespace ae;


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
			 
			 /*m[0][0], m[0][1], m[0][2], m[0][3], // consistent with to_string and SceneKit notation...
			 m[1][0], m[1][1], m[1][2], m[1][3],
			 m[2][0], m[2][1], m[2][2], m[2][3],
			 m[3][0], m[3][1], m[3][2], m[3][3]);*/
	
	return (os << str);
}

optional<string> ae::utils::loadTextFile(const string &path) {
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

vec2 ae::utils::aiVector3DToGLMVec2(const aiVector2D& from) {
	return vec2(from.x, from.y);
}

vec3 ae::utils::aiVector3DToGLMVec3(const aiVector3D& from) {
	return vec3(from.x, from.y, from.z);
}

// https://github.com/mruan/gl-exp/blob/master/src/math_util.hpp
// copy from Row-major to Column major matrix
// i.e. from aiMatrix4x4 to glm::mat4
//template <typename RM, typename CM>
//void ae::utils::RowMajorToColumnMajorMat4(const RM& from, CM& to) {
mat4 ae::utils::aiMaxtrix4x4ToGLMMat4(const aiMatrix4x4& from) {
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



std::string ae::utils::ShaderSourceDirectoryPath() {
#ifdef XCODE
	return "../../../../avara-engine/shaders/";
#else
	return "../../../avara-engine/shaders/";
#endif
}

std::shared_ptr<std::string> ae::utils::ShaderSourceNamed(const std::string& name, const std::string& type) {
	string fullPath = ShaderSourceDirectoryPath() + name + "." + type;
	
	auto source = loadTextFile(fullPath);
	if (source) {
		return make_shared<string>(*source);
	}
	return nullptr;
}



vector<string> ae::utils::pathComponents(const string& str, const set<char> delimiters) {
	vector<string> result;
	
	char const* pch = str.c_str();
	char const* start = pch;
	for(; *pch; ++pch)
	{
		if (delimiters.find(*pch) != delimiters.end())
		{
			if (start != pch)
			{
				std::string str(start, pch);
				result.push_back(str);
			}
			else
			{
				result.push_back("");
			}
			start = pch + 1;
		}
	}
	result.push_back(start);
	
	return result;
}

string ae::utils::pathFromComponents(const vector<string> components, const char delimiter) {
	string path = "";
	for (unsigned int i=0; i<components.size(); ++i) {
		path += components[i];
		if (i < components.size()-1) { // not last item
			path += delimiter;
		}
	}
	return path;
}

