#ifndef __TEST_R3DROCKET_H_
#define __TEST_R3DROCKET_H_

#include <Rocket/Core.h>
#include <memory>

namespace r3d
{
	class Engine;
	class Buffer;
	class VertexArray;
	class VertexArrayManager;
	class BufferManager;
	class Program;
}

namespace R3DRocket
{
	
	class SystemInterface: public Rocket::Core::SystemInterface
	{
	public:
		SystemInterface(r3d::Engine *engine);
		// Get the number of seconds elapsed since the start of the application.
		virtual float GetElapsedTime();

		// Log the specified message.
		virtual bool LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message);

	private:
		r3d::Engine *m_engine;
	};

	class RenderInterface: public Rocket::Core::RenderInterface
	{
	public:
		RenderInterface(r3d::Engine *engine);
		// Called by Rocket when it wants to render geometry that the application does not wish to optimise.
		virtual void RenderGeometry(Rocket::Core::Vertex* vertices,
		int num_vertices, int* indices, int num_indices, 
		Rocket::Core::TextureHandle texture, 
		const Rocket::Core::Vector2f& translation);

		// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
		virtual Rocket::Core::CompiledGeometryHandle CompileGeometry(Rocket::Core::Vertex* vertices,
 		int num_vertices, int* indices, int num_indices,
 		Rocket::Core::TextureHandle texture);

		// Called by Rocket when it wants to render application-compiled geometry.
		virtual void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, 
			const Rocket::Core::Vector2f& translation);

		// Called by Rocket when it wants to release application-compiled geometry.
		virtual void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry);

		// Called by Rocket when it wants to enable or disable scissoring to clip content.
		virtual void EnableScissorRegion(bool enable);

		// Called by Rocket when it wants to change the scissor region.
		virtual void SetScissorRegion(int x, int y, int width, int height);

		// Called by Rocket when a texture is required by the library.
		virtual bool LoadTexture(Rocket::Core::TextureHandle& texture_handle,
		                         Rocket::Core::Vector2i& texture_dimensions,
		                         const Rocket::Core::String& source);

		// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
		virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
		                             const Rocket::Core::byte* source,
		                             const Rocket::Core::Vector2i& source_dimensions);

		// Called by Rocket when a loaded texture is no longer required.
		virtual void ReleaseTexture(Rocket::Core::TextureHandle texture_handle){}

		void SetViewport(uint32_t width, uint32_t height);
	private:
		r3d::Engine *m_engine;
		r3d::Buffer *vbo, *ibo;
		r3d::VertexArray *vao, *vaoc;
		r3d::VertexArrayManager *m_vMgr;
		r3d::BufferManager *m_bMgr;
		std::shared_ptr<r3d::Program> m_program, m_programNoTexture;
		uint32_t m_tmp_id;
	};
}

#endif