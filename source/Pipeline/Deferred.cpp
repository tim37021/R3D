#include <r3d/Pipeline/Deferred.hpp>
#include <r3d/Pipeline/GBuffer.hpp>
#include <r3d/Pipeline/SSAO.hpp>
#include <r3d/Pipeline/PostFX.hpp>
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
	"uniform vec2 rectBottomLeft;\n"
	"uniform vec2 rectTopRight;\n"
	"void main(){\n"
	"gl_Position=vec4(rectBottomLeft.x, rectBottomLeft.y, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(rectTopRight.x, rectBottomLeft.y, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(rectTopRight.x, rectTopRight.y, 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n"
	"gl_Position=vec4(rectBottomLeft.x, rectBottomLeft.y, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(rectTopRight.x, rectTopRight.y, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(rectBottomLeft.x, rectTopRight.y, 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n"
	"}\n";
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
	"uniform sampler2D AOMap;\n"
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
	"float ao=texture(AOMap, vTexCoord).r;"
	"float d=length(pos-lightPos);\n"
	"if(length(norm)<=0.5||d>=32) discard;"
	"vec3 lightVec=normalize(lightPos-pos);"
	"float diffuse=dot(norm, lightVec);\n"
	"if(diffuse>=0.0){"
	"float falloff = 1.0-clamp((d-24.0)/8.0, 0.0, 1.0);\n"
	"float d=length(lightPos-pos);\n"
	"float att=falloff*1.0/(0.5+0.5*d*d);"
	"float specular = pow(max(dot(reflect(-lightVec, norm), normalize(eyePos-pos)), 0), 30);\n"
	"color=mix(ao, 1.0, diffuse*diffuse)*att*vec4(diffuse*fColor*lightColor+specular*lightColor*spec, 1);} else discard;\n"
	"}\n";

static const char *fragment_shader_spotlight=
	"#version 330\n"
	"uniform sampler2D posMap;\n"
	"uniform sampler2D diffuseMap;\n"
	"uniform sampler2D normMap;\n"
	"uniform sampler2D specMap;\n"
	"uniform sampler2D AOMap;\n"
	"uniform sampler2D shadowMap;\n"
	"uniform sampler2D noiseMap;\n"
	"uniform vec2 viewport;\n"
	"uniform vec3 eyePos;\n"
	"uniform vec3 lightPos;\n"
	"uniform vec3 lightColor;\n;"
	"uniform vec3 lightDir;\n"
	"uniform vec3 converter\n;"
	"uniform mat4 lightCamVp;\n"
	"uniform float innerAngle;\n"
	"uniform float outerAngle;\n"
	"out vec4 color;\n"

	////////PCF/////// 

	"const int numSamplingPositions = 4;\n"
	"vec2 kernel[9] = vec2 [](\n"
	"	vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),\n"
	"	vec2(-1.0, 0.0), vec2(0.0, 0.0), vec2(1.0, 0.0),\n"
	"	vec2(-1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 1.0)\n"
	");\n"
	
	"vec2 poissonKernel[4] = vec2 [](\n"
	"	vec2( -0.94201624, -0.39906216 ),\n"
  	"	vec2( 0.94558609, -0.76890725 ),\n"
  	"	vec2( -0.094184101, -0.92938870 ),\n"
  	"	vec2( 0.34495938, 0.29387760 )\n"
	");\n"

	"float gaussian[9] = float [] (\n"
	"	0.07511360795411207, 0.12384140315297386, 0.07511360795411207, \n"
	"	0.12384140315297386, 0.20417995557165622, 0.12384140315297386, \n"
	"	0.07511360795411207, 0.12384140315297386, 0.07511360795411207\n"
	");\n"

	
	"float sample(in vec2 coords, in vec2 offset){\n"
	"	return texture(shadowMap, coords + offset/1024);\n"
	"}\n"
	////////

	"float shadowIntensity(vec3 shadowCoord, vec3 pos, vec3 normal){\n"
	////////PCF/////// 
	///Using poisson//
	"	float mapDepth = 0;\n"
	"	float intense = 0;\n"
	"	float bias = -0.01 * tan(acos(dot (normal,  normalize(lightPos - pos)))); \n"
	"	bias = clamp(bias, -0.07, 0); \n"
	"	for (int i = 0; i<4; i++){\n"
	//"		float sampleDepth =sample(shadowCoord.xy, kernel[i] * 2) * 2 - 1;\n"
	"		float sampleDepth = texture(shadowMap, shadowCoord.xy + poissonKernel[i]/3600).x*2 -1 ;\n"
	"		float sampleLinearDepth = converter.x / (converter.y-sampleDepth*converter.z);\n"
	" 		float objectLinearDepth = converter.x / (converter.y-shadowCoord.z*converter.z);\n"
	"		if( sampleLinearDepth + bias < objectLinearDepth ) {\n"
	"			intense +=  0.2; \n" //exp(-5*(objectLinearDepth - sampleLinearDepth )/converter.z)*
	"		}\n"
	"	}\n"
	//////////////////
	"	return intense; \n"	
	"}\n"

	"void main(){\n"
	"vec2 vTexCoord=gl_FragCoord.xy/viewport;\n"
	"vec3 pos=texture(posMap, vTexCoord).xyz;\n"
	"vec3 fColor=texture(diffuseMap, vTexCoord).rgb;\n"
	"vec3 norm=texture(normMap, vTexCoord).xyz;\n"
	"vec3 spec=texture(specMap, vTexCoord).rgb;\n"
	"float ao=texture(AOMap, vTexCoord).r;"
	"float d = length(pos-lightPos);\n"
	"if(length(norm)<=0.5||d>=32) discard;\n"
	"vec3 lightVec=normalize(lightPos-pos);\n"
	"float diffuse=dot(norm, lightVec);\n"
	"if(diffuse>=0.0){\n"
	"	float angleCos = dot(-lightVec, lightDir); "
	"	if(angleCos < cos(outerAngle)) discard;\n"
	"	vec4 shadowCoord=lightCamVp* vec4(pos, 1.0);\n"
	"	shadowCoord.xyz/=shadowCoord.w;\n"
	"	shadowCoord.xy=(shadowCoord.xy+vec2 (1.0))/2.0;\n"
	"	float shadow_inten=shadowIntensity(shadowCoord.xyz, pos, norm); \n" // Decide if pixel should be litted
	"	float falloff = 1.0-clamp((d-28)/4.0, 0.0, 1.0);\n"
	"	float d=length(lightPos-pos);\n"
	"	float att=falloff*1.0/(0.5+0.5*d*d);\n"
	"	float specular = pow(max(dot(reflect(-lightVec, norm), normalize(eyePos-pos)), 0), 30);\n"
	"	vec4 lightIntense = mix(ao, 1.0, diffuse*diffuse)*att*vec4(diffuse*fColor*lightColor+specular*lightColor*spec, 1);"
	//Spotlight fading
	"	if(angleCos < cos(innerAngle)) {\n"
	"		float ratio = (acos(angleCos) - innerAngle)/(outerAngle - innerAngle);"
	"		float factor = 1 - ratio * ratio;\n"
	"		lightIntense *= factor; \n"
	"	}\n"

	"	color=(1.0-shadow_inten)*lightIntense; \n"
	"}\n"
	"else discard;\n"

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
static const char *fragment_shader_combine=
	"#version 330\n"
	"uniform sampler2D lightedMap;\n"
	"uniform sampler2D bloomMap;\n"
	"uniform vec2 viewport;\n"
	"uniform float gamma=2.2;\n"
	"out vec4 color;\n"
	"void main(){"
	"vec2 vTexCoord=gl_FragCoord.xy/viewport;\n"
	"vec3 lighted=texture(lightedMap, vTexCoord).rgb;\n"
	"vec3 bloomed=texture(bloomMap, vTexCoord).rgb;"
	"color=vec4(pow(mix(lighted, bloomed, 0.8), vec3(1/gamma)), 1.0);\n"
	"}";

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
		auto tMgr = cw->getTextureManager();

		m_renderer=engine->getRenderer();
		m_vao=m_cw->getVertexArrayManager()->registerVertexArray("ATTRIBUTELESS");

		m_programPL = MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader);
		m_programSL = MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader_spotlight);
		m_programA = MakeShaderProgram(engine, vertex_shader, geometry_shader_ambient, fragment_shader_ambient);
		m_programDepth = MakeShaderProgram(engine, vertex_shader_depth, fragment_shader_depth);
		m_programCombine = MakeShaderProgram(engine, vertex_shader, geometry_shader_ambient, fragment_shader_combine);
		prepareProgramInput();

		m_gBuffer = new GBuffer(engine, cw->getWidth(), cw->getHeight());

		m_ssao = new SSAO(engine, cw, m_gBuffer->getPositionMap(), m_gBuffer->getDepthMap(), m_gBuffer->getNormalMap());
		m_enableSSAO = true;
		m_whiteMap = m_cw->getTextureManager()->registerColorTexture2D("white.png");

		m_pfx = new PostFX(engine, cw);

		SpotLight *light=new SpotLight(cw);

		m_lightRT = engine->newRenderTarget2D();
		m_lightedMap = m_cw->getTextureManager()->registerColorTexture2D("LightedMap", cw->getWidth(), cw->getHeight(), PF_BGRF);
		ColorTexture2D *texts[]={m_lightedMap};
		m_lightRT->attachColorTextures(1, texts);

		// register bloom effect
		m_bloom=m_pfx->pushEffect("bloom", m_lightedMap);
		m_lightCamera = new Camera(m_cw, glm::vec3 (0.0f), glm::vec3 (0.0f), glm::vec3 (0.0f));
	
		m_renderTarget = engine->newRenderTarget2D();
		m_renderTarget->attachDepthTexture(cw->getTextureManager()->registerDepthTexture2D("ShadowMap[1536x1536]", 1536, 1536, DF_24));
		m_noiseMap = tMgr->registerColorTexture2D("noise.png");
	}

	Deferred::~Deferred()
	{
		delete m_gBuffer;
		delete m_ssao;
		delete m_pfx;
		delete m_lightCamera;
	}

	void Deferred::run()
	{
		auto mainCamera=m_cw->getSceneManager()->getMainCamera();
		m_gBuffer->beginScene();
		foreachSceneNode(m_cw->getSceneManager()->getRootNode().get(), 
			std::bind(&Deferred::renderMaterial, this, 
				mainCamera, 
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		m_gBuffer->endScene();

		if(m_enableSSAO)
			m_ssao->update(mainCamera);

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
					litPointLight(mainCamera, (PointLight *)light);
					break;
				case LT_SPOT_LIGHT:
					litSpotLight(mainCamera, (SpotLight *)light);
					break;
				default:;
			}
		}
		litAmbientLight(glm::vec3(0.05f));

		endLightPass();

		m_pfx->runAll();


		combineStage();

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
		m_lightRT->bind();

		// preparing gbuffer
		m_gBuffer->getPositionMap()->bind(0);
		m_gBuffer->getDiffuseMap()->bind(1);
		m_gBuffer->getNormalMap()->bind(2);
		m_gBuffer->getSpecularMap()->bind(3);
		if(m_enableSSAO)
			m_ssao->getBlurredAmbientMap()->bind(4);
		else
			m_whiteMap->bind(4);
		//ShadowMap bind (5) for each light source
		m_noiseMap->bind(6);

		// disable depth test!!!!
		m_renderer->enableDepthTest(false);
		// additive blending
		m_renderer->enableBlending(true, BP_ONE, BP_ONE, BF_ADD);

		m_renderer->clear();
	}

	void Deferred::endLightPass()
	{
		m_renderer->enableBlending(false);
		m_lightRT->unbind();
	}

	void Deferred::litPointLight(Camera *cam, PointLight *light)
	{
		m_programPL->setUniform("lightPos", light->pos);
		m_programPL->setUniform("lightColor", light->color);
		auto region=calcLitRegion(cam, light->pos, 32.0f);
		m_programPL->setUniform("rectBottomLeft", region.first);
		m_programPL->setUniform("rectTopRight", region.second);
		m_renderer->drawArrays(m_programPL.get(), m_vao, PT_POINTS, 1);
	}

	void Deferred::litSpotLight(Camera *cam, SpotLight *light )
	{
		// See if we need this light?
		auto region=calcLitRegion(cam, light->pos, 47.0f);
		if(glm::length(region.second-region.first)<0.005f*1.414f)
			return;

		m_lightCamera->setPos(light->pos);
		m_lightCamera->setDir(light->dir);
		m_lightCamera->setUp(light->up);
		m_lightCamera->setNear(0.4f);
		m_lightCamera->setFar(47.0f);
		m_lightCamera->setAspect(1.0f);
		m_lightCamera->setFov(light->outerAngle*2.0f);
		float near = m_lightCamera->getNear();
		float far = m_lightCamera->getFar();

		m_renderer->enableDepthTest(true);
		m_renderTarget->bind();
		m_renderer->setViewport(0, 0, light->dMap->getWidth(), light->dMap->getHeight());
		m_engine->getRenderer()->enableFaceCulling(F_FRONT, true);
		m_renderer->clear();
		foreachSceneNode(m_cw->getSceneManager()->getRootNode().get(), 
			std::bind(&Deferred::renderDepth, this, m_lightCamera, std::placeholders::_1, std::placeholders::_2
				, std::placeholders::_3));
		m_renderer->enableDepthTest(false);
		m_engine->getRenderer()->enableFaceCulling(F_BACK, true);

		m_lightRT->bind();

		m_renderer->setViewport(0, 0, m_cw->getWidth(), m_cw->getHeight());
		light->dMap->bind(5);

		m_programSL->setUniform("lightPos", light->pos);
		m_programSL->setUniform("lightColor", light->color);
		m_programSL->setUniform("lightDir", glm::normalize(light->dir));

		m_programSL->setUniform("rectBottomLeft", region.first);
		m_programSL->setUniform("rectTopRight", region.second);
		m_programSL->setUniform("innerAngle", light->innerAngle*3.1415f/180);
		m_programSL->setUniform("outerAngle", light->outerAngle*3.1415f/180);
		m_programSL->setUniform("lightCamVp", m_lightCamera->getVPMatrix());
		m_programSL->setUniform("converter", {2.0f*near*far, near+far, far-near});
		m_renderer->drawArrays(m_programSL.get(), m_vao, PT_POINTS, 1);	
	}

	void Deferred::litAmbientLight(const glm::vec3 &lColor)
	{
		m_gBuffer->getDiffuseMap()->bind(0);
		if(m_enableSSAO)
			m_ssao->getBlurredAmbientMap()->bind(1);
		else
			m_whiteMap->bind(1);
		m_programA->setUniform("lightColor", lColor);
		m_renderer->drawArrays(m_programA.get(), m_vao, PT_POINTS, 1);
	}
	

	void Deferred::prepareProgramInput()
	{
		m_programPL->setUniform("posMap", 0);
		m_programPL->setUniform("diffuseMap", 1);
		m_programPL->setUniform("normMap", 2);
		m_programPL->setUniform("specMap", 3);
		m_programPL->setUniform("AOMap", 4);

		m_programSL->setUniform("posMap", 0);
		m_programSL->setUniform("diffuseMap", 1);
		m_programSL->setUniform("normMap", 2);
		m_programSL->setUniform("specMap", 3);
		m_programSL->setUniform("AOMap", 4);
		m_programSL->setUniform("shadowMap", 5);
		m_programSL->setUniform("noiseMap",6);

		m_programA->setUniform("diffuseMap", 0);
		m_programA->setUniform("AOMap", 1);

		m_programCombine->setUniform("lighedMap", 0);
		m_programCombine->setUniform("bloomMap", 1);
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

	void Deferred::combineStage()
	{
		m_programCombine->setUniform("viewport", {m_cw->getWidth(), m_cw->getHeight()});
		m_lightedMap->bind(0);
		m_bloom->getResult()->bind(1);
		m_renderer->drawArrays(m_programCombine.get(), m_vao, PT_POINTS, 1);

	}

	std::pair<glm::vec2, glm::vec2> Deferred::calcLitRegion(Camera *cam, const glm::vec3 &lightPos, float r)
	{
		float rect[4]={-1, -1, 1, 1};
		float e=-0.1f, a=(float)m_cw->getHeight()/m_cw->getWidth();
		glm::vec3 l=glm::vec3(cam->getVMatrix()*glm::vec4(lightPos, 1.0)); glm::vec3 l2=l*l; float r2=r*r;
		float d=r2*l2.x-(l2.x+l2.z)*(r2-l2.z);
		if(d>=0)
		{
			d=sqrt(d);
			float nx1=(r*l.x+d)/(l2.x+l2.z), nx2=(r*l.x-d)/(l2.x+l2.z);
			float nz1=(r-nx1*l.x)/l.z, nz2=(r-nx2*l.x)/l.z;
			float pz1=(l2.x+l2.z-r2)/(l.z-(nz1/nx1)*l.x), pz2=(l2.x+l2.z-r2)/(l.z-(nz2/nx2)*l.x);
			if(pz1>=0&&pz2>=0) return {{}, {}};
			if(pz1<0)
			{
				float fx=nz1/nx1/(tan(22.5)/a);
				float px=-pz1*nz1/nx1;
				if(px<l.x)
					rect[0]=glm::max(rect[0], fx);
				else
					rect[2]=glm::min(rect[2], fx);
			}
			if(pz2<0)
			{
				float fx=nz2/nx2/(tan(22.5)/a);
				float px=-pz2*nz2/nx2;
				if(px<l.x)
					rect[0]=glm::max(rect[0], fx);
				else
					rect[2]=glm::min(rect[2], fx);
			}
		}
		d=r2*l2.y-(l2.y+l2.z)*(r2-l2.z);
		if(d>=0)
		{
			d=sqrt(d);
			float ny1=(r*l.y+d)/(l2.y+l2.z), ny2=(r*l.y-d)/(l2.y+l2.z);
			float nz1=(r-ny1*l.y)/l.z, nz2=(r-ny2*l.y)/l.z;
			float pz1=(l2.y+l2.z-r2)/(l.z-(nz1/ny1)*l.y), pz2=(l2.y+l2.z-r2)/(l.z-(nz2/ny2)*l.y);
			if(pz1>=0&&pz2>=0) return {{}, {}};
			if(pz1<0)
			{
				float fy=nz1/ny1/tan(22.5);
				float py=-pz1*nz1/ny1;
				if(py<l.y)
					rect[1]=glm::max(rect[1], fy);
				else
					rect[3]=glm::min(rect[3], fy);
			}
			if(pz2<0)
			{
				float fy=nz2/ny2/tan(22.5);
				float py=-pz2*nz2/ny2;
				if(py<l.y)
					rect[1]=glm::max(rect[1], fy);
				else
					rect[3]=glm::min(rect[3], fy);
			}
		}

		return {{rect[0], rect[1]}, {rect[2], rect[3]}};
	}

	void Deferred::setSSAORadius(float r)
	{
		m_ssao->setSampleRadius(r);
	}
}