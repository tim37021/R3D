#include <fstream>
#include <functional>
#include <cstdio>
#include <r3d/r3d.hpp>

#include <Rocket/Core.h>
#include <Rocket/Debugger.h>
#include <Rocket/Controls.h>

//LUA
#include <Rocket/Core/Lua/Interpreter.h>
#include <Rocket/Controls/Lua/Controls.h>
#include "r3drocket.h"
#include "LuaInterface.h"

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


static R3DRocket::SystemInterface *si;
static R3DRocket::RenderInterface *ri;

static r3d::Camera *global_fps;
static r3d::VertexArray *vao;

class MyEventListener: public r3d::EventListener
{
public:
	MyEventListener(Rocket::Core::Context *context_, r3d::ContextWindow *cw_): 
		m_context(context_), cw(cw_)
	{
		FPSMode=false;
	}
	virtual void OnMouseButtonStateChange(int button, int action, int mods)
	{
		if(!FPSMode)
		{
			switch(action)
			{
				case 1:
				m_context->ProcessMouseButtonDown(button, 0); break;
				case 0:
				m_context->ProcessMouseButtonUp(button, 0); break;
			}
		}else
		{
			if(button==1&&action){
				auto sMgr=cw->getSceneManager();
				
				r3d::PointLight *light=new r3d::PointLight();
				light->pos=global_fps->getPos();
				light->color=(glm::vec3(0.3f)+
				glm::vec3((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX));
				sMgr->addLight(light);
			}
		}

	}

	virtual void OnMouseScrollStateChange(double xoffset, double yoffset)
	{
		if(!FPSMode)
			m_context->ProcessMouseWheel(-yoffset, 0);
	}

	virtual void OnCharIncoming(uint32_t chr)
	{
		if(!FPSMode&&chr>=32)
			m_context->ProcessTextInput(chr);
	}

	virtual void OnKeyStateChange(int32_t key, int32_t scancode, 
			int32_t action, int32_t mods)
	{
		Rocket::Core::Input::KeyIdentifier keyMap;
		switch(key)
		{
			case r3d::KC_BACKSPACE:
				keyMap=Rocket::Core::Input::KI_BACK; break;
			case r3d::KC_ENTER:
				keyMap=Rocket::Core::Input::KI_RETURN; break;
			case r3d::KC_LEFT:
				keyMap=Rocket::Core::Input::KI_LEFT; break;
			case r3d::KC_RIGHT:
				keyMap=Rocket::Core::Input::KI_RIGHT; break;
			case r3d::KC_UP:
				keyMap=Rocket::Core::Input::KI_UP; break;
			case r3d::KC_DOWN:
				keyMap=Rocket::Core::Input::KI_DOWN; break;
			case r3d::KC_P:
				break;
			case r3d::KC_F1:
				if(action)
					FPSMode=!FPSMode;
				cw->getMouse()->setPos(cw->getWidth()/2, cw->getHeight()/2);
				return;
			default:
				return;
		}
		if(!FPSMode)
		{
			if(action)
				m_context->ProcessKeyDown(keyMap, 0);
			else
				m_context->ProcessKeyUp(keyMap, 0);
		}
	}
	std::list<r3d::PointLight> ps;
	virtual void OnFilesDropIn(uint32_t count, const char *filenames[])
	{
		/*
		auto sMgr=cw->getManagers()->sMgr;
		for(uint32_t i=0; i<count; i++)
			sMgr->loadObjScene(sMgr->getRootNode(), filenames[i]);*/

		LuaInterface::OnFilesDropIn(count, filenames);
	}
	
	bool FPSMode;
private:
	Rocket::Core::Context *m_context;
	r3d::ContextWindow *cw;
};

class RocketEventListener: public Rocket::Core::EventListener
{
public:
	RocketEventListener(r3d::Engine *engine_): engine(engine_){}
	virtual void ProcessEvent(Rocket::Core::Event& event)
	{
		auto sMgr=engine->getSceneManager();
		auto var=event.GetCurrentElement()->GetElementById("fn")->GetAttribute("value");
		float zoom=event.GetCurrentElement()->GetElementById("zoom")->GetAttribute("value")->Get<float>();
		auto node=sMgr->loadObjScene(sMgr->getRootNode(), var->Get<Rocket::Core::String>().CString());
		node->getTransformation()->setScale({zoom, zoom, zoom});
	}
private:
	r3d::Engine *engine;
};

static void BeginLightPass(r3d::Renderer *renderer, r3d::ContextWindow *cw, r3d::ProgramPtr &program, std::shared_ptr<r3d::GBuffer> &gBuffer)
{
	program->use();
	gBuffer->getPositionMap()->bind(0);
	program->setUniform("posMap", 0);
	gBuffer->getDiffuseMap()->bind(1);
	program->setUniform("diffuseMap", 1);
	gBuffer->getNormalMap()->bind(2);
	program->setUniform("normMap", 2);
	gBuffer->getSpecularMap()->bind(3);
	program->setUniform("specMap", 3);
	program->setUniform("viewport", glm::vec2(cw->getWidth(), cw->getHeight()));


	renderer->enableDepthTest(false);
	renderer->clear();
	renderer->enableBlending(true, r3d::BP_ONE, r3d::BP_ONE, r3d::BF_ADD);
}

static Rocket::Core::Context *SetupRocket(r3d::Engine *engine)
{
	si=new R3DRocket::SystemInterface(engine);
	ri=new R3DRocket::RenderInterface(engine);
	Rocket::Core::SetSystemInterface(si);
	Rocket::Core::SetRenderInterface(ri);

	Rocket::Core::Initialise();
	Rocket::Controls::Initialise();

	// Initialise the Lua interface
	Rocket::Core::Lua::Interpreter::Initialise();
	Rocket::Controls::Lua::RegisterTypes(Rocket::Core::Lua::Interpreter::GetLuaState());
	/////
	// Load and show the tutorial document.
	auto cw=engine->getCurrentContext();
	Rocket::Core::Context* context = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(cw->getWidth(), cw->getHeight()));
	if(!context) return nullptr;

