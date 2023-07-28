//
//  Utilities.h
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Utilities_h
#define Utilities_h


#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

//#include <boost/optional.hpp>
//#include <boost/filesystem.hpp>
#include <glm/glm.hpp>

#include "Aliases.h"


struct GLFWmonitor;


namespace ae {
	
	
	class Buffer;
	class Color;
	class CubeImage;
	class Font;
	class Image;
	class Node;
	class RenderContext;
	class Scene;
	
	
	namespace utils {
		
/*********************************************************************************************
	Output Utilities
 *********************************************************************************************/
		
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
		
		std::string StringFromTree(Node& root);
		
		std::string DateTimeString();
		
#if defined(MACOS) || defined(LINUX)
		std::string StackTrace(unsigned dropFunctions = 0);
#endif

/*********************************************************************************************
	Numeric Utilities
 *********************************************************************************************/
		
		int Uniform(int min, int max);
		float Uniform(float min, int max);
		
		bool Zero(const glm::vec3& v, float tolerance = 0.0001);
		float Max(const glm::vec3& v);
		
		bool Equal(float a, float b, float tolerance = 0.0001);
		bool Equal(const glm::vec3& a, const glm::vec3& b, float tolerance = 0.0001);
		bool Equal(const glm::vec4& a, const glm::vec4& b, float tolerance = 0.0001);
		
/*********************************************************************************************
	String Utilities
 *********************************************************************************************/
		
		void StringReplace(std::string& str,
						   const std::string& oldStr,
						   const std::string& newStr);

/*********************************************************************************************
	File Utilities
 *********************************************************************************************/

		// *** executable and working directories ***
		
#ifndef ANDROID
		std::optional<std::filesystem::path> ExecutablePath();
		std::optional<std::filesystem::path> ExecutableDirectory();
		std::optional<std::filesystem::path> ExecutableName();
		std::optional<std::filesystem::path> CurrentWorkingDirectory();
#endif
		
		// *** search paths ***
		
#ifndef ANDROID
		std::vector<std::filesystem::path> BaseSearchPaths();
		std::vector<std::filesystem::path> ShaderSearchPaths();
		std::vector<std::filesystem::path> SceneSearchPaths();
		std::vector<std::filesystem::path> ImageSearchPaths();
		std::vector<std::filesystem::path> FontSearchPaths();
		std::optional<std::filesystem::path> SearchInPaths(const std::string& filename,
															   std::vector<std::filesystem::path> paths);
#endif
		
		// *** binary and text files ***
		
#ifdef ANDROID
		std::optional<std::filesystem::path> InternalFilesDirectory();
		std::optional<std::string> TextAsset(const std::string& relPath);
		std::shared_ptr<Buffer> BinaryAsset(const std::string& relPath);
#else
		std::optional<std::string> TextFile(const std::filesystem::path& path);
#endif
		
		// *** shaders ***
		
		std::optional<std::string> ShaderSource(const std::string& name,
												  const std::string& type);
		
		// *** fonts ***

		FontSPtr FontNamed(const std::string& name,
										const std::string& type);
		
		// ***  images ***
		
		ImageSPtr ImageNamed(const std::string& name,
										  bool flipHorizontal=true);
		ImageSPtr ImageNamed(const std::string& name,
										  const std::string& type,
										  bool flipHorizontal=true);
		CubeImageSPtr CubeImageNamed(const std::string& name);
		CubeImageSPtr CubeImageNamed(const std::string& name,
												  const std::string& type);
		
		// *** scenes ***
		
#ifndef ANDROID
		SceneSPtr SceneNamed(const std::string& name);
		SceneSPtr SceneNamed(const std::string& name,
										  const std::string& type);
#endif

/*********************************************************************************************
	Misc Utilities
 *********************************************************************************************/
		
		void SaveSnapshot(RenderContext& context);
		void StartGIFRecording(RenderContext& context,
							   unsigned maxHeight, unsigned maxFramerate);
		void StopGIFRecording(RenderContext& context);
	}
}

#endif /* Utilities_h */
