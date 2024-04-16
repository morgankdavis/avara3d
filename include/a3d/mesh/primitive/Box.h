//
//  Box.h
//  avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BOX_H
#define AVARA3D_BOX_H


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Box: public MeshElement {

	private:

		static constexpr int DEFAULT_SEGMENTS = 8;

	public:

		static std::shared_ptr<Mesh> Mesh(float length,
										  float width,
										  float height,
										  int lengthSegments = DEFAULT_SEGMENTS,
										  int widthSegments = DEFAULT_SEGMENTS,
										  int heightSegments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		// z, x, y?
		Box(float length,
			float width,
			float height,
			int lengthSegments = DEFAULT_SEGMENTS,
			int widthSegments = DEFAULT_SEGMENTS,
			int heightSegments = DEFAULT_SEGMENTS);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 		length() const;
		float 		width() const;
		float 		height() const;
		int 		lengthSegments() const;
		int 		widthSegments() const;
		int 		heightSegments() const;

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		float		_length;
		float		_width;
		float		_height;
		int 		_lengthSegments;
		int 		_widthSegments;
		int 		_heightSegments;
	};
}


#endif /* AVARA3D_BOX_H */
