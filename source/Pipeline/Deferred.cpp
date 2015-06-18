#include <r3d/Pipeline/Deferred.hpp>
#include <r3d/Pipeline/GBuffer.hpp>
#include <r3d/Pipeline/SSAO.hpp>
#include <r3d/Core/VertexArray.hpp>
#include <r3d/Scene/SceneNode.hpp>
#include <r3d/Scene/Light.hpp>
#include <r3d/Camera/Camera.hpp>
#include <r3d/Core/Core.hpp>
#include <r3d/Renderer/Renderer.hpp>
#include <glm/glm.hpp>
#include <stack>
#include <cstdio>
#include <r3d/Utils/Image.hpp>

static const char *vertex_shader=
	"#version 330\n"
	"void main(){}";

static const char *geometry_shader_ambient=
	"#version 330\n"
	"layout(points) in;\n"
	"layout(triangle_strip, max_vertices=6) out;\n"
	"void main(){"
	"gl_Position=vec4(-1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n"
	"gl_Position=vec4(-1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(-1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n}"
	"\n";

static const char *geometry_shader=
	"#version 330\n"
	"layout(points) in;\n"
	"layout(triangle_strip, max_vertices=6) out;\n"
	"uniform vec3 lightPos\n;"
	"uniform mat4 view;\n"
	"uniform float e=0.1, a=1.0;\n"
	"void main(){\n"
	"float rect[4]=float[](-1, -1, 1, 1);\n"
	"float r=8.0;\n"
	"vec3 l=(view*vec4(lightPos, 1.0)).xyz; vec3 l2=l*l; float r2=r*r;\n"
	"float d=r2*l2.x-(l2.x+l2.z)*(r2-l2.z);\n"
	"if(d>=0){\n"
	"d=sqrt(d);\n"
	"float nx1=(r*l.x+d)/(l2.x+l2.z), nx2=(r*l.x-d)/(l2.x+l2.z);\n"
	"float nz1=(r-nx1*l.x)/l.z, nz2=(r-nx2*l.x)/l.z;\n"
	"float pz1=(l2.x+l2.z-r2)/(l.z-(nz1/nx1)*l.x), pz2=(l2.x+l2.z-r2)/(l.z-(nz2/nx2)*l.x);\n"
	"if(pz1>=0&&pz2>=0) return;\n"
	"if(pz1<0){\n"
	"float fx=nz1/nx1/(tan(22.5)/a);\n"
	"float px=-pz1*nz1/nx1;\n"
	"if(px<l.x) rect[0]=max(rect[0],fx);\n"
	"else rect[2]=min(rect[2],fx);\n"
	"}\n"
	"if(pz2<0){\n"
	"float fx=nz2/nx2/(tan(22.5)/a);\n"
	"float px=-pz2*nz2/nx2;\n"
	"if(px<l.x) rect[0]=max(rect[0],fx);\n"
	"else rect[2]=min(rect[2],fx);\n"
	"}\n"
	"}\n"
	"d=r2*l2.y-(l2.y+l2.z)*(r2-l2.z);\n"
	"if(d>=0){\n"
	"d=sqrt(d);\n"
	"float ny1=(r*l.y+d)/(l2.y+l2.z), ny2=(r*l.y-d)/(l2.y+l2.z);\n"
	"float nz1=(r-ny1*l.y)/l.z, nz2=(r-ny2*l.y)/l.z;\n"
	"float pz1=(l2.y+l2.z-r2)/(l.z-(nz1/ny1)*l.y), pz2=(l2.y+l2.z-r2)/(l.z-(nz2/ny2)*l.y);\n"
	"if(pz1>=0&&pz2>=0) return;\n"
	"if(pz1<0){\n"
	"float fy=nz1/ny1/tan(22.5);\n"
	"float py=-pz1*nz1/ny1;\n"
	"if(py<l.y) rect[1]=max(rect[1],fy);\n"
	"else rect[3]=min(rect[3],fy)\n;"
	"}\n"
	"if(pz2<0){\n"
	"float fy=nz2/ny2/tan(22.5);\n"
	"float py=-pz2*nz2/ny2;\n"
	"if(py<l.y) rect[1]=max(rect[1],fy);\n"
	"else rect[3]=min(rect[3],fy)\n;"
	"}\n"
	"}\n"
	"gl_Position=vec4(rect[0], rect[1], 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(rect[2], rect[1], 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(rect[2], rect[3], 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n"
	"gl_Position=vec4(rect[0], rect[1], 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(rect[2], rect[3], 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(rect[0], rect[3], 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n}"
	"\n";
