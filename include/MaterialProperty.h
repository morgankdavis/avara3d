//
//  MaterialProperty.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef MaterialProperty_h
#define MaterialProperty_h


#include <memory>
#include <string>
#include <vector>

#include "Aliases.h"
#include "Types.h"


namespace ae {


	class Color;
	class Image;
	class MaterialPropertyContents;
	
	
	class MaterialProperty : public std::enable_shared_from_this<MaterialProperty> {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		MaterialProperty();
		MaterialProperty(MaterialPropertyContentsSPtr contents);
		~MaterialProperty();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		MaterialPropertyContentsSPtr 	contents() const;
		void							contents(const MaterialPropertyContentsSPtr contents);

		FILTER_MODE 					minificationFilter() const;
		void 							minificationFilter(FILTER_MODE mode);
		
		FILTER_MODE 					magnificationFilter() const;
		void 							magnificationFilter(FILTER_MODE mode);

		float 							maxAnisotropy() const;
		void 							maxAnisotropy(float max);
		
		WRAP_MODE 						wrapS() const;
		void 							wrapS(WRAP_MODE mode);
		
		WRAP_MODE 						wrapT() const;
		void 							wrapT(WRAP_MODE mode);
		
		WRAP_MODE 						wrapR() const;
		void 							wrapR(WRAP_MODE mode);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		MATERIAL_PROPERTY_DIRTY_BITS 	dirtyBits() const;
		void 							dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS bits);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		MaterialPropertyContentsSPtr	_contents;
		
		FILTER_MODE						_minificationFilter;
		FILTER_MODE						_magnificationFilter;
		float							_maxAnisotropy;
		WRAP_MODE						_wrapS;
		WRAP_MODE						_wrapT;
		WRAP_MODE						_wrapR;

		MATERIAL_PROPERTY_DIRTY_BITS	_dirtyBits;
	};
}


#endif /* MaterialProperty_h */
