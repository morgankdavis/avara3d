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
#include <assimp/vector2.h>
#include <boost/optional.hpp>
#include <glm/glm.hpp>

#include "Types.h"


namespace ae {


	class Color;
	class Scene;


	namespace utils {

		std::ostream& operator<<(std::ostream& os, const glm::vec3& v);
		std::ostream& operator<<(std::ostream& os, const glm::vec4& v);
		std::ostream& operator<<(std::ostream& os, const glm::quat& q);
		std::ostream& operator<<(std::ostream& os, const glm::mat4& m);
		
		boost::optional<std::string> LoadTextFile(const std::string &path);

		glm::vec2 AIVector3DToGLMVec2(const aiVector2D& from);
		glm::vec3 AIVector3DToGLMVec3(const aiVector3D& from);
		//		template <typename RM, typename CM>
//		void RowMajorToColumnMajorMat4(const RM& from, CM& to);
		glm::mat4 AIMaxtrix4x4ToGLMMat4(const aiMatrix4x4& from);
		Color AIColor4DToColor(const aiColor4D& from);
		
		std::string ShaderSourceDirectoryPath();
		std::string ShaderPath(const std::string& name, const std::string& type);
		std::shared_ptr<std::string> ShaderSourceNamed(const std::string& name, const std::string& type);

		std::string TestDataDirectoryPath();
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name);
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name,
											  const std::string& type);
		
//		std::vector<std::string> pathComponents(const std::string& str, const std::set<char> delimiters);
//		std::string pathFromComponents(const std::vector<std::string> components, const char delimiter);
//		char* ae_realpath(const char* path, char* resolved_path);
	}
}

#endif /* Utilities_h */
