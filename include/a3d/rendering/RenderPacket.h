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

		// future: mainOpaque, mainMask, mainTransparent
		std::vector<DrawItem> 	main;
		std::vector<DrawItem> 	wireframe;
		std::vector<Node*> 		lightNodes;
		std::vector<Line> 		debugLines;
	};
}

#endif //AVARA3D_RENDERPACKET_H
