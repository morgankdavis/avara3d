//
//  Snapshot.h
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_SNAPSHOT_H
#define AVARA3D_UTIL_SNAPSHOT_H

namespace a3d {
class RenderContext;
} // namespace a3d

namespace a3d::util::snapshot {
    // [Public Functions]

    /** @brief Attempts to save a timestamped PNG snapshot of @p context beside the current executable. */
void SaveSnapshot(RenderContext& context);
} // namespace a3d::util::snapshot

#endif // AVARA3D_UTIL_SNAPSHOT_H
