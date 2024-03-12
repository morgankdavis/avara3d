//
//  Renderer.h
//	avara3d
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Renderer_h
#define Renderer_h


#include <memory>
#include <set>

#include "glm/glm.hpp"

#include "a3d/Types.h"


namespace a3d {
	

	class Image;
	class Line;
	class Material;
	class Mesh;
	class MeshElement;
	class Point;
	class RenderContext;
	class Scene;
	
	
	class Renderer {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Renderer();
		Renderer(const Renderer& other) = delete; // copy constructor
		Renderer& operator=(const Renderer& other) = delete; // copy assignment
		virtual ~Renderer();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		virtual RenderingApi 				renderingApi() const = 0;
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		// TODO: make pure virtual

		virtual bool 						initialize(const RenderContext& context);
		
		virtual void 						beginFrame(const Scene& scene,
													   const RenderContext& context,
													   const DebugOptions& debugOptions,
													   Stats& stats);
		virtual void 						endFrame(const Scene& scene,
													 const RenderContext& context,
													 const DebugOptions& debugOptions,
													 Stats& stats);

		virtual void 						render(const Scene& scene,
												   const DebugOptions& debugOptions,
												   Stats& stats);
		virtual void 						render(Mesh& mesh,
												   const glm::mat4& modelMat,
												   const glm::mat4& viewMat,
												   const glm::mat4& projectionMat,
												   const DebugOptions& debugOptions,
												   Stats& stats);
		virtual void 						render(MeshElement& element,
												   Material& material,
												   const glm::mat4& modelMat,
												   const glm::mat4& viewMat,
												   const glm::mat4& projectionMat,
												   const DebugOptions& debugOptions,
												   Stats& stats);
		virtual void 						render(LineSet& lines,
												   const glm::mat4& modelMat,
												   const glm::mat4& viewMat,
												   const glm::mat4& projectionMat);
		virtual void 						render(PointSet& points,
												   const glm::mat4& modelMat,
												   const glm::mat4& viewMat,
												   const glm::mat4& projectionMat);

		virtual std::unique_ptr<Image>		snapshot(const RenderContext& context) const;
	};
}


#endif /* Renderer_h */