static const char *fragment_shader_ambient=
	"#version 330\n"
	"uniform sampler2D diffuseMap;"
	"uniform sampler2D AOMap;"
	"uniform vec2 viewport;"
	"uniform vec3 lightColor;"
	"out vec4 color;"
	"void main(){"
	"vec2 vTexCoord=gl_FragCoord.xy/viewport;"
	"vec3 fColor=texture(diffuseMap, vTexCoord).rgb;"
	"float AO=texture(AOMap, vTexCoord).r;"
	"color=vec4(fColor*lightColor*AO, 1.0);"
	"}\n";

static const char *fragment_shader=
	"#version 330\n"
	"uniform sampler2D posMap;\n"
	"uniform sampler2D diffuseMap;\n"
	"uniform sampler2D normMap;\n"
	"uniform sampler2D specMap;\n"
	"uniform vec2 viewport;"
	"uniform vec3 eyePos;"
	"uniform vec3 lightPos;\n"
	"uniform vec3 lightColor;\n;"
	"out vec4 color;\n"
	"void main(){\n"
	"vec2 vTexCoord=gl_FragCoord.xy/viewport;"
	"vec3 pos=texture(posMap, vTexCoord).xyz;\n"
	"vec3 fColor=texture(diffuseMap, vTexCoord).rgb;\n"
	"vec3 norm=texture(normMap, vTexCoord).xyz;\n"
	"vec3 spec=texture(specMap, vTexCoord).rgb;\n"
	"float d=length(pos-lightPos);\n"
	"if(length(norm)<=0.5||d>=8) discard;"
	"vec3 lightVec=normalize(lightPos-pos);"
	"float diffuse=dot(norm, lightVec);\n"
	"if(diffuse>=0.0){"
	"float falloff = 1.0-clamp((d-6.0)/2.0, 0.0, 1.0);\n"
	"float d=length(lightPos-pos);\n"
	"float att=falloff*1.0/(0.9+0.1*d*d);"
	"float specular = pow(max(dot(reflect(-lightVec, norm), normalize(eyePos-pos)), 0), 30);\n"
	"color=att*vec4(diffuse*fColor*lightColor+specular*lightColor*spec, 1);} else discard;\n"
	"}\n";

static const char *fragment_shader_spotlight=
	"#version 330\n"
	"uniform sampler2D posMap;\n"
	"uniform sampler2D diffuseMap;\n"
	"uniform sampler2D normMap;\n"
	"uniform sampler2D specMap;\n"
	"uniform vec2 viewport;"
	"uniform vec3 eyePos;"
	"uniform vec3 lightPos;\n"
	"uniform vec3 lightColor;\n;"
	"uniform vec3 lightDir;\n"
	"uniform float angle;\n"
	"out vec4 color;\n"
	"void main(){\n"
	"vec2 vTexCoord=gl_FragCoord.xy/viewport;"
	"vec3 pos=texture(posMap, vTexCoord).xyz;\n"
	"vec3 fColor=texture(diffuseMap, vTexCoord).rgb;\n"
	"vec3 norm=texture(normMap, vTexCoord).xyz;\n"
	"vec3 spec=texture(specMap, vTexCoord).rgb;\n"
	"float d=length(pos-lightPos);\n"
	"if(length(norm)<=0.5||d>=8) discard;"
	"vec3 lightVec=normalize(lightPos-pos);"
	"float diffuse=dot(norm, lightVec);\n"
	"if(diffuse>=0.0){"
	"if(dot(-lightVec, lightDir) < cos(angle)) discard;\n"
	"float falloff = 1.0-clamp((d-6.0)/2.0, 0.0, 1.0);\n"
	"float d=length(lightPos-pos);\n"
	"float att=falloff*1.0/(0.9+0.1*d*d);"
	"float specular = pow(max(dot(reflect(-lightVec, norm), normalize(eyePos-pos)), 0), 30);\n"
	"color=att*vec4(diffuse*fColor*lightColor+specular*lightColor*spec, 1);} else discard;\n"
	"}\n";
