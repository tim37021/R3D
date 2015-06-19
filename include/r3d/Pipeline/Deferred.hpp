#ifndef __R3D_PIPELINE_DEFERRED_HPP_
#define __R3D_PIPELINE_DEFERRED_HPP_

#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <r3d/Core/Texture.hpp>

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
	class RenderTarget2D;
	class SceneNode;

	typedef std::shared_ptr<RenderTarget2D> RenderTarget2DPtr;
	typedef std::shared_ptr<Program> ProgramPtr;

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
		Camera *m_lightCamera;
		RenderTarget2DPtr m_renderTarget;
		ColorTexture2D *m_noiseMap;
		GBuffer *m_gBuffer;

		SSAO *m_ssao;

		// point light program
		ProgramPtr m_programPL, m_programSL, m_programA, m_programDepth;

		// attributeless vao
		VertexArray *m_vao;

		void litPointLight(PointLight *light);
		void litSpotLight(SpotLight *);
		void litAmbientLight();
		void beginLightPass();
		void endLightPass();

		bool renderMaterial(Camera *cam, SceneNode *node, const glm::mat4 &trans, const glm::mat4 &rot);
		bool renderDepth(Camera *cam, SceneNode *node, const glm::mat4 &trans, const glm::mat4 &rot);
		bool findObjectByID(uint32_t id, SceneNode *&searchResult, SceneNode *node, const glm::mat4 &trans, const glm::mat4 &rot);
		void foreachSceneNode(SceneNode *root, std::function<bool(SceneNode *, const glm::mat4&, const glm::mat4&)> callback);

		void prepareProgramInput();
		void prepareViewRelativeUniform(ProgramPtr program, Camera *cam);
	};

}

#endif