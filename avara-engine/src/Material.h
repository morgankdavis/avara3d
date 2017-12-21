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

#include <boost/optional.hpp>

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
		Material(std::shared_ptr<MaterialProperty> ambient,
				 std::shared_ptr<MaterialProperty> diffuse,
				 std::shared_ptr<MaterialProperty> specular);
		Material(std::shared_ptr<MaterialProperty> ambient,
				 std::shared_ptr<MaterialProperty> diffuse,
				 std::shared_ptr<MaterialProperty> specular,
				 std::string programName);
		Material(std::shared_ptr<MaterialProperty> emissive);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		boost::optional<std::string> name() const;
		void name(const std::string& name);
		
		std::shared_ptr<MaterialProperty> ambient() const;
		void ambient(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> diffuse() const;
		void diffuse(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> specular() const;
		void specular(const std::shared_ptr<MaterialProperty> property);
		
		std::shared_ptr<MaterialProperty> emissive() const;
		void emissive(const std::shared_ptr<MaterialProperty> property);
		
		float specularExponent() const;
		void specularExponent(float exponent);
		
		bool locksAmbientWithDiffuse() const;
		void locksAmbientWithDiffuse(bool flag);
		
		bool doubleSided() const;
		void doubleSided(bool flag);
		
		FillMode fillMode() const;
		void fillMode(FillMode mode);
		
		std::shared_ptr<Program> program() const;
		void program(const std::shared_ptr<Program> program); // remove this?
		
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
		
		boost::optional<std::string>		m_name;
		
		std::shared_ptr<MaterialProperty>	m_ambient;
		std::shared_ptr<MaterialProperty> 	m_diffuse;
		std::shared_ptr<MaterialProperty> 	m_specular;
		std::shared_ptr<MaterialProperty> 	m_emissive;

		float 								m_specularExponent;
		bool 								m_locksAmbientWithDiffuse;
		bool 								m_doubleSided;
		FillMode 							m_fillMode;
	};
}


#endif /* Material_h */
