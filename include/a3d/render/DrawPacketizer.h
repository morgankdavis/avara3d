//
//  DrawPacketizer.h
//  avara3d
//
//  Created by Morgan Davis on 12/28/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DRAWPACKETIZER_H
#define AVARA3D_DRAWPACKETIZER_H

#include <cstdint>

#include "a3d/Types.h"

namespace a3d {

	struct DrawPacket;
	struct GatherOutput;

	class DrawPacketizer {

	public:

		static DrawPacket Packetize(GatherOutput& gatherOutput,
									uint32_t vertexLayoutKey,
									const DebugOptions& debugOptions);
	};
}

#endif //AVARA3D_DRAWPACKETIZER_H
