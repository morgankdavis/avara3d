//
//  Utilities.h
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Utilities_h
#define Utilities_h


#include <fstream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <assimp/color4.h>
#include <assimp/matrix4x4.h>
#include <assimp/types.h>
#include <assimp/vector2.h>
#include <boost/optional.hpp>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "Types.h"


struct GLFWmonitor;


namespace ae {


	class Color;
	class Image;
	class Scene;
	class Window;


	namespace utils {

		std::ostream& operator<<(std::ostream& os, const glm::vec3& v);
		std::ostream& operator<<(std::ostream& os, const glm::vec4& v);
		std::ostream& operator<<(std::ostream& os, const glm::quat& q);
		std::ostream& operator<<(std::ostream& os, const glm::mat4& m);
		std::ostream& operator<<(std::ostream& os, const Color& c);
		
		// work-around for spdlog
		std::string StringFromGLMVec3(const glm::vec3& v);
		std::string StringFromGLMVec4(const glm::vec4& v);
		std::string StringFromGLMQuat(const glm::quat& q);
		std::string StringFromGLMMat4(const glm::mat4& m);
		std::string StringFromColor(const Color& c);
		
		boost::optional<std::string> LoadTextFile(const std::string &path);

		glm::vec2 AIVector3DToGLMVec2(const aiVector2D& from);
		glm::vec3 AIVector3DToGLMVec3(const aiVector3D& from);
		glm::mat4 AIMaxtrix4x4ToGLMMat4(const aiMatrix4x4& from);
		Color AIColor3DToColor(const aiColor3D& from);
		Color AIColor4DToColor(const aiColor4D& from);
		
		btVector3 BTVector3FromGLMVec3(glm::vec3& from);
		btVector4 BTVector4FromGLMVec4(glm::vec4& from);
		
		void CheckGLError();
		
		int Random(int min, int max);
		float Random(float min, int max);
		
		bool Zero(const glm::vec3& v);
		float Max(const glm::vec3& v);
		
		bool FloatEqual(float a, float b, float tolerance);
		
		std::string ShaderSourceDirectoryPath();
		std::string ShaderPath(const std::string& name, const std::string& type);
		std::shared_ptr<std::string> ShaderSourceNamed(const std::string& name, const std::string& type);
		
		std::string ImagesDirectoryPath();
		std::string ImagePath(const std::string& name, const std::string& type);
		std::shared_ptr<Image> ImageNamed(const std::string& name, const std::string& type);
		std::shared_ptr<std::vector<std::shared_ptr<Image>>> TestCubeNamed(const std::string& name,
																		   const std::string& type);
		
		std::string FontsDirectoryPath();
		std::string FontPath(const std::string& name, const std::string& type);

		std::string TestDataDirectoryPath();
		
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name);
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name,
											  const std::string& type);
		
		std::shared_ptr<Image> TestImageNamed(const std::string& name,
											  bool flipHorizontal=true);
		std::shared_ptr<Image> TestImageNamed(const std::string& name,
											  const std::string& type,
											  bool flipHorizontal=true);
//		std::vector<std::shared_ptr<Image>> TestCubeMaterialPropertyNamed(const std::string& name,
//																		  const std::string& type);
		
//		std::vector<std::string> pathComponents(const std::string& str, const std::set<char> delimiters);
//		std::string pathFromComponents(const std::vector<std::string> components, const char delimiter);
//		char* ae_realpath(const char* path, char* resolved_path);
		
		std::string DateTimeString();
		
		void SaveSnapshot(Window& window);
		
		void StartGIFRecording(Window& window, unsigned maxHeight, unsigned maxFramerate);
		void StopGIFRecording(Window& window);


		//void GetScreenResolution(int& width, int& height);
        float GetScreenScaleFactor(GLFWmonitor* monitor);
		
		
		//void PrintAllChildNodeNames(std::shared_ptr<Node> theNode);
	}
}

#endif /* Utilities_h */