	// Initialize debugger
	Rocket::Debugger::Initialise(context);
	Rocket::Debugger::SetVisible(true);

	Rocket::Core::Lua::Interpreter::LoadFile("assets/start.lua");

	return context;
}

static void litPointLight(r3d::Renderer *renderer, r3d::ProgramPtr program, r3d::PointLight *light)
{
	program->setUniform("lightPos", light->pos);
	program->setUniform("lightColor", light->color);
	renderer->drawArrays(program.get(), vao, r3d::PT_POINTS, 1);
}

static void litAmbientLight(r3d::Renderer *renderer, r3d::ProgramPtr program, r3d::SSAO *ssao)
{
	ssao->getBlurredAmbientMap()->bind(1);
	program->setUniform("lightColor", {0.8f, 0.8f, 0.8f});
	program->setUniform("AOMap", 1);
	renderer->drawArrays(program.get(), vao, r3d::PT_POINTS, 1);
}

int main(int argc, char *argv[])
{
	r3d::Engine *engine = new r3d::Engine(r3d::RA_OPENGL_4_3);


	int width=1440, height=900;
	if(argc==3)
	{
		width=atoi(argv[1]);
		height=atoi(argv[2]);
	}
	auto cw=engine->newContextWindow(width, height, "R3D Engine v0.1");

	auto sMgr=engine->getSceneManager();
	std::shared_ptr<r3d::Camera> fps(new r3d::FPSCamera(cw, 45.0f, glm::vec3(5.0f, 0.0f, 0.0f)));
	sMgr->setMainCamera(fps);
	global_fps=fps.get();

	std::shared_ptr<r3d::GBuffer> gBuffer(new r3d::GBuffer(engine, cw->getWidth(), cw->getHeight()));
	r3d::PostFX::Initialise();
	r3d::PostFX PostFXTest(engine, cw); 
	PostFXTest.pushEffect("bloom");

	// For lighting
	auto program = r3d::MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader);
	auto program_ambient = r3d::MakeShaderProgram(engine, vertex_shader, geometry_shader_ambient, fragment_shader_ambient);
	vao = cw->getVertexArrayManager()->registerVertexArray("ATTRIBUTELESS");

	Rocket::Core::Context *context=SetupRocket(engine);
	MyEventListener myel(context, cw);
	cw->setEventListener(&myel);
	RocketEventListener myrel(engine);
	LuaInterface::Initialise(engine);

	r3d::SSAO ssao(engine, cw, gBuffer->getPositionMap(), gBuffer->getDepthMap(), gBuffer->getNormalMap());

	cw->getMouse()->setPos(cw->getWidth()/2, cw->getHeight()/2);
	fps->update(engine->getTime());

	while(!cw->isCloseButtonClicked())
	{
		if(myel.FPSMode)
			fps->update(engine->getTime());
		else
		{
			double posx, posy;
			cw->getMouse()->getPos(&posx, &posy);
			context->ProcessMouseMove((int)posx, (int)posy, 0);
		}
		//Update GBuffer
		gBuffer->beginScene();
		sMgr->drawAll();
		gBuffer->endScene();
		
		ssao.update(fps.get());

		PostFXTest.beginSource(); 
		BeginLightPass(engine->getRenderer(), cw, program, gBuffer);

		// setup post-processing lighting shader parameter
		program->setUniform("eyePos", fps->getPos());
		program->setUniform("view", fps->getVMatrix());
		program->setUniform("a", (float)cw->getHeight()/cw->getWidth());
		program->setUniform("e", 0.1f);

		for(auto light: sMgr->getLights())
		{
			switch(light->type)
			{
				case r3d::LT_POINT_LIGHT:
					litPointLight(engine->getRenderer(), program, (r3d::PointLight *)light);
					break;
				default:;
			}
		}

		program_ambient->use();
		gBuffer->getDiffuseMap()->bind(0);
		program_ambient->setUniform("diffuseMap", 0);
		program_ambient->setUniform("viewport", glm::vec2(cw->getWidth(), cw->getHeight()));

		litAmbientLight(engine->getRenderer(), program_ambient, &ssao);
		PostFXTest.endSource();
	
		engine->getRenderer()->clear();
		PostFXTest.runAll();
		engine->getRenderer()->enableBlending(true, r3d::BP_SRC_ALPHA, r3d::BP_ONE_MINUS_SRC_ALPHA, r3d::BF_ADD);
		context->Render();

		cw->pollInput();
		cw->swapBuffers();
	}

	delete engine;
}