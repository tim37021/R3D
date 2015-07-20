#include <r3d/Manager/SceneManager.hpp>
#include <r3d/Core/Core.hpp>
#include <r3d/Core/Vertex.hpp>
#include <r3d/Core/AttribPointer.hpp>
#include <r3d/Scene/SceneNode.hpp>
#include <glm/glm.hpp>
#include <r3d/Material/Material.hpp>
#include <r3d/Utils/ObjLoader.hpp>
#include "../Scene/MeshSceneNode.hpp"
#include "../Scene/EmptySceneNode.hpp"


static const char *vertex_shader=
	"#version 330\n"
	"layout(location=0) in vec3 pos;\n"
	"layout(location=1) in vec2 texCoord;\n"
	"layout(location=2) in vec3 norm;\n"
	"layout(location=3) in vec3 tangent;\n"
	//"layout(location=4) in vec3 bitangent;\n"
	"uniform mat4 mvp;"
	"uniform mat4 model;"
	"out vec3 vNorm;\n"
	"out vec3 vWorldPos;\n"
	"out vec2 vTexCoord;\n"
	"out vec3 vTangent;\n"
	"out vec3 vBitangent;\n"
	"void main(){\n"
	"vWorldPos=(model*vec4(pos, 1.0)).xyz;"
	"vTexCoord=texCoord;"
	"vNorm=norm;"
	"vTangent=tangent;\n"
	//"vBitangent=bitangent;\n"
	"gl_Position=mvp*vec4(pos, 1.0);"
	"}\n";

static const char *geometry_shader=
	"#version 330\n"
	"layout(triangles) in;\n"
	"layout(triangle_strip, max_vertices=3) out;\n"
	"in vec3 vNorm[3];\n"
	"in vec3 vWorldPos[3];\n"
	"in vec2 vTexCoord[3];\n"
	"in vec3 vTangent[3];\n"
	//"in vec3 vBitangent[3];\n"
	"out vec3 gNorm;\n"
	"out vec3 gWorldPos;\n"
	"out vec2 gTexCoord;\n"
	"out vec3 vBC;\n"
	"out vec3 gTangent;\n"
	//"out vec3 gBitangent;\n"
	"out mat3 gTBN;\n"
	"out mat3 gInvTBN;\n"

	"uniform int enableSmooth=1;"
	"uniform mat4 nmat;"
	"vec3 barycentric[3] = vec3[3](vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));"
	"void main(){\n"
	"vec3 oa=vWorldPos[1]-vWorldPos[0];\n"
	"vec3 ob=vWorldPos[2]-vWorldPos[0];\n"
	"vec3 norm=normalize(cross(oa, ob));\n"
	"for(int i=0; i<3; i++){\n"
	"gl_Position=gl_in[i].gl_Position;\n"
	"gNorm=(enableSmooth==1? (nmat*vec4(vNorm[i], 0.0)).xyz: norm);\n"
	"gWorldPos=vWorldPos[i];\n"
	"gTexCoord=vTexCoord[i];\n"
	"gTangent=(nmat*vec4(vTangent[i], 0.0)).xyz;\n"
	"vec3 bitan = cross(gTangent, gNorm);"
	"gTBN = mat3(gTangent, bitan, gNorm);\n"
	"gInvTBN = transpose(gTBN);"
	//"gBitangent=vBitangent[i];\n"
	"\n"
	"vBC=barycentric[i];\n"
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
	"uniform vec3 emission;\n"	
	"uniform sampler2D diffuseTexture;\n"
	"uniform sampler2D specularTexture;\n"
	"uniform sampler2D normalTexture;\n"
	"uniform sampler2D heightTexture;\n"
	"uniform vec3 eyePos;\n"
	"uniform uint id;\n"
	"uniform int wireframeView;\n"
	"uniform float normalMapIntensity;\n"
	"uniform float parallaxMapIntensity;\n"
	"in vec2 gTexCoord;\n"
	"in vec3 gWorldPos;\n"
	"in vec3 gNorm;\n"
	"in vec3 vBC;\n"
	"in vec3 gTangent;\n"
	"in mat3 gTBN;\n"
	"in mat3 gInvTBN;\n"
	//"in vec3 gBitangent;\n"

	"float edgeFactor(){\n"
	"    vec3 d = fwidth(vBC);\n"
	"    vec3 a3 = smoothstep(vec3(0.0), d*1.5, vBC);\n"
	"    return min(min(a3.x, a3.y), a3.z);\n"
	"}"

	"void main(){\n"
	"vec3 viewVecTangnet = normalize(gInvTBN*(eyePos-gWorldPos));\n"
	"vec2 ggTexCoord = gTexCoord + viewVecTangnet.xy*0.05*(texture(heightTexture, gTexCoord).x-0.1);\n"

	"worldPosMap=gWorldPos;\n"
	"vec3 color=pow(texture(diffuseTexture, ggTexCoord).rgb, vec3(2.2))*diffuse;\n"
	"diffuseMap=(wireframeView==1? mix(vec3(0.0), color, edgeFactor()): color);\n"
	"vec3 norm_w = gTBN*(texture(normalTexture, ggTexCoord).rgb*2.0-vec3(1.0));\n"
	"normalMap=normalize(gNorm+normalMapIntensity*norm_w);\n"
	"specularMap=(specular.x<0?vec3(texture(heightTexture, ggTexCoord)): specular);\n"
	"objectMap=id;\n"
	"}\n";

static std::string ReplaceString(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

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
		std::size_t end=-1;

		if(base)
			tinyobj::LoadObj(shapes, materials, filename, base);
		else
		{
			end=basePath.find_last_of("/\\");
			end=(end!=std::string::npos? end: basePath.size()-1);
			basePath=basePath.substr(0, end+1);
			tinyobj::LoadObj(shapes, materials, filename, basePath.c_str());
		}

		m_engine->log(std::string("Load Obj Scene: (")+std::to_string(shapes.size())+": "+std::to_string(materials.size())+")"+basePath);

		//TODO: Context management
		auto cw=m_engine->getCurrentContext();
		auto tMgr=cw->getTextureManager();
		auto objNode=SceneNodePtr(new EmptySceneNode(node, cw));
		objNode->setName(ReplaceString(std::string(filename+end+1), ".", "_").c_str());
		node->addChild(objNode);
		for(auto &shape: shapes)
		{
			MeshSceneNode *newNode;
			
			auto m_defaultMaterial = std::make_shared<Material>(m_program);

			m_defaultMaterial->setDiffuse(tMgr->registerColorTexture2D("white.png"));

			// Find available material
			int32_t mid=-1;
			for(uint32_t i=0; mid==-1&&i<shape.mesh.material_ids.size(); i++)
				mid=shape.mesh.material_ids[i]>=0? shape.mesh.material_ids[i]: mid;
			if(mid>=0)
			{
				auto material=materials[mid];

				if(material.normal_texname!="")
				{
					// construct object with tangent and bitangent
					newNode = new MeshSceneNode(objNode, cw, shape, true);
					auto tex=tMgr->registerColorTexture2D(basePath+material.normal_texname);
					m_defaultMaterial->setNormalMap(tex);

					// If bump map exist, we can do parallax mapping
					if(material.bump_texname!="")
					{
						tex=tMgr->registerColorTexture2D(basePath+material.bump_texname);
						m_defaultMaterial->setHeightMap(tex);
					}
				}
				else
					newNode = new MeshSceneNode(objNode, cw, shape);

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
			
			}else
				newNode = new MeshSceneNode(objNode, cw, shape);
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

	SceneNode *SceneManager::addLightSceneNode(SceneNodePtr node, Light *light)
	{
		auto cw=m_engine->getCurrentContext();
		auto objNode=SceneNodePtr(new LightSceneNode(node, cw, light));
		node->addChild(objNode);
		return objNode.get();
	}
}