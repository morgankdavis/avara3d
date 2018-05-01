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

#include "Types.h"


namespace ae {


	class Color;
	class Image;
	class MaterialPropertyContents;
	
	
	class MaterialProperty {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/

		MaterialProperty();
		MaterialProperty(std::shared_ptr<MaterialPropertyContents> contents);
//		MaterialProperty(std::shared_ptr<Image> image);
//		MaterialProperty(std::shared_ptr<Color> color);
//		MaterialProperty(std::shared_ptr<std::vector<std::shared_ptr<Image>>> cube);
		~MaterialProperty();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		std::shared_ptr<MaterialPropertyContents> contents() const;
		void contents(const std::shared_ptr<MaterialPropertyContents> contents);

//		std::shared_ptr<Image> image() const;
//		void image(const std::shared_ptr<Image> image);
//		
//		std::shared_ptr<Color> color() const;
//		void color(const std::shared_ptr<Color> color);
//		
//		std::shared_ptr<std::vector<std::shared_ptr<Image>>> cube() const; // +X, -X, +Y, -Y, +Z, -Z,
//		void cube(const std::shared_ptr<std::vector<std::shared_ptr<Image>>> cube);
		
		FILTER_MODE minificationFilter() const;
		void minificationFilter(FILTER_MODE mode);
		
		FILTER_MODE magnificationFilter() const;
		void magnificationFilter(FILTER_MODE mode);

		float maxAnisotropy() const;
		void maxAnisotropy(float max);
		
		WRAP_MODE wrapS() const;
		void wrapS(WRAP_MODE mode);
		
		WRAP_MODE wrapT() const;
		void wrapT(WRAP_MODE mode);
		
		WRAP_MODE wrapR() const;
		void wrapR(WRAP_MODE mode);
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		MATERIAL_PROPERTY_DIRTY_BITS dirtyBits() const;
		void dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS bits);
		
		TEXTURE_ID textureID() const;
		void textureID(TEXTURE_ID textureID);
		
		std::vector<TEXTURE_ID> replacedTextureIDs() const;
		void replacedTextureIDs(std::vector<TEXTURE_ID> textureIDs);
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/

		std::shared_ptr<MaterialPropertyContents>				m_contents;
		
//		std::shared_ptr<Image>									m_image;
//		std::shared_ptr<Color>									m_color;
//		std::shared_ptr<std::vector<std::shared_ptr<Image>>>	m_cube;
		
		FILTER_MODE												m_minificationFilter;
		FILTER_MODE												m_magnificationFilter;
		float													m_maxAnisotropy;
		WRAP_MODE												m_wrapS;
		WRAP_MODE												m_wrapT;
		WRAP_MODE												m_wrapR;

		MATERIAL_PROPERTY_DIRTY_BITS							m_dirtyBits;
		TEXTURE_ID 												m_textureID;
		std::vector<TEXTURE_ID>									m_replacedTextureIDs;
	};
}


#endif /* MaterialProperty_h */
