//
// Created by mkd on 3/6/24.
//

#ifndef AVARA3D_SPRING_H
#define AVARA3D_SPRING_H


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Spring: public MeshElement {

	private:

		static constexpr int DEFAULT_SLICES = 8;
		static constexpr int DEFAULT_SEGMENTS = 32;

	public:

		static std::unique_ptr<Mesh> Mesh(float minorRadius,
										  float majorRadius,
										  float length,
										  int slices = DEFAULT_SLICES,
										  int segments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Spring(float minorRadius,
			   float majorRadius,
			   float length,
			   int slices = DEFAULT_SLICES,
			   int segments = DEFAULT_SEGMENTS);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 	minorRadius() const;
		float 	majorRadius() const;
		float 	length() const;
		int 	slices() const;
		int 	segments() const;

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	private:

		float 	_minorRadius;
		float 	_majorRadius;
		float 	_length;
		int 	_slices;
		int 	_segments;
	};
}


#endif //AVARA3D_SPRING_H
