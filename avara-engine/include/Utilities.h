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

#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <glm/glm.hpp>

#include "Types.h"


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
		boost::optional<boost::filesystem::path> ExecutablePath();
		boost::optional<boost::filesystem::path> ExecutableDirectory();
		boost::optional<boost::filesystem::path> ExecutableName();
		boost::optional<boost::filesystem::path> CurrentWorkingDirectory();
#endif
		
		// *** search paths ***
		
#ifndef ANDROID
		std::vector<boost::filesystem::path> BaseSearchPaths();
		std::vector<boost::filesystem::path> ShaderSearchPaths();
		std::vector<boost::filesystem::path> SceneSearchPaths();
		std::vector<boost::filesystem::path> ImageSearchPaths();
		std::vector<boost::filesystem::path> FontSearchPaths();
		boost::optional<boost::filesystem::path> SearchInPaths(const std::string& filename,
															   std::vector<boost::filesystem::path> paths);
#endif
		
		// *** binary and text files ***
		
#ifdef ANDROID
		boost::optional<boost::filesystem::path> InternalFilesDirectory();
		boost::optional<std::string> TextAsset(const std::string& relPath);
		std::shared_ptr<Buffer> BinaryAsset(const std::string& relPath);
#else
		boost::optional<std::string> TextFile(const boost::filesystem::path& path);
#endif
		
		// *** shaders ***
		
		boost::optional<std::string> ShaderSource(const std::string& name,
												  const std::string& type);
		
		// *** fonts ***

		std::shared_ptr<Font> FontNamed(const std::string& name,
										const std::string& type);
		
		// ***  images ***
		
		std::shared_ptr<Image> ImageNamed(const std::string& name,
										  bool flipHorizontal=true);
		std::shared_ptr<Image> ImageNamed(const std::string& name,
										  const std::string& type,
										  bool flipHorizontal=true);
		std::shared_ptr<CubeImage> CubeImageNamed(const std::string& name);
		std::shared_ptr<CubeImage> CubeImageNamed(const std::string& name,
												  const std::string& type);
		
		// *** scenes ***
		
#ifndef ANDROID
		std::shared_ptr<Scene> SceneNamed(const std::string& name);
		std::shared_ptr<Scene> SceneNamed(const std::string& name,
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
