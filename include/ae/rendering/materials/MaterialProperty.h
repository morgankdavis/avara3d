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

#include "ae/Types.h"


namespace ae {


	class Color;
	class Image;
	class MaterialPropertyContents;
	
	
	class MaterialProperty {

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

//	public:
//
//		static std::shared_ptr<MaterialProperty> ColorProperty(std::shared_ptr<Color> color);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		MaterialProperty();
		//MaterialProperty(std::shared_ptr<Color> contents);
		MaterialProperty(std::shared_ptr<MaterialPropertyContents> contents);
		~MaterialProperty();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::shared_ptr<MaterialPropertyContents> 	contents() const;
		void										contents(const std::shared_ptr<MaterialPropertyContents> contents);

		FILTER_MODE 								minificationFilter() const;
		void 										minificationFilter(FILTER_MODE mode);
		
		FILTER_MODE 								magnificationFilter() const;
		void 										magnificationFilter(FILTER_MODE mode);

		float 										maxAnisotropy() const;
		void 										maxAnisotropy(float max);
		
		WRAP_MODE 									wrapS() const;
		void 										wrapS(WRAP_MODE mode);
		
		WRAP_MODE 									wrapT() const;
		void 										wrapT(WRAP_MODE mode);
		
		WRAP_MODE 									wrapR() const;
		void 										wrapR(WRAP_MODE mode);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		MATERIAL_PROPERTY_DIRTY_MASK 				dirtyMask() const;
		void 										dirtyMask(MATERIAL_PROPERTY_DIRTY_MASK mask);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<MaterialPropertyContents>	_contents;
		
		FILTER_MODE									_minificationFilter;
		FILTER_MODE									_magnificationFilter;
		float										_maxAnisotropy;
		WRAP_MODE									_wrapS;
		WRAP_MODE									_wrapT;
		WRAP_MODE									_wrapR;

		MATERIAL_PROPERTY_DIRTY_MASK				_dirtyMask;
	};
}


#endif /* MaterialProperty_h */
