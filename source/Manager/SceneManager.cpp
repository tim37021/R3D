#include <r3d/Manager/SceneManager.hpp>
#include <r3d/Utils/tiny_obj_loader.h>
#include <r3d/Core/Core.hpp>
#include <r3d/Core/Vertex.hpp>
#include <r3d/Core/AttribPointer.hpp>
#include <r3d/Scene/SceneNode.hpp>
#include <glm/glm.hpp>
#include <r3d/Material/Material.hpp>
#include "../Scene/MeshSceneNode.hpp"
#include "../Scene/EmptySceneNode.hpp"

static const char *vertex_shader=
	"#version 330\n"
	"layout(location=0) in vec3 pos;\n"
	"layout(location=1) in vec2 texCoord;\n"
	"layout(location=2) in vec3 norm;\n"
	"uniform mat4 mvp;"
	"uniform mat4 model;"
	"out vec3 vNorm;"
	"out vec3 worldPos;"
	"out vec2 vTexCoord;"
	"void main(){\n"
	"worldPos=vec3(model*vec4(pos, 1.0));"
	"vTexCoord=texCoord;"
	"vNorm=norm;"
	"gl_Position=mvp*vec4(pos, 1.0);"
	"}\n";

static const char *fragment_shader=
	"#version 330\n"
	"layout(location = 0) out vec3 worldPosMap;"
	"layout(location = 1) out vec3 diffuseMap;\n"
	"layout(location = 2) out vec3 normalMap;\n"
	"layout(location = 3) out vec3 specularMap;\n"
	"uniform vec3 diffuse;\n"
	"uniform vec3 specular;\n"
	"uniform vec3 emission;"
	"uniform sampler2D diffuseTexture;\n"
	"in vec2 vTexCoord;\n"
	"in vec3 worldPos;\n"
	"in vec3 vNorm;\n"
	"void main(){\n"
	"worldPosMap=worldPos;\n"
	"diffuseMap=vec3(texture(diffuseTexture, vTexCoord))*diffuse;\n"
	"normalMap=vNorm;\n"
	"specularMap=specular;\n"
	"if(length(emission)>0.2){ specularMap=vec3(-1, -1, -1); }"
	"}\n";

static r3d::ProgramPtr MakeShaderProgram(const r3d::Engine *engine, const char *vsource,
	const char *fsource)
{
	auto program=engine->newProgram();
	auto vs=engine->newShader(r3d::ST_VERTEX_SHADER);
	auto fs=engine->newShader(r3d::ST_FRAGMENT_SHADER);
	vs->source(vsource);
	fs->source(fsource);
	vs->compile();
	fs->compile();
	program->attachShader(vs);
	program->attachShader(fs);
	program->link();

	return program;
}

namespace r3d
{
	SceneManager::SceneManager(Engine *engine)
		: m_engine(engine), m_rootNode(SceneNodePtr(new EmptySceneNode(SceneNodePtr(), engine->getCurrentContext())))
	{
		m_program=MakeShaderProgram(m_engine, vertex_shader, fragment_shader);
	}

	void SceneManager::drawAll()
	{
		if(m_camera)
			m_rootNode->render(m_engine->getRenderer(), m_camera.get());
	}

	SceneNode *SceneManager::loadObjScene(SceneNodePtr node, const char *filename, const char *base)
	{
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err = tinyobj::LoadObj(shapes, materials, filename, base);

		//TODO: Context management
		auto cw=m_engine->getCurrentContext();
		auto tMgr=cw->getTextureManager();
		auto objNode=SceneNodePtr(new EmptySceneNode(node, cw));
		objNode->setName(filename);
		node->addChild(objNode);
		for(auto &shape: shapes)
		{
			
			std::vector<Vertex> vertices;
			for(int v=0; v<shape.mesh.positions.size()/3; v++)
			{

				vertices.push_back({glm::vec3(shape.mesh.positions[v*3], shape.mesh.positions[v*3+1], shape.mesh.positions[v*3+2]),
								shape.mesh.texcoords.size()>0?glm::vec2(shape.mesh.texcoords.at(v*2), 1.0f-shape.mesh.texcoords.at(v*2+1)): glm::vec2(),
								glm::vec3(shape.mesh.normals.at(v*3), shape.mesh.normals.at(v*3+1), shape.mesh.normals.at(v*3+2))});
			}
			MeshSceneNode *newNode = new MeshSceneNode(objNode, cw, vertices, shape.mesh.indices, shape.name.c_str());
			
			auto m_defaultMaterial = std::make_shared<Material>(m_program);

			int32_t mid=-1;
			for(int i=0; mid==-1&&i<shape.mesh.material_ids.size(); i++)
				mid=shape.mesh.material_ids[i]>=0? shape.mesh.material_ids[i]: mid;

			m_defaultMaterial->setDiffuse(tMgr->registerColorTexture2D("white.png"));
			if(mid>=0)
			{
				auto material=materials[mid];
				if(material.diffuse_texname!="")
				{
					auto tex=tMgr->registerColorTexture2D(material.diffuse_texname);
					if(tex) m_defaultMaterial->setDiffuse(tex);
				}

				m_defaultMaterial->setDiffuse(glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]));
				m_defaultMaterial->setSpecular(glm::vec3(material.specular[0], material.specular[1], material.specular[2]));
			}
			newNode->setMaterial(m_defaultMaterial);

			objNode->addChild(SceneNodePtr(newNode));
		}
		return objNode.get();
	}
}