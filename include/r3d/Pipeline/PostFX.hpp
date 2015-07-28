#ifndef __R3D_PIPELINE_POSTFX_HPP_
#define __R3D_PIPELINE_POSTFX_HPP_

#include <list>
#include <string>
#include <memory>

namespace r3d
{
	class PostEffect;
	class PostFX;
	class Engine;
	class ContextWindow;
	class VertexArray;
	class RenderTarget2D;
	class Texture2D;

	typedef std::shared_ptr<RenderTarget2D> RenderTarget2DPtr;

	class PostEffectInstancer
	{
	public:
		PostEffectInstancer()=default;
		PostEffectInstancer(PostEffect *(*ctor)(PostFX *, Texture2D *), void (*dtor)(PostEffect *)):
			m_constructor(ctor), m_destructor(dtor){}
		PostEffectInstancer(const PostEffectInstancer &)=default;
		PostEffect *newPostEffect(PostFX *pfx, Texture2D *input) { return m_constructor(pfx, input); }
		void freePostEffect(PostEffect *effect) { m_destructor(effect); }
	private:
		PostEffect *(*m_constructor)(PostFX *, Texture2D *);
		void (*m_destructor)(PostEffect *);
	};

	class PostEffect
	{
	public:
		PostEffect(PostFX *pfx, const std::string &effectName, Texture2D *input);
		virtual ~PostEffect(){}
		virtual void run()=0;
		virtual Texture2D *getResult()=0;

		const std::string getName() const
		{ return m_effectName; }

	protected:
		PostFX *m_pfx;
		Engine *m_engine;
		std::string m_effectName;
		Texture2D *m_text;
		
		
		ContextWindow *m_cw;
	};

	class PostFX
	{
	public:
		PostFX(Engine *m_engine, ContextWindow *cw);
		~PostFX();

		void runAll();
		PostEffect *pushEffect(const std::string &effectName, Texture2D *input);
		void clear();

		static void Initialise();

		Engine *getEngine() { return m_engine; }
		ContextWindow *getContextWindow() { return m_cw; }

	private:
		Engine *m_engine;
		ContextWindow *m_cw;

		std::list<PostEffect *> effects;
	};

	class PostFXFactory
	{
	public:
		static void RegisterEffect(const std::string &name, PostEffectInstancer);
	};
}

#endif