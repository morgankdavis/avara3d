//
//  ALFImporter.h
//  avara3d
//
//  Created by Morgan Davis on 3/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXTENSIONS_ALFIMPORTER_H
#define AVARA3D_EXTENSIONS_ALFIMPORTER_H

#include <filesystem>
#include <memory>

namespace a3d {

    class Scene;
    class VisualWorld;

}

namespace a3d::ext {

    class ALFImporter {

    public:
        /// Internal Lifecycle Functions ///

        explicit ALFImporter(const std::filesystem::path& path);

        /// Internal Member Functions ///

        std::unique_ptr<Scene> scene(VisualWorld& visualWorld);

        /// Private Member Variables ///

        std::filesystem::path  _path;
    };

}

#endif //AVARA3D_EXTENSIONS_ALFIMPORTER_H
