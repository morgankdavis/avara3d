//
//  Snapshot.h
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_SNAPSHOT_H
#define AVARA3D_UTIL_SNAPSHOT_H

#include "a3d/Math.h"

namespace a3d {

    class RenderContext;

}

namespace a3d::util::snapshot {

    void SaveSnapshot(RenderContext& context);

}

#endif //AVARA3D_UTIL_SNAPSHOT_H
