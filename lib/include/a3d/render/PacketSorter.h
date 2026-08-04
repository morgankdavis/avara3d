//
//  PacketSorter.h
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_PACKETSORTER_H
#define AVARA3D_RENDER_PACKETSORTER_H

#include <cstdint>

namespace a3d {

    struct DrawItem;
    struct DrawPacket;

    class PacketSorter {

    public:
        /// Internal Static Member Functions ///

        static uint64_t MakeSortKey(const DrawItem& item);
        static void     SortPacket(DrawPacket& packet);
    };

}

#endif //AVARA3D_RENDER_PACKETSORTER_H
