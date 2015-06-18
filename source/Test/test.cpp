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


static R3DRocket::SystemInterface *si;
static R3DRocket::RenderInterface *ri;

static r3d::Camera *global_fps;
static r3d::VertexArray *vao;
r3d::Deferred *deferred_pipeline;

class MyEventListener: public r3d::EventListener
{
public:
	MyEventListener(Rocket::Core::Context *context_, r3d::ContextWindow *cw_): 
		m_context(context_), cw(cw_)
	{
		FPSMode=false;
		node=nullptr;
	}
	virtual void OnMouseButtonStateChange(int button, int action, int mods)
	{
		double posx, posy;
		cw->getMouse()->getPos(&posx, &posy);
		if(!FPSMode)
		{
			switch(action)
			{
				case 1:
				m_context->ProcessMouseButtonDown(button, 0); break;
				case 0:
				m_context->ProcessMouseButtonUp(button, 0); break;
			}
			if(action==1&&button==1)
			{
				if(node&&node->getMaterial())
					node->getMaterial()->enableWireframeView(false);
				node=deferred_pipeline->getObject(posx, posy);
				if(node)
				{
					fprintf(stderr, "%s\n", node->getName());
					node->getMaterial()->enableWireframeView(true);
					LuaInterface::SetSelectObject(node);
				}
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
	r3d::SceneNode *node;
	r3d::ContextWindow *cw;
};

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

	LuaInterface::Initialise(engine);

	Rocket::Core::Lua::Interpreter::LoadFile("assets/start.lua");

	return context;
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

	std::shared_ptr<r3d::Camera> fps(new r3d::FPSCamera(cw, 45.0f, glm::vec3(5.0f, 0.0f, 0.0f)));
	cw->getSceneManager()->setMainCamera(fps);
	global_fps=fps.get();

	deferred_pipeline = new r3d::Deferred(engine, cw);

	Rocket::Core::Context *context=SetupRocket(engine);
	MyEventListener myel(context, cw);
	cw->setEventListener(&myel);

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

		engine->getRenderer()->clear();

		deferred_pipeline->run();

		engine->getRenderer()->enableBlending(true, r3d::BP_SRC_ALPHA, r3d::BP_ONE_MINUS_SRC_ALPHA, r3d::BF_ADD);
		context->Update();
		context->Render();

		cw->pollInput();
		cw->swapBuffers();
	}

	delete engine;
}