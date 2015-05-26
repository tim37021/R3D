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

	typedef std::shared_ptr<RenderTarget2D> RenderTarget2DPtr;

	class PostEffectInstancer
	{
	public:
		PostEffectInstancer()=default;
		PostEffectInstancer(PostEffect *(*ctor)(PostFX *), void (*dtor)(PostEffect *)):
			m_constructor(ctor), m_destructor(dtor){}
		PostEffectInstancer(const PostEffectInstancer &)=default;
		PostEffect *newPostEffect(PostFX *pfx) { return m_constructor(pfx); }
		void freePostEffect(PostEffect *effect) { m_destructor(effect); }
	private:
		PostEffect *(*m_constructor)(PostFX *);
		void (*m_destructor)(PostEffect *);
	};

	class PostEffect
	{
	public:
		PostEffect(PostFX *pfx, const std::string &effectName);
		virtual ~PostEffect(){}
		virtual void run()=0;

		const std::string getName() const
		{ return m_effectName; }

	protected:
		PostFX *m_pfx;
		std::string m_effectName;
		Engine *m_engine;
		ContextWindow *m_cw;
	};

	class PostFX
	{
	public:
		PostFX(Engine *m_engine, ContextWindow *cw);
		~PostFX();

		void runAll();
		void pushEffect(const std::string &effectName);
		void clear();

		static void Initialise();

		Engine *getEngine() { return m_engine; }
		ContextWindow *getContextWindow() { return m_cw; }
		VertexArray *getVAO() { return m_vao; }

		void beginSource();
		void endSource();
	private:
		Engine *m_engine;
		ContextWindow *m_cw;
		VertexArray *m_vao;
		RenderTarget2DPtr m_fbosource;

		std::list<PostEffect *> effects;
	};

	class PostFXFactory
	{
	public:
		static void RegisterEffect(const std::string &name, PostEffectInstancer);
	};
}

#endif