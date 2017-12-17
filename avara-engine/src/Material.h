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

#include "Types.h"


namespace ae {


	class MaterialProperty;
	class Program;


	class Material {
		
	public:
		
		/***************************************************************************************
		     MARK:   Static
		 **************************************************************************************/
		
		static std::shared_ptr<Material> DefaultMaterial();
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Material();
//		Material(const std::string imagePath);
		Material(std::shared_ptr<MaterialProperty> ambient,
				 std::shared_ptr<MaterialProperty> diffuse,
				 std::shared_ptr<MaterialProperty> specular);
		Material(std::shared_ptr<MaterialProperty> ambient,
				 std::shared_ptr<MaterialProperty> diffuse,
				 std::shared_ptr<MaterialProperty> specular,
				 std::string shaderName);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

//		std::string name() const;
//		void name(const std::string name);
		
		std::shared_ptr<MaterialProperty> ambient() const;
		void ambient(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> diffuse() const;
		void diffuse(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> specular() const;
		void specular(const std::shared_ptr<MaterialProperty> property);
		
		float specularExponent() const;
		void specularExponent(const float exponent);
		
		bool locksAmbientWithDiffuse() const;
		void locksAmbientWithDiffuse(const bool flag);
		
		bool doubleSided() const;
		void doubleSided(const bool flag);
		
		MaterialFillMode fillMode() const;
		void fillMode(const MaterialFillMode mode);
		
		std::shared_ptr<Program> program() const;
		void program(const std::shared_ptr<Program> program);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		void loadShaderProgram(const std::string& shaderName);
		void prepareToRender() const;
		
	protected:
		
		/***************************************************************************************
		     MARK:   Protected
		 **************************************************************************************/
		
		std::shared_ptr<Program>			m_program;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
//		std::string							m_name;
		
		std::shared_ptr<MaterialProperty>	m_ambient;
		std::shared_ptr<MaterialProperty> 	m_diffuse;
		std::shared_ptr<MaterialProperty> 	m_specular;

		float 								m_specularExponent;
		bool 								m_locksAmbientWithDiffuse;
		bool 								m_doubleSided;
		MaterialFillMode 					m_fillMode;
		
	};
}


#endif /* Material_h */
