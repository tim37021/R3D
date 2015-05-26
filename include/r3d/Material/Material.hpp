#ifndef __R3D_MATERIAL_MATERIAL_HPP_
#define __R3D_MATERIAL_MATERIAL_HPP_

#include <glm/glm.hpp>
#include <r3d/Shader/Program.hpp>
#include <r3d/Core/Texture.hpp>

namespace r3d
{
	class Material
	{
	public:
		Material(ProgramPtr program): 
			m_ambient(0.05f), m_diffuse(1.0f), m_specular(1.0f), m_program(program),
			m_dMap(nullptr)
		{}

		void setAmbient(const glm::vec3 &v)
		{ m_ambient=v; }

		void setDiffuse(const glm::vec3 &v)
		{ m_diffuse=v; }

		void setDiffuse(ColorTexture2D *text) 
		{m_dMap=text;}

		void setSpecular(const glm::vec3 &v)
		{ m_specular=v; }

		glm::vec3 getDiffuse() const { return m_diffuse; }

		void prepareShader() const
		{
			m_program->setUniform("ambient", m_ambient);
			
			m_program->setUniform("specular", m_specular);

			if(m_dMap)
			{
				m_dMap->bind(0);
				m_program->setUniform("diffuseTexture", 0);
			}
			m_program->setUniform("diffuse", m_diffuse);
		}

		ProgramPtr getProgram() 
		{ return m_program; }
	private:
		glm::vec3 m_ambient;
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;
		glm::vec3 m_texture;

		ColorTexture2D *m_dMap;

		float shininess;

		ProgramPtr m_program;
	};

	typedef std::shared_ptr<Material> MaterialPtr;
};

#endif