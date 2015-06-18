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
	"out vec3 vWorldPos;"
	"out vec2 vTexCoord;"
	"void main(){\n"
	"vWorldPos=(model*vec4(pos, 1.0)).xyz;"
	"vTexCoord=texCoord;"
	"vNorm=norm;"
	"gl_Position=mvp*vec4(pos, 1.0);"
	"}\n";

static const char *geometry_shader=
	"#version 330\n"
	"layout(triangles) in;\n"
	"layout(triangle_strip, max_vertices=3) out;\n"
	"in vec3 vNorm[3];\n"
	"in vec3 vWorldPos[3];\n"
	"in vec2 vTexCoord[3];\n"
	"out vec3 gNorm;\n"
	"out vec3 gWorldPos;\n"
	"out vec2 gTexCoord;\n"
	"uniform int enableSmooth=1;"
	"uniform mat4 nmat;"
	"void main(){\n"
	"vec3 oa=vWorldPos[1]-vWorldPos[0];\n"
	"vec3 ob=vWorldPos[2]-vWorldPos[0];\n"
	"vec3 norm=normalize(cross(oa, ob));\n"
	"for(int i=0; i<3; i++){\n"
	"gl_Position=gl_in[i].gl_Position;\n"
	"gNorm=(enableSmooth==1? (nmat*vec4(vNorm[i], 1.0)).xyz: norm);\n"
	"gWorldPos=vWorldPos[i];\n"
	"gTexCoord=vTexCoord[i];\n"
	"EmitVertex();\n"
	"}"
	"EndPrimitive();\n"
	"}\n";

static const char *fragment_shader=
	"#version 330\n"
	"layout(location = 0) out vec3 worldPosMap;"
	"layout(location = 1) out vec3 diffuseMap;\n"
	"layout(location = 2) out vec3 normalMap;\n"
	"layout(location = 3) out vec3 specularMap;\n"
	"layout(location = 4) out uint objectMap;\n"
	"uniform vec3 diffuse;\n"
	"uniform vec3 specular;\n"
	"uniform vec3 emission;"
	"uniform sampler2D diffuseTexture;\n"
	"uniform sampler2D specularTexture;\n"
	"uniform uint id;"
	"in vec2 gTexCoord;\n"
	"in vec3 gWorldPos;\n"
	"in vec3 gNorm;\n"
	"void main(){\n"
	"worldPosMap=gWorldPos;\n"
	"diffuseMap=vec3(texture(diffuseTexture, gTexCoord))*diffuse;\n"
	"normalMap=gNorm;\n"
	"specularMap=(specular.x<0?vec3(texture(specularTexture, gTexCoord)): specular);\n"
	"objectMap=id;\n"
	"if(length(emission)>0.2){ specularMap=-emission; }"
	"}\n";

namespace r3d
{
	SceneManager::SceneManager(Engine *engine)
		: m_engine(engine), m_rootNode(SceneNodePtr(new EmptySceneNode(SceneNodePtr(), engine->getCurrentContext())))
	{
		m_program=MakeShaderProgram(m_engine, vertex_shader, geometry_shader, fragment_shader);
	}

	SceneNode *SceneManager::loadObjScene(SceneNodePtr node, const char *filename, const char *base)
	{
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string basePath(filename);

		if(base)
			tinyobj::LoadObj(shapes, materials, filename, base);
		else
		{
			std::size_t end=basePath.find_last_of("/\\");
			end=(end!=std::string::npos? end: basePath.size()-1);
			basePath=basePath.substr(0, end+1);
			tinyobj::LoadObj(shapes, materials, filename, basePath.c_str());
		}

		m_engine->log(std::string("Load Obj Scene: (")+std::to_string(shapes.size())+": "+std::to_string(materials.size())+")"+basePath);

		//TODO: Context management
		auto cw=m_engine->getCurrentContext();
		auto tMgr=cw->getTextureManager();
		auto objNode=SceneNodePtr(new EmptySceneNode(node, cw));
		objNode->setName(filename);
		node->addChild(objNode);
		for(auto &shape: shapes)
		{
			std::vector<Vertex> vertices;
			for(uint32_t v=0; v<shape.mesh.positions.size()/3; v++)
			{
				vertices.push_back({glm::vec3(shape.mesh.positions[v*3], shape.mesh.positions[v*3+1], shape.mesh.positions[v*3+2]),
								shape.mesh.texcoords.size()>0?glm::vec2(shape.mesh.texcoords.at(v*2), 1.0f-shape.mesh.texcoords.at(v*2+1)): glm::vec2(),
								glm::vec3(shape.mesh.normals.at(v*3), shape.mesh.normals.at(v*3+1), shape.mesh.normals.at(v*3+2))});
			}
			MeshSceneNode *newNode = new MeshSceneNode(objNode, cw, vertices, shape.mesh.indices, shape.name.c_str());
			
			auto m_defaultMaterial = std::make_shared<Material>(m_program);

			int32_t mid=-1;
			for(uint32_t i=0; mid==-1&&i<shape.mesh.material_ids.size(); i++)
				mid=shape.mesh.material_ids[i]>=0? shape.mesh.material_ids[i]: mid;

			m_defaultMaterial->setDiffuse(tMgr->registerColorTexture2D("white.png"));
			if(mid>=0)
			{
				auto material=materials[mid];

				// We use both texture and diffuse for color masking
				if(material.diffuse_texname!="")
				{
					auto tex=tMgr->registerColorTexture2D(basePath+material.diffuse_texname);
					m_defaultMaterial->setDiffuse(tex);
				}
				m_defaultMaterial->setDiffuse(glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]));
				
				// If there exists specular map, don't use specular in mtl
				if(material.specular_texname!="")
				{
					auto tex=tMgr->registerColorTexture2D(basePath+material.specular_texname);
					m_defaultMaterial->setSpecular(tex);
				}else
					m_defaultMaterial->setSpecular(glm::vec3(material.specular[0], material.specular[1], material.specular[2]));
			}
			newNode->setMaterial(m_defaultMaterial);

			objNode->addChild(SceneNodePtr(newNode));
		}
		return objNode.get();
	}

	SceneNode *SceneManager::addEmptySceneNode(SceneNodePtr node)
	{
		auto cw=m_engine->getCurrentContext();
		auto objNode=SceneNodePtr(new EmptySceneNode(node, cw));
		//objNode->setName(filename);
		node->addChild(objNode);
		return objNode.get();
	}
}