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
	"layout(location=0) in vec3 pos;\n"
	"void main(){\n"
	"gl_Position=vec4(pos, 1.0);"
	"}\n";
 
static const char *fragment_shader=
	"#version 330\n"
	"uniform sampler2D posMap;\n"
	"uniform sampler2D diffuseMap;\n"
	"uniform sampler2D normMap;\n"
	"uniform vec2 viewport;"
	"uniform vec3 eyePos;"
	"uniform vec3 lightPos=vec3(0, 30, 0);\n"
	"uniform vec3 lightColor=vec3(0.8)\n;"
	"out vec4 color;\n"
	"void main(){\n"
	"vec2 vTexCoord=gl_FragCoord.xy/viewport;"
	"vec3 pos=texture(posMap, vTexCoord).xyz;\n"
	"vec3 fColor=texture(diffuseMap, vTexCoord).rgb;\n"
	"vec3 norm=texture(normMap, vTexCoord).xyz;"
	"if(length(norm)<=0.5) { color=vec4(0.0); return; }"
	"vec3 lightVec=normalize(lightPos-pos);"
	"float diffuse=dot(norm, lightVec);\n"
	"if(diffuse>=0.0){"
	"float specular = pow(max(dot(reflect(-lightVec, norm), normalize(eyePos-pos)), 0), 30);\n"
	"color=vec4(diffuse*fColor*lightColor+lightColor*specular, 1);} else{color=vec4(0.0f);}\n"
	"}\n";

static R3DRocket::SystemInterface *si;
static R3DRocket::RenderInterface *ri;

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
		switch(action)
		{
			case 1:
			m_context->ProcessMouseButtonDown(button, 0); break;
			case 0:
			m_context->ProcessMouseButtonUp(button, 0); break;
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

static r3d::VertexArray *MakeQuad(r3d::Engine *engine)
{
	std::vector<glm::vec3> vertices=
	{glm::vec3(-1, -1, 0),glm::vec3(1, -1, 0),glm::vec3(1, 1, 0),glm::vec3(-1, 1, 0)};
 
 	auto bMgr=engine->getBufferManager();
 	auto aMgr=engine->getVertexArrayManager();

	auto vbo=bMgr->registerVertexBuffer("QUAD", &vertices[0], vertices.size()*3*sizeof(float), r3d::BU_STATIC_DRAW);
	auto ibo=bMgr->registerIndexBuffer("QUAD", {0, 1, 2, 0, 2, 3}, r3d::BU_STATIC_DRAW);

	auto vao=aMgr->registerVertexArray("QUAD");
	vao->bindVertexBuffer(vbo);
	vao->bindIndexBuffer(ibo);

	r3d::AttribPointer vertAtt(r3d::RT_FLOAT, 3, 0, 0);
	vao->enableAttribArray(0, vertAtt);
	return vao;
}

static void BeginLightPass(r3d::Renderer *renderer, r3d::ContextWindow *cw, r3d::ProgramPtr &program, std::shared_ptr<r3d::GBuffer> &gBuffer)
{
	program->use();
	gBuffer->getPositionMap()->bind(0);
	program->setUniform("posMap", 0);
	gBuffer->getDiffuseMap()->bind(1);
	program->setUniform("diffuseMap", 1);
	gBuffer->getNormalMap()->bind(2);
	program->setUniform("normMap", 2);
	program->setUniform("viewport", glm::vec2(cw->getWidth(), cw->getHeight()));

	renderer->enableBlending(true, r3d::BP_ONE, r3d::BP_ONE, r3d::BF_ADD);
	renderer->enableDepthTest(false);
	renderer->clear();
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

int main(int argc, char *argv[])
{
	r3d::Engine *engine = new r3d::Engine(r3d::RA_OPENGL_3_3);


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

	std::shared_ptr<r3d::GBuffer> gBuffer(new r3d::GBuffer(engine, cw->getWidth(), cw->getHeight()));
	r3d::PostFX::Initialise();
	r3d::PostFX PostFXTest(engine, cw); 
	PostFXTest.pushEffect("bloom");

	// render to texture
	auto program = MakeShaderProgram(engine, vertex_shader, fragment_shader);
	auto vao=MakeQuad(engine);	

	// enable backface culling..
	//engine->getRenderer()->enableBackfaceCulling(true);

	r3d::PointLight p, p2;
	p2.pos=glm::vec3(10);
	p2.color=glm::vec3(2.0f, 2.0f, 2.0f);
	p.pos=glm::vec3(0, 30, 0);
	p.color=glm::vec3(1.0f, 1.0f, 1.0f);
	sMgr->addLight(&p);
	sMgr->addLight(&p2);

	Rocket::Core::Context *context=SetupRocket(engine);
	MyEventListener myel(context, cw);
	cw->setEventListener(&myel);
	RocketEventListener myrel(engine);
	LuaInterface::Initialise(engine);

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
		LuaInterface::updatemspos(width,height);
		gBuffer->beginScene();
		sMgr->drawAll();
		gBuffer->endScene();

		PostFXTest.beginSource(); 
		BeginLightPass(engine->getRenderer(), cw, program, gBuffer);

		// setup post-processing lighting shader parameter
		program->setUniform("eyePos", fps->getPos());

		for(auto light: sMgr->getLights())
		{
			// First light
			program->setUniform("lightPos", light->pos);
			program->setUniform("lightColor", light->color);
			engine->getRenderer()->drawElements(program.get(), vao, r3d::PT_TRIANGLES, 6);
		}
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