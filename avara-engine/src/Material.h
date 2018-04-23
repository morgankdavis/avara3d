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
		     Static
		 ***************************************************************************************/
		
		static std::shared_ptr<Material> DefaultMaterial();
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Material();
		Material(std::shared_ptr<Program> program);
		Material(std::shared_ptr<MaterialProperty> ambient,
				 std::shared_ptr<MaterialProperty> diffuse,
				 std::shared_ptr<MaterialProperty> specular);
		Material(std::shared_ptr<MaterialProperty> ambient,
				 std::shared_ptr<MaterialProperty> diffuse,
				 std::shared_ptr<MaterialProperty> specular,
				 std::shared_ptr<Program> program);
		Material(std::shared_ptr<MaterialProperty> emissive);
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/

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
		
		FILL_MODE fillMode() const;
		void fillMode(FILL_MODE mode);
		
		float uvScale() const;
		void uvScale(float scale);
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		std::shared_ptr<Program> program() const;
		void program(std::shared_ptr<Program> program);
		
		//void loadShaderProgram(const std::string& shaderName);
		std::shared_ptr<Program> selectProgram(DEBUG_OPTIONS debugOptions);
		void prepareToRender(DEBUG_OPTIONS debugOptions);
		
	protected:
		
		/***************************************************************************************
		     Protected
		 ***************************************************************************************/
		
		std::shared_ptr<Program>			m_program;
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		boost::optional<std::string>		m_name;
		
		std::shared_ptr<MaterialProperty>	m_ambient;
		std::shared_ptr<MaterialProperty> 	m_diffuse;
		std::shared_ptr<MaterialProperty> 	m_specular;
		std::shared_ptr<MaterialProperty> 	m_emissive;

		float 								m_specularExponent;
		bool 								m_locksAmbientWithDiffuse;
		bool 								m_doubleSided;
		FILL_MODE 							m_fillMode;
		float 								m_uvScale;
	};
}


#endif /* Material_h */
