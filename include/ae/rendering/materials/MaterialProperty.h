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

		FilterMode 									minificationFilter() const;
		void 										minificationFilter(FilterMode mode);
		
		FilterMode 									magnificationFilter() const;
		void 										magnificationFilter(FilterMode mode);

		float 										maxAnisotropy() const;
		void 										maxAnisotropy(float max);
		
		WrapMode 									wrapS() const;
		void 										wrapS(WrapMode mode);
		
		WrapMode 									wrapT() const;
		void 										wrapT(WrapMode mode);
		
		WrapMode 									wrapR() const;
		void 										wrapR(WrapMode mode);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		MaterialPropertyDirtyMask 					dirtyMask() const;
		void 										dirtyMask(MaterialPropertyDirtyMask mask);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<MaterialPropertyContents>	_contents;
		
		FilterMode									_minificationFilter;
		FilterMode									_magnificationFilter;
		float										_maxAnisotropy;
		WrapMode									_wrapS;
		WrapMode									_wrapT;
		WrapMode									_wrapR;

		MaterialPropertyDirtyMask					_dirtyMask;
	};
}


#endif /* MaterialProperty_h */
