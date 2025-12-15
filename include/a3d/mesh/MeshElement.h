//
//  MeshElement.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESHELEMENT_H
#define AVARA3D_MESHELEMENT_H

#include <memory>
#include <vector>

#include "a3d/Types.h"
#include "a3d/Math.h"

namespace a3d {

	class Line;
	class Material;
	class Node;
	class Program;
	class Renderer;
	class RenderContext;
	class RenderItem;

	class MeshElement {

	public:
		/// Public Lifecycle Functions ///

		MeshElement(const std::vector<Vertex>& verticies,
					const std::vector<Face>& faces);
		virtual ~MeshElement();

		/// Internal Member Functions ///

		void 							gather(std::vector<RenderItem>& items,
											   Material& material,
											   math::mat4& model,
											   FrameStats& stats);

		void 							draw(Renderer& renderer,
											 const RenderContext& context,
											 Material& material,
											 const math::mat4& modelMat,
											 const math::mat4& viewMat,
											 const math::mat4& projectionMat,
											 const DebugOptions& debugOptions,
											 FrameStats& stats);

		void 							burnTransform(const math::mat4& transform,
													  bool normals);

		const std::vector<Vertex>& 		vertices() const;
		const std::vector<Face>&		faces() const;

		AABB							aabb(const Node* convertTo = nullptr) const;
		math::vec3 						extent(const Node* convertTo = nullptr) const;

		const std::vector<Line>&				aabbLines();

		MeshElementDirtyMask 			dirtyMask() const;
		void 							dirtyMask(MeshElementDirtyMask mask);

	protected:
		/// Protected Lifecycle ///

		MeshElement();

		/// Protected Member Variables ///

		std::vector<Vertex>				_vertices;
		std::vector<Face>				_faces;
		std::vector<Line>				_aabbLines;
		MeshElementDirtyMask			_dirtyMask;
	};
}

#endif /* AVARA3D_MESHELEMENT_H */
