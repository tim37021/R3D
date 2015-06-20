#ifndef __R3D_PIPELINE_DEFERRED_HPP_
#define __R3D_PIPELINE_DEFERRED_HPP_

#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <utility>

namespace r3d
{
	class Engine;
	class ContextWindow;
	class Renderer;
	class GBuffer;
	class Program;
	class VertexArray;
	class PointLight;
	class SpotLight;
	class Camera;
	class SSAO;
	class SceneNode;
	class RenderTarget2D;
	class ColorTexture2D;
	class PostFX;
	class PostEffect;

	typedef std::shared_ptr<Program> ProgramPtr;
	typedef std::shared_ptr<RenderTarget2D> RenderTarget2DPtr;

	class Deferred
	{
	public:
		Deferred(Engine *, ContextWindow *);
		~Deferred();
		void run();

		SceneNode *getObject(uint32_t x, uint32_t y);
	private:
		Engine *m_engine;
		ContextWindow *m_cw;
		Renderer *m_renderer;

		GBuffer *m_gBuffer;

		// post processing
		SSAO *m_ssao;
		PostFX *m_pfx;
		PostEffect *m_bloom;

		// point light program
		ProgramPtr m_programPL, m_programSL, m_programA, m_programDepth, m_programCombine;

		RenderTarget2DPtr m_lightRT;
		ColorTexture2D *m_lightedMap;

		// attributeless vao
		VertexArray *m_vao;

		void litPointLight(Camera *cam, PointLight *light);
		void litSpotLight(Camera *cam, SpotLight *);
		void litAmbientLight(const glm::vec3 &lColor);
		void beginLightPass();
		void endLightPass();
		void combineStage();

		std::pair<glm::vec2, glm::vec2> calcLitRegion(Camera *cam, const glm::vec3 &lightPos, float radius);

		bool renderMaterial(Camera *cam, SceneNode *node, const glm::mat4 &trans, const glm::mat4 &rot);
		bool renderDepth(Camera *cam, SceneNode *node, const glm::mat4 &trans, const glm::mat4 &rot);
		bool findObjectByID(uint32_t id, SceneNode *&searchResult, SceneNode *node, const glm::mat4 &trans, const glm::mat4 &rot);
		void foreachSceneNode(SceneNode *root, std::function<bool(SceneNode *, const glm::mat4&, const glm::mat4&)> callback);

		void prepareProgramInput();
		void prepareViewRelativeUniform(ProgramPtr program, Camera *cam);
	};

}

#endif