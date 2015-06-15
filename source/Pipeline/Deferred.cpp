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
	"vec3 AO=texture(AOMap, vTexCoord).rgb;"
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

namespace r3d
{
	// private struct for rendering
	struct StackElement
	{
		SceneNode *node;
		glm::mat4 parentTransform;
		glm::mat4 parentRotation;
	};

	Deferred::Deferred(Engine *engine, ContextWindow *cw):
		m_engine(engine), m_cw(cw)
	{
		m_renderer=engine->getRenderer();
		m_vao=m_cw->getVertexArrayManager()->registerVertexArray("ATTRIBUTELESS");

		m_programPL = r3d::MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader);
		m_programA = r3d::MakeShaderProgram(engine, vertex_shader, geometry_shader_ambient, fragment_shader_ambient);
		prepareProgramInput();

		m_gBuffer = new GBuffer(engine, cw->getWidth(), cw->getHeight());
		m_ssao = new SSAO(engine, cw, m_gBuffer->getDepthMap(), m_gBuffer->getPositionMap(), m_gBuffer->getNormalMap());
	}

	Deferred::~Deferred()
	{
		delete m_gBuffer;
		delete m_ssao;
	}

	void Deferred::run()
	{
		m_gBuffer->beginScene();
		m_cw->getSceneManager()->drawAll();
		m_gBuffer->endScene();

		m_ssao->update(m_cw->getSceneManager()->getMainCamera());

		beginLightPass();

		prepareViewRelativeUniform(m_programPL, m_cw->getSceneManager()->getMainCamera());
		prepareViewRelativeUniform(m_programA, m_cw->getSceneManager()->getMainCamera());

		// lit the light one by one
		for(auto light: m_cw->getSceneManager()->getLights())
		{
			switch(light->type)
			{
				case LT_POINT_LIGHT:
					litPointLight((PointLight *)light);
					break;
				default:;
			}
		}

		litAmbientLight();

		endLightPass();
	}

	void Deferred::renderMaterial()
	{
		std::stack<StackElement> stk;
		//stk.push(stk.getRootNode);
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

	void Deferred::litAmbientLight()
	{
		m_gBuffer->getDiffuseMap()->bind(0);
		m_ssao->getBlurredAmbientMap()->bind(1);
		m_programA->setUniform("lightColor", {0.8f, 0.8f, 0.8f});
		m_renderer->drawArrays(m_programA.get(), m_vao, PT_POINTS, 1);
	}
	

	void Deferred::prepareProgramInput()
	{
		m_programPL->setUniform("posMap", 0);
		m_programPL->setUniform("diffuseMap", 1);
		m_programPL->setUniform("normMap", 2);
		m_programPL->setUniform("specMap", 3);

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
}