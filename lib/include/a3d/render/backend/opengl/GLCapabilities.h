//
//  GLCapabilities.h
//  avara3d
//
//  Created by Morgan Davis on 8/12/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_GLCAPABILITIES_H
#define AVARA3D_RENDER_BACKEND_OPENGL_GLCAPABILITIES_H

namespace a3d {

    struct GLCapabilities {
        bool polygonMode {false};
        bool drawTimer {false};
    };

}

#endif // AVARA3D_RENDER_BACKEND_OPENGL_GLCAPABILITIES_H
