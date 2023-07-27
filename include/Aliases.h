//
// Created by mkd on 7/27/23.
//

#ifndef AVARA_ENGINE_ALIASES_H
#define AVARA_ENGINE_ALIASES_H


#include <memory>


namespace ae {

	class GeometryElement;

	class Material;


	using GeometryElementUPtr = std::unique_ptr<ae::GeometryElement>;
	using GeometryElementSPtr = std::shared_ptr<ae::GeometryElement>;

	using MaterialUPtr = std::unique_ptr<Material>;
	using MaterialSPtr = std::shared_ptr<Material>;

}


#endif //AVARA_ENGINE_ALIASES_H
