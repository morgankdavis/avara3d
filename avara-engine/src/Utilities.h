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

#include <assimp/matrix4x4.h>
#include <assimp/vector2.h>
#ifdef WINDOWS
#include <optional.hpp>
#else
#include <boost/optional.hpp>
#endif
#include <glm/glm.hpp>

#include "Types.h"


namespace ae {


	class Scene;


	namespace utils {

		std::ostream& operator<<(std::ostream& os, const glm::vec3& v);
		std::ostream& operator<<(std::ostream& os, const glm::vec4& v);
		std::ostream& operator<<(std::ostream& os, const glm::quat& q);
		std::ostream& operator<<(std::ostream& os, const glm::mat4& m);
		
		boost::optional<std::string> loadTextFile(const std::string &path);

		glm::vec2 aiVector3DToGLMVec2(const aiVector2D& from);
		glm::vec3 aiVector3DToGLMVec3(const aiVector3D& from);
		//		template <typename RM, typename CM>
//		void RowMajorToColumnMajorMat4(const RM& from, CM& to);
		glm::mat4 aiMaxtrix4x4ToGLMMat4(const aiMatrix4x4& from);

		void TransformVertices(std::vector<Vertex>& verts, const glm::mat4 t, bool norm);
		
		std::string ShaderSourceDirectoryPath();
		std::shared_ptr<std::string> ShaderSourceNamed(const std::string& name, const std::string& type);

		std::string TestDataDirectoryPath();
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name);
		std::shared_ptr<Scene> TestSceneNamed(const std::string& name,
											  const std::string& type);
		
		std::vector<std::string> pathComponents(const std::string& str, const std::set<char> delimiters);
		std::string pathFromComponents(const std::vector<std::string> components, const char delimiter);
	}
}

#endif /* Utilities_h */
