//
//  GLSLPreprocessor.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_GLSLPREPROCESSOR_H
#define AVARA3D_RENDER_BACKEND_OPENGL_GLSLPREPROCESSOR_H

#include <functional>
#include <optional>
#include <string>

namespace a3d {

    class GLSLPreprocessor {

    public:
        /// Internal Types ///

        using IncludeResolver = std::function<std::optional<std::string>(const std::string&)>;

        /// Internal Static Functions ///

        static std::string Process(const std::string&     source,
                                   const std::string&     sourceName,
                                   const IncludeResolver& includeResolver);
    };

}

#endif //AVARA3D_RENDER_BACKEND_OPENGL_GLSLPREPROCESSOR_H
