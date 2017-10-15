//
//  Material.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Material_h
#define Material_h


#include <memory>
#include <string>


namespace ae {


	class MaterialProperty;


	typedef enum {
		MaterialFillModeFill,
		MaterialFillModeLine
	} MaterialFillMode;


	class Material {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Material(const std::string name);
		Material(const std::string name,
				 std::shared_ptr<MaterialProperty> ambient,
				 std::shared_ptr<MaterialProperty> diffuse,
				 std::shared_ptr<MaterialProperty> specular);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		std::string name() const;
		void name(const std::string name);
		
		std::shared_ptr<MaterialProperty> ambient() const;
		void ambient(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> diffuse() const;
		void diffuse(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> specular() const;
		void specular(const std::shared_ptr<MaterialProperty> property);
		
		float specularExponent() const;
		void specularExponent(const float exponent);
		
		bool litPerPixel() const;
		void litPerPixel(const bool flag);
		
		bool lockAmbientWithDiffuse() const;
		void lockAmbientWithDiffuse(const bool flag);
		
		bool doubleSided() const;
		void doubleSided(const bool flag);
		
		bool readFromDepthBuffer() const;
		void readFromDepthBuffer(const bool flag);
		
		MaterialFillMode fillMode() const;
		void fillMode(const MaterialFillMode mode);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		std::string							m_name;
		
		std::shared_ptr<MaterialProperty>	m_ambient;
		std::shared_ptr<MaterialProperty> 	m_diffuse;
		std::shared_ptr<MaterialProperty> 	m_specular;

		float 								m_specularExponent;
		bool 								m_litPerPixel;
		bool 								m_lockAmbientWithDiffuse;
		bool 								m_doubleSided;
		bool 								m_readFromDepthBuffer;
		MaterialFillMode 					m_fillMode;
		
	};
}


#endif /* Material_h */
