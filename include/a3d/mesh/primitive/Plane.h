//
//  Plane.h
//	avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PLANE_H
#define AVARA3D_PLANE_H


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;

	
	class Plane: public MeshElement {

	private:

		static constexpr int DEFAULT_SEGMENTS = 8;

	public:

		static std::shared_ptr<Mesh> Mesh(float width,
										  float height,
										  int widthSegements = DEFAULT_SEGMENTS,
										  int heightSegments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Plane(float width,
			  float height,
			  int widthSegements = DEFAULT_SEGMENTS,
			  int heightSegments = DEFAULT_SEGMENTS);
		
/*********************************************************************************************
 	Public
 *********************************************************************************************/
		
		float 		width() const;
		float 		height() const;
		int			widthSegements() const;
		int			heightSegments() const;
		
/*********************************************************************************************
 	Private
 *********************************************************************************************/

	private:

		float		_width;
		float		_height;
		int			_widthSegements;
		int			_heightSegments;
	};
}


#endif /* AVARA3D_PLANE_H */
