//
// Created by mkd on 12/24/25.
//

#ifndef AVARA3D_RENDERPACKET_H
#define AVARA3D_RENDERPACKET_H

#include <vector>

#include "a3d/mesh/Line.h"
#include "a3d/rendering/DrawItem.h"

namespace a3d {

	class DrawItem;
	class Node;

	struct RenderPacket {

		BackgroundPass			backgroundPass;
		// future: mainOpaque, mainMask, mainTransparent
		std::vector<DrawItem> 	mainPassItems;
		std::vector<DrawItem> 	wireframePassItems;
		//std::vector<Line> 		debugLines;
		LinesPass				linesPass;
		std::vector<Node*> 		lightNodes;
	};
}

#endif //AVARA3D_RENDERPACKET_H