////////////////////////////////////
static const char *vertex_shader_depth=
	"#version 330\n"
	"layout(location=0) in vec3 pos;\n"
	"layout(location=1) in vec2 texCoord;\n"
	"layout(location=2) in vec3 norm;\n"
	"uniform mat4 mvp;"
	"void main(){\n"
	"gl_Position=mvp*vec4(pos, 1.0);"
	"}\n";
static const char *fragment_shader_depth = 
	"#version 330\n"
	"void main(){}";

///////////////////////////////////
namespace r3d
{
	// private struct for rendering
	struct StackElement
	{
		SceneNode *node;
		glm::mat4 parentTransform;
		glm::mat4 parentRotation;

		StackElement(SceneNode *node_, const glm::mat4 &trans=glm::mat4(1.0f), const glm::mat4 &rot=glm::mat4(1.0f)):
			node(node_), parentTransform(trans), parentRotation(rot){}
	};

	Deferred::Deferred(Engine *engine, ContextWindow *cw):
		m_engine(engine), m_cw(cw)
	{
		m_renderer=engine->getRenderer();
		m_vao=m_cw->getVertexArrayManager()->registerVertexArray("ATTRIBUTELESS");

		m_programPL = r3d::MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader);
		m_programSL = r3d::MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader_spotlight);
		m_programA = r3d::MakeShaderProgram(engine, vertex_shader, geometry_shader_ambient, fragment_shader_ambient);
		m_programDepth = r3d::MakeShaderProgram(engine, vertex_shader_depth, fragment_shader_depth);
		prepareProgramInput();

		m_gBuffer = new GBuffer(engine, cw->getWidth(), cw->getHeight());
		m_ssao = new SSAO(engine, cw, m_gBuffer->getDepthMap(), m_gBuffer->getPositionMap(), m_gBuffer->getNormalMap());

		SpotLight *light=new SpotLight(cw);
		light->color=glm::vec3(1.0f);
		light->pos=glm::vec3(0, 5, 0);
		light->dir=glm::vec3(0, -1, 0);
		light->angle=10.0f;
		cw->getSceneManager()->addLight(light);
	}

	Deferred::~Deferred()
	{
		delete m_gBuffer;
		delete m_ssao;
	}

	void Deferred::run()
	{
		m_gBuffer->beginScene();
		foreachSceneNode(m_cw->getSceneManager()->getRootNode().get(), 
			std::bind(&Deferred::renderMaterial, this, 
				m_cw->getSceneManager()->getMainCamera(), 
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		m_gBuffer->endScene();

		m_ssao->update(m_cw->getSceneManager()->getMainCamera());

		beginLightPass();

		prepareViewRelativeUniform(m_programPL, m_cw->getSceneManager()->getMainCamera());
		prepareViewRelativeUniform(m_programA, m_cw->getSceneManager()->getMainCamera());
		prepareViewRelativeUniform(m_programSL, m_cw->getSceneManager()->getMainCamera());

		// lit the light one by one
		for(auto light: m_cw->getSceneManager()->getLights())
		{
			switch(light->type)
			{
				case LT_POINT_LIGHT:
					litPointLight((PointLight *)light);
					break;
				case LT_SPOT_LIGHT:
					litSpotLight((SpotLight *)light);
				default:;
			}
		}

		litAmbientLight();

		endLightPass();
	}

	void Deferred::foreachSceneNode(SceneNode *root, std::function<bool(SceneNode *, const glm::mat4&, const glm::mat4&)> callback)
	{
		std::stack<StackElement> stk;
		stk.push({root});
		while(!stk.empty())
		{
			StackElement top=stk.top(); stk.pop();

			if(!callback(top.node, top.parentTransform, top.parentRotation))
				break;

			const glm::mat4 &tmpMatrix=top.parentTransform*top.node->getTransformation()->getMatrix();
			const glm::mat4 &tmpRotation=top.parentRotation*top.node->getTransformation()->getRotationMatrix();
			auto &clist=top.node->getChildren();
			for(auto &child: clist)
				stk.push({child.get(), tmpMatrix, tmpRotation});
		}
	}

	bool Deferred::renderMaterial(Camera *cam, SceneNode *node, const glm::mat4 &trans, const glm::mat4 &rot)
	{
		auto mtl=node->getMaterial();
		if(mtl)
		{
			auto mtl_shader=mtl->getProgram().get();
			m_renderer->setFillMode(mtl->getFillMode());
			mtl->prepareShader();
			mtl_shader->setUniform("id", node->getID());
			node->render(m_renderer, mtl_shader, cam, trans, rot);
		}
		return true;
	}
	bool Deferred::renderDepth(Camera *cam, SceneNode *node, const glm::mat4 &trans, const glm::mat4 &rot)
	{
		node->render(m_renderer, m_programDepth.get(), cam, trans, rot);
		return true;
	}

	bool Deferred::findObjectByID(uint32_t id, SceneNode *&searchResult, SceneNode *node, const glm::mat4 &trans, const glm::mat4 &rot)
	{
		if(node->getID()!=id)
			return true;
		
		searchResult=node;
		return false;
	}

	void Deferred::beginLightPass()
	{
		// preparing gbuffer
		m_gBuffer->getPositionMap()->bind(0);
		m_gBuffer->getDiffuseMap()->bind(1);
		m_gBuffer->getNormalMap()->bind(2);
		m_gBuffer->getSpecularMap()->bind(3);

		// disable depth test!!!!
		m_renderer->enableDepthTest(false);
		// additive blending
		m_renderer->enableBlending(true, BP_ONE, BP_ONE, BF_ADD);

		m_renderer->clear();
	}

	void Deferred::endLightPass()
	{
		m_renderer->enableBlending(false);
	}

	void Deferred::litPointLight(PointLight *light)
	{
		m_programPL->setUniform("lightPos", light->pos);
		m_programPL->setUniform("lightColor", light->color);
		m_renderer->drawArrays(m_programPL.get(), m_vao, PT_POINTS, 1);
	}

	void Deferred::litSpotLight(SpotLight *light )
	{

		m_programSL->setUniform("lightPos", light->pos);
		m_programSL->setUniform("lightColor", light->color);
		m_programSL->setUniform("lightDir", glm::normalize(light->dir));
		m_programSL->setUniform("angle", light->angle);
		m_renderer->drawArrays(m_programSL.get(), m_vao, PT_POINTS, 1);	

	}

	void Deferred::litAmbientLight()
	{
		m_gBuffer->getDiffuseMap()->bind(0);
		m_ssao->getBlurredAmbientMap()->bind(1);
		m_programA->setUniform("lightColor", {0.1f, 0.1f, 0.1f});
		m_renderer->drawArrays(m_programA.get(), m_vao, PT_POINTS, 1);
	}
	

	void Deferred::prepareProgramInput()
	{
		m_programPL->setUniform("posMap", 0);
		m_programPL->setUniform("diffuseMap", 1);
		m_programPL->setUniform("normMap", 2);
		m_programPL->setUniform("specMap", 3);

		m_programSL->setUniform("posMap", 0);
		m_programSL->setUniform("diffuseMap", 1);
		m_programSL->setUniform("normMap", 2);
		m_programSL->setUniform("specMap", 3);

		m_programA->setUniform("diffuseMap", 0);
		m_programA->setUniform("AOMap", 1);
	}

	void Deferred::prepareViewRelativeUniform(ProgramPtr program, Camera *cam)
	{
		program->setUniform("eyePos", cam->getPos());
		program->setUniform("view", cam->getVMatrix());
		program->setUniform("a", (float)m_cw->getHeight()/m_cw->getWidth());
		program->setUniform("e", 0.1f);
		program->setUniform("viewport", {m_cw->getWidth(), m_cw->getHeight()});
	}

	SceneNode *Deferred::getObject(uint32_t x, uint32_t y)
	{
		uint32_t id;
		uint32_t *ptr=(uint32_t *)m_gBuffer->getObjectMap()->lock();
		id=ptr[(m_cw->getHeight()-y)*m_cw->getWidth()+x];
		m_gBuffer->getObjectMap()->unlock();

		SceneNode *searchResult=nullptr;

		foreachSceneNode(m_cw->getSceneManager()->getRootNode().get(), std::bind(&Deferred::findObjectByID, this, id, std::ref(searchResult), 
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		return searchResult;
	}
}