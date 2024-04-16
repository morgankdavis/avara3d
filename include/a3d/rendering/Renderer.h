//
//  Renderer.h
//	avara3d
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDERER_H
#define AVARA3D_RENDERER_H


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
 *************************************{}********************************************************/

	public:

		Renderer();
		Renderer(const Renderer& other) = delete; // copy constructor
		Renderer& operator=(const Renderer& other) = delete; // copy assignment
		virtual ~Renderer() = 0;

/*********************************************************************************************
	Public
 *********************************************************************************************/

		virtual RenderingApi 				renderingApi() const = 0;
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		virtual bool 						initialize(const RenderContext& context) = 0;
		
		virtual void 						beginFrame(const Scene& scene,
													   const RenderContext& context,
													   const DebugOptions& debugOptions,
													   Stats& stats) = 0;
		virtual void 						endFrame(const Scene& scene,
													 const RenderContext& context,
													 const DebugOptions& debugOptions,
													 Stats& stats) = 0;

		virtual void 						render(const Scene& scene,
												   const DebugOptions& debugOptions,
												   Stats& stats) = 0;
		virtual void 						render(Mesh& mesh,
												   const glm::mat4& modelMat,
												   const glm::mat4& viewMat,
												   const glm::mat4& projectionMat,
												   const DebugOptions& debugOptions,
												   Stats& stats) = 0;
		virtual void 						render(MeshElement& element,
												   Material& material,
												   const glm::mat4& modelMat,
												   const glm::mat4& viewMat,
												   const glm::mat4& projectionMat,
												   const DebugOptions& debugOptions,
												   Stats& stats) = 0;
		virtual void 						render(const std::vector<Line>& lines,
												   const glm::mat4& modelMat,
												   const glm::mat4& viewMat,
												   const glm::mat4& projectionMat) = 0;

		virtual std::unique_ptr<Image>		snapshot(const RenderContext& context) const = 0;
	};
}


#endif /* AVARA3D_RENDERER_H */
