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
	
	
	class Color;
	class CubeImage;
	class Image;
	class RenderContext;
	class Scene;
	
	
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
		 	Numeric Utilities
		 ***************************************************************************************/
		
		int Uniform(int min, int max);
		float Uniform(float min, int max);
		
		bool Zero(const glm::vec3& v, float tolerance = 0.0001);
		float Max(const glm::vec3& v);
		
		bool Equal(float a, float b, float tolerance = 0.0001);
		bool Equal(const glm::vec3& a, const glm::vec3& b, float tolerance = 0.0001);
		bool Equal(const glm::vec4& a, const glm::vec4& b, float tolerance = 0.0001);
		
		/**************************************************************************************
		     String Utilities
		 **************************************************************************************/
		
		void StringReplace(std::string& str,
						   const std::string& oldStr,
						   const std::string& newStr);
		
		/***************************************************************************************
		 	File Utilities
		 ***************************************************************************************/
		
		// *** executable and working directories ***
		
#ifndef ANDROID
		boost::optional<boost::filesystem::path> ExecutablePath();
		boost::optional<boost::filesystem::path> ExecutableDirectory();
		boost::optional<boost::filesystem::path> CurrentWorkingDirectory();
#endif
		
		// *** binary and text files ***
		
		std::vector<unsigned char> Buffer(unsigned char* buf, unsigned len);
		unsigned Buffer(std::vector<unsigned char>& inBuf, unsigned char* outBuf);
		
#ifdef ANDROID
		boost::optional<boost::filesystem::path> InternalFilesDirectory();
		boost::optional<std::string> TextAsset(const std::string& relPath);
		std::vector<unsigned char> BinaryAsset(const std::string& relPath);
#else
		boost::optional<std::string> TextFile(const boost::filesystem::path& path);
		std::vector<unsigned char> BinaryFile(const boost::filesystem::path& path);
		unsigned BinaryFile(const boost::filesystem::path& path, std::vector<unsigned char> buffer);
#endif
		
		// *** engine shaders ***
		
		boost::optional<std::string> ShaderSource(const std::string& name,
												  const std::string& type);
#ifndef ANDROID
		boost::optional<boost::filesystem::path> ShadersDirectory();
		boost::optional<boost::filesystem::path> ShaderPath(const std::string& name,
															const std::string& type);
#endif
		
		// *** engine fonts ***
		
		std::vector<unsigned char> FontData(const std::string& name,
											const std::string& type);
#ifndef ANDROID
		boost::optional<boost::filesystem::path> FontsDirectory();
		boost::optional<boost::filesystem::path> FontPath(const std::string& name,
														  const std::string& type);
#endif
		
		// *** test directory ***
#ifndef ANDROID
		boost::optional<boost::filesystem::path> TestDataDirectory();
#endif
		
		// *** engine images ***
		
		std::shared_ptr<Image> ImageNamed(const std::string& name,
										  const std::string& type);
#ifndef ANDROID
		boost::optional<boost::filesystem::path> ImagesDirectory();
		boost::optional<boost::filesystem::path> ImagePath(const std::string& name,
														   const std::string& type);
#endif
		
		// *** test images ***
		
		std::shared_ptr<Image> TestImageNamed(const std::string& name,
											  bool flipHorizontal=true);
		std::shared_ptr<Image> TestImageNamed(const std::string& name,
											  const std::string& type,
											  bool flipHorizontal=true);
		std::shared_ptr<CubeImage> TestCubeImageNamed(const std::string& name,
													  const std::string& type);
		
		// *** test scenes ***

#ifndef ANDROID
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name);
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name,
											  const std::string& type);
#endif

		/***************************************************************************************
		 	Misc Utilities
		 ***************************************************************************************/
		
		void SaveSnapshot(RenderContext& context);
		void StartGIFRecording(RenderContext& context,
							   unsigned maxHeight, unsigned maxFramerate);
		void StopGIFRecording(RenderContext& context);
	}
}

#endif /* Utilities_h */
