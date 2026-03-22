//
//  ALFImporter.h
//  avara3d
//
//  Created by Morgan Davis on 3/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

// *** This class probably ultimately belongs in a tool for converting map data,
// not in the engine. similarly, its dependency pugixml should eventually be removed. ***

#ifndef AVARA3D_APP_ALFIMPORTER_H
#define AVARA3D_APP_ALFIMPORTER_H

#include <filesystem>
#include <memory>

namespace a3d {

	class Scene;
	class VisualWorld;

	class ALFImporter {

	public:
		/// Internal Lifecycle Functions ///

		explicit ALFImporter(const std::filesystem::path& path);

		/// Internal Member Functions ///

		std::unique_ptr<Scene> 		scene(VisualWorld& visualWorld);

		/// Private Member Variables ///

		std::filesystem::path 		_path;
		//std::unique_ptr<Scene> 		_scene;
	};
}

#endif //AVARA3D_ALFIMPORTER_H
