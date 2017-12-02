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

#include <GL/glew.h>

#include "Types.h"


namespace ae {


	class Color;
	class Image;
	
	
	class MaterialProperty {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/

		MaterialProperty(const std::shared_ptr<Image> image);
		MaterialProperty(const std::shared_ptr<Color> color);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::shared_ptr<Image> image();
		void image(const std::shared_ptr<Image> image);
		
		std::shared_ptr<Color> color();
		void color(const std::shared_ptr<Color> color);
		
		WrapMode wrapS() const;
		void wrapS(const WrapMode mode);
		
		WrapMode wrapT() const;
		void wrapT(const WrapMode mode);
		
		FilterMode minificationFilter() const;
		void minificationFilter(const FilterMode mode);
		
		FilterMode mipFilter() const;
		void mipFilter(const FilterMode mode);
		
		float maxAnisotropy() const;
		void maxAnisotropy(const float max);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		void load();
		void bind(MaterialPropertyType type, GLuint programID);
		int glTextureID();
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		std::shared_ptr<Image>		m_image;
		std::shared_ptr<Color>		m_color;
		
		WrapMode					m_wrapS;
		WrapMode					m_wrapT;
		FilterMode					m_minificationFilter;
		FilterMode					m_magnificationFilter;
		FilterMode					m_mipFilter;
		float						m_maxAnisotropy;
		
		
		int							m_glTextureID;
	};
}


#endif /* MaterialProperty_h */
