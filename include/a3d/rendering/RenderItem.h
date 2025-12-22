//
// Created by mkd on 12/14/25.
//

#ifndef AVARA3D_RENDERITEM_H
#define AVARA3D_RENDERITEM_H

#include "a3d/Math.h"

namespace a3d {

	class Light;
	class Material;
	class Mesh;
	class MeshElement;

	struct RenderItem {

		a3d::Mesh* 			mesh = nullptr;
		uint32_t 			elementIndex = 0;
		a3d::MeshElement* 	element = nullptr;
		a3d::Material* 		material = nullptr;

		a3d::math::mat4 	model;
		a3d::AABB 			aabb; // world space

		float 				depth = 0.0f;        // view-space depth for later
		bool 				transparent = false;  // for later -- always false in BlendFunction
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
