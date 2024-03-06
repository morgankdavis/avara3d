//
//  Box.h
//	avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Box_h
#define Box_h

#include <memory>


#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Box: public MeshElement {

	public:

		static std::shared_ptr<Mesh> Mesh(float length,
										  float width,
										  float height,
										  int lengthSegments = 1,
										  int widthSegments = 1,
										  int heightSegments = 1,
										  const std::shared_ptr<Material> material
										          = std::make_shared<Material>());

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		// z, x, y?
		Box(float length,
			float width,
			float height,
			int lengthSegments = 1,
			int widthSegments = 1,
			int heightSegments = 1);
		
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
		int 		_heightSegments ;
	};
}


#endif /* Box_h */
