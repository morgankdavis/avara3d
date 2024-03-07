//
// Created by mkd on 3/6/24.
//

#ifndef AVARA3D_ROUNDEDBOX_H
#define AVARA3D_ROUNDEDBOX_H


#include <memory>


#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class RoundedBox: public MeshElement {

	private:

		static constexpr int DEFAULT_SLICES = 8;
		static constexpr int DEFAULT_SEGMENTS = 8;

	public:

		static std::shared_ptr<Mesh> Mesh(float radius,
										  float length,
										  float width,
										  float height,
										  int slices = DEFAULT_SLICES,
										  int lengthSegments = DEFAULT_SEGMENTS,
										  int widthSegments = DEFAULT_SEGMENTS,
										  int heightSegments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		RoundedBox(float radius,
				   float length,
				   float width,
				   float height,
				   int slices = DEFAULT_SLICES,
				   int lengthSegments = DEFAULT_SEGMENTS,
				   int widthSegments = DEFAULT_SEGMENTS,
				   int heightSegments = DEFAULT_SEGMENTS);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 		radius() const;
		float 		length() const;
		float 		width() const;
		float 		height() const;
		int 		slices() const;
		int 		lengthSegments() const;
		int 		widthSegments() const;
		int 		heightSegments() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float 		_radius;
		float		_length;
		float		_width;
		float		_height;
		int 		_slices;
		int 		_lengthSegments;
		int 		_widthSegments;
		int 		_heightSegments;
	};
}


#endif //AVARA3D_ROUNDEDBOX_H
