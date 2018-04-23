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
#include <boost/filesystem.hpp>
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
		
		/***************************************************************************************
		 Output Utilities
		 ***************************************************************************************/
		
		std::ostream& operator<<(std::ostream& os, const glm::vec3& v);
		std::ostream& operator<<(std::ostream& os, const glm::vec4& v);
		std::ostream& operator<<(std::ostream& os, const glm::quat& q);
		std::ostream& operator<<(std::ostream& os, const glm::mat4& m);
		std::ostream& operator<<(std::ostream& os, const Color& c);
		
		std::string StringFromGLMVec3(const glm::vec3& v);
		std::string StringFromGLMVec4(const glm::vec4& v);
		std::string StringFromGLMQuat(const glm::quat& q);
		std::string StringFromGLMMat4(const glm::mat4& m);
		std::string StringFromColor(const Color& c);
		
		std::string DateTimeString();
		
		/***************************************************************************************
		 Conversion Utilities
		 ***************************************************************************************/
		
		glm::vec2 GLMVec2FromAIVector3D(const aiVector2D& from);
		glm::vec3 GLMVec3FromAIVector3D(const aiVector3D& from);
		glm::mat4 GLMMat4FromAIMaxtrix4x4(const aiMatrix4x4& from);
		Color ColorFromAIColor3D(const aiColor3D& from);
		Color ColorFromAIColor4D(const aiColor4D& from);
		
		glm::vec3 GLMVec3FromBTVector3(const btVector3& from);
		glm::vec4 GLMVec4FromBTVector4(const btVector4& from);
		btVector3 BTVector3FromGLMVec3(const glm::vec3& from);
		btVector4 BTVector4FromGLMVec4(const glm::vec4& from);
		
		/***************************************************************************************
		 Error Utilities
		 ***************************************************************************************/
		
		void CheckGLError();
		
		/***************************************************************************************
		 Numeric Utilities
		 ***************************************************************************************/
		
		int Random(int min, int max);
		float Random(float min, int max);
		
		bool Zero(const glm::vec3& v);
		float Max(const glm::vec3& v);
		
		bool FloatEqual(float a, float b, float tolerance = 0.001);
		
		/***************************************************************************************
		 File Utilities
		 ***************************************************************************************/
		
		boost::optional<boost::filesystem::path> ExecutablePath();
		boost::optional<boost::filesystem::path> ExecutableDirectory();
		boost::optional<boost::filesystem::path> CurrentWorkingDirectory();
		
		boost::optional<std::string> LoadTextFile(boost::filesystem::path& path);
		std::vector<unsigned char> LoadBinaryFile(boost::filesystem::path& path);
		
		boost::optional<boost::filesystem::path> ShadersDirectory();
		boost::optional<boost::filesystem::path> ShaderPath(const std::string& name, const std::string& type);
		
		boost::optional<boost::filesystem::path> ImagesDirectory();
		boost::optional<boost::filesystem::path> ImagePath(const std::string& name, const std::string& type);
		std::shared_ptr<Image> ImageNamed(const std::string& name, const std::string& type);
		std::shared_ptr<std::vector<std::shared_ptr<Image>>> TestCubeNamed(const std::string& name,
																		   const std::string& type);
		
		boost::optional<boost::filesystem::path> FontsDirectory();
		boost::optional<boost::filesystem::path> FontPath(const std::string& name, const std::string& type);
		
		boost::optional<boost::filesystem::path> TestDataDirectory();
		
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name);
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name,
											  const std::string& type);
		
		std::shared_ptr<Image> TestImageNamed(const std::string& name,
											  bool flipHorizontal=true);
		std::shared_ptr<Image> TestImageNamed(const std::string& name,
											  const std::string& type,
											  bool flipHorizontal=true);
		
		/***************************************************************************************
		 Misc Utilities
		 ***************************************************************************************/
		
		void SaveSnapshot(Window& window);
		
		void StartGIFRecording(Window& window, unsigned maxHeight, unsigned maxFramerate);
		void StopGIFRecording(Window& window);
		
		float GetScreenScaleFactor(GLFWmonitor* monitor);
	}
}

#endif /* Utilities_h */
