//
//  string.cc
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/util/string.h"

#include <cstring>
#include <ctime>
#include <format>
#include <fstream>
#include <sstream>

#ifdef A3D_POSIX
#include <execinfo.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#ifdef A3D_MACOS
#include <CoreGraphics/CoreGraphics.h> // NEEDED?
#include <mach-o/dyld.h>
//#include <sys/syslimits.h> // PATH_MAX
#endif

//#ifdef A3D_WINDOWS
//#include <windows.h> // MAX_PATH
//#define PATH_MAX MAX_PATH
//#endif

#include "a3d/mesh/Mesh.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/light/Light.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/scene/Node.h"

using namespace a3d;
using namespace std;

void TreeStringRec(Node& n, stringstream& ss, unsigned depth);

void a3d::util::string::Replace(std::string &str,
						 const std::string &oldStr,
						 const std::string &newStr) {
	std::string::size_type pos = 0u;
	while ((pos = str.find(oldStr, pos)) != std::string::npos) {
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}

vector<string> a3d::util::string::Split(const std::string& s, std::string delim) {
	// https://stackoverflow.com/a/46931770

	size_t pos_start = 0, pos_end, delim_len = delim.length();
	std::string token;
	vector<std::string> res;

	while ((pos_end = s.find(delim, pos_start)) != std::string::npos) {
		token = s.substr (pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}
	res.push_back(s.substr (pos_start));

	return res;
}


string a3d::util::string::TreeString(const Node& root) {

	stringstream ss;
	std::string name = (root.name() ? "\"" + *(root.name()) + "\"" : "null");
	ss << "[NODE] (" << static_cast<const void*>(&root) << ", " << name << ")" << endl;

	unsigned depth = 0;

	for (const auto& c : root.children(false)) {
		TreeStringRec(*c, ss, depth+1);
	}

	return ss.str();
}

string a3d::util::string::DateTimeString() {
	constexpr size_t BUF_SIZE = 128;
	char buf[BUF_SIZE];
#ifdef A3D_WINDOWS
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
	return std::string(msBuf);
#endif
}

#ifdef A3D_POSIX
string a3d::util::string::StackTraceString(unsigned dropFunctions) {

	auto traceStr = std::string();
	static const unsigned MAX_FRAMES = 64;

	void* addrList[MAX_FRAMES];
	unsigned addrLen = backtrace(addrList, sizeof(addrList) / sizeof(void*));

	if (addrLen != 0) {
		char** symbolList = backtrace_symbols(addrList, addrLen);
		for (int x=dropFunctions+1; x<addrLen; ++x) {
			traceStr += std::string(symbolList[x]) + "\n";
		}

		free(symbolList);
	}
	else {
		traceStr = "No stack trace.\n";
	}

	return traceStr;
}
#endif

void TreeStringRec(Node& n, stringstream& ss, unsigned depth) {

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
		TreeStringRec(*c, ss, depth+1);
	}
}
