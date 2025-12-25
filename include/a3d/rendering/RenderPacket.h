//
// Created by mkd on 12/24/25.
//

#ifndef AVARA3D_RENDERPACKET_H
#define AVARA3D_RENDERPACKET_H

#include <vector>

namespace a3d {

	class DrawItem;
	class Node;

	struct RenderPacket {

		std::vector<DrawItem> 	main;
		std::vector<DrawItem> 	wire;
		std::vector<Node*> 		lightNodes;
	};
}

#endif //AVARA3D_RENDERPACKET_H
