//
//  DrawPacketizer.h
//  avara3d
//
//  Created by Morgan Davis on 12/28/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_DRAWPACKETIZER_H
#define AVARA3D_RENDER_DRAWPACKETIZER_H

namespace a3d {
struct DrawPacket;
struct GatherOutput;

class DrawPacketizer {

public:
    // [Internal Static Member Functions]

    static DrawPacket Packetize(GatherOutput& gatherOutput);
};
} // namespace a3d

#endif // AVARA3D_RENDER_DRAWPACKETIZER_H
