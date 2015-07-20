#ifndef __R3D_MATERIAL_MATERIAL_HPP_
#define __R3D_MATERIAL_MATERIAL_HPP_

#include <glm/glm.hpp>
#include <r3d/Shader/Program.hpp>
#include <r3d/Core/Texture.hpp>
#include <r3d/Renderer/Renderer.hpp>

namespace r3d
{
	class Material
	{
	public:
		Material(ProgramPtr program): 
			m_ambient(0.05f), m_diffuse(1.0f), m_specular(1.0f),
			m_emission(0.0f), m_dMap(nullptr), m_sMap(nullptr), m_nMap(nullptr),
			m_bMap(nullptr), normalMapIntensity(1.0f), parallaxMapIntensity(1.0f), 
			m_wireframeView(false), m_program(program)
		{
			m_program->setUniform("diffuseTexture", 0);
			m_program->setUniform("specularTexture", 1);
			m_program->setUniform("normalTexture", 2);
			m_program->setUniform("heightTexture", 3);
		}

		void setAmbient(const glm::vec3 &v)
		{ m_ambient=v; }

		void setDiffuse(const glm::vec3 &v)
		{ m_diffuse=v; }

		void setDiffuse(ColorTexture2D *text) 
		{m_dMap=text;}

		void setSpecular(ColorTexture2D *text) 
		{m_sMap=text;}

		void setSpecular(const glm::vec3 &v)
		{ m_specular=v; }

		void setEmission(const glm::vec3 &v)
		{ m_emission=v; }

		void setNormalMap(ColorTexture2D *text)
		{ m_nMap=text; }

		void setHeightMap(ColorTexture2D *text)
		{ m_bMap=text; }

		void enableWireframeView(bool value)
		{ m_wireframeView=value; }

		glm::vec3 getDiffuse() const { return m_diffuse; }
		glm::vec3 getSpecular() const { return m_specular; }

		void prepareShader() const
		{
			m_program->setUniform("ambient", m_ambient);
			
			m_program->setUniform("specular", m_specular);

			m_program->setUniform("emission", m_emission);
			m_program->setUniform("wireframeView", (int)m_wireframeView);

			if(m_dMap)
				m_dMap->bind(0);

			if(m_sMap)
			{
				m_sMap->bind(1);
				m_program->setUniform("specular", {-1, -1, -1});
			}

			if(m_nMap)
			{
				m_nMap->bind(2);
				m_program->setUniform("normalMapIntensity", normalMapIntensity);

				if(m_bMap)
				{
					m_bMap->bind(3);
					m_program->setUniform("parallaxMapIntensity", parallaxMapIntensity);
				}
			}else
				m_program->setUniform("normalMapIntensity", 0.0f);

			m_program->setUniform("diffuse", m_diffuse);
		}

		ProgramPtr getProgram() 
		{ return m_program; }
	private:
		glm::vec3 m_ambient;
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;
		glm::vec3 m_emission;
		glm::vec3 m_texture;

		ColorTexture2D *m_dMap;
		ColorTexture2D *m_sMap;
		ColorTexture2D *m_nMap;
		ColorTexture2D *m_bMap;

		float shininess;
		float normalMapIntensity;
		float parallaxMapIntensity;

		bool m_wireframeView;

		ProgramPtr m_program;
	};

	typedef std::shared_ptr<Material> MaterialPtr;
};

#endif