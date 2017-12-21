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

#include <GL/glew.h>

#include "Types.h"


namespace ae {


	class Color;
	class Image;
	class Program;
	
	
	class MaterialProperty {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/

		MaterialProperty(std::shared_ptr<Image> image);
		MaterialProperty(std::shared_ptr<Color> color);
		MaterialProperty(std::shared_ptr<std::vector<std::shared_ptr<Image>>> cube);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::shared_ptr<Image> image() const;
		void image(const std::shared_ptr<Image> image);
		
		std::shared_ptr<Color> color() const;
		void color(const std::shared_ptr<Color> color);
		
		std::shared_ptr<std::vector<std::shared_ptr<Image>>> cube() const; // +X, -X, +Y, -Y, +Z, -Z,
		void cube(const std::shared_ptr<std::vector<std::shared_ptr<Image>>> cube);
		
		FilterMode minificationFilter() const;
		void minificationFilter(FilterMode mode);
		
		FilterMode magnificationFilter() const;
		void magnificationFilter(FilterMode mode);

		float maxAnisotropy() const;
		void maxAnisotropy(float max);
		
		WrapMode wrapS() const;
		void wrapS(WrapMode mode);
		
		WrapMode wrapT() const;
		void wrapT(WrapMode mode);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		void loadTexture();
		void bind(MaterialPropertyType type, Program& program);
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		std::shared_ptr<Image>									m_image;
		std::shared_ptr<Color>									m_color;
		std::shared_ptr<std::vector<std::shared_ptr<Image>>>	m_cube;
		
		WrapMode												m_wrapS;
		WrapMode												m_wrapT;
		FilterMode												m_minificationFilter;
		FilterMode												m_magnificationFilter;
		FilterMode												m_mipFilter;
		float													m_maxAnisotropy;
		
		GLuint													m_glTextureID;
	};
}


#endif /* MaterialProperty_h */
