//
// Created by mkd on 12/14/25.
//

#ifndef AVARA3D_RENDERITEM_H
#define AVARA3D_RENDERITEM_H

#include "a3d/Math.h"

namespace a3d {

	class Light;
	class Material;
	class MeshElement;

	struct RenderItem {
		MeshElement*	element;
		Material*		material;
		math::mat4	 	model;
		Node*			dbg_node;
	};

//	struct LightNode {
//		Light*		light;
//		math::mat4 	model;
//	};

//	struct MeshItem {
//		Mesh*		mesh;
//		math::mat4 	model;
//	};
//
//	struct LightItem {
//		Light*		light;
//		math::mat4 	model;
//	};
}

#endif //AVARA3D_RENDERITEM_H
