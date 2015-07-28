#ifndef __R3D_CORE_TEXTURE_H_
#define __R3D_CORE_TEXTURE_H_

#include <cstdint>
#include <memory>
#include <glm/glm.hpp>

namespace r3d
{

	class Image;

	enum PixelFormat
	{
		PF_R,
		PF_RF,
		// texture for storing object, is an unsigned int texture storing in R component
		PF_OBJECT_R,
		PF_RGB,
		PF_BGR,
		PF_RGBA,
		PF_BGRA,
		PF_RGBF,
		PF_BGRF,
		PF_RGBAF,
		PF_BGRAF
	};

	enum DepthFormat
	{
		DF_24,
		DF_32
	};

	enum Wrapping
	{
		W_CLAMP_TO_EDGE,
		W_CLAMP_TO_BORDER,
		W_REPEAT,
		W_MIRRORED_REPEAT
	};

	enum Filter
	{
		F_NEAREST,
		F_LINEAR,
		F_NEAREST_MIPMAP_NEAREST,
		F_LINEAR_MIPMAP_NEAREST,
		F_NEAREST_MIPMAP_LINEAR,
		F_LINEAR_MIPMAP_LINEAR
	};

	enum AccessLevel
	{
		AL_READ_ONLY,
		AL_WRITE_ONLY,
		AL_READ_WRITE
	};

	class ITexture
	{
	public:
		virtual void bind(uint32_t channel)=0;
		virtual void bindImage(uint32_t channel, int level, AccessLevel)=0;
		virtual void unbind()=0;
		virtual void *lock()=0;
		virtual void unlock()=0;

		virtual const glm::vec4 &getBorder()= 0 ;
		virtual void setBorder(const glm::vec4 border) = 0;

		const void *getData() const { return m_data.get(); }
	protected:
		//! data will be stored in raw major
		std::shared_ptr<void> m_data;
		glm::vec4 m_border;
	};

	class Texture2D: public ITexture
	{
	public:
		Texture2D(uint32_t width, uint32_t height)
			: m_width(width), m_height(height) {}
		virtual ~Texture2D(){}

		//! is virtual, implementation might update gpu data
		virtual void resize(uint32_t width, uint32_t height)
		{ 
			m_width=width;
			m_height=height;
			allocator(width, height);
		}

		virtual void setWrapping(Wrapping s, Wrapping t) = 0;
		virtual void setFilter(Filter min, Filter mag) = 0;
		virtual void generateMipmap() = 0;

		uint32_t getWidth() const
		{ return m_width; }

		uint32_t getHeight() const
		{ return m_height; }

		const glm::vec4 &getBorder() { return m_border ;}
		virtual void setBorder(const glm::vec4 border) { m_border = border ;}
		

	protected:
		virtual void allocator(uint32_t width, uint32_t height)=0;
	private:
		uint32_t m_width, m_height;
	};

	class ColorTexture2D: public Texture2D
	{
	public:
		//! Create empty black texture
		ColorTexture2D(uint32_t width, uint32_t height, PixelFormat pf);
		PixelFormat getPixelFormat() const { return m_pf; }
	protected:
		virtual void allocator(uint32_t width, uint32_t height);
	private:
		PixelFormat m_pf;
	};

	// [-1, 1]
	class DepthTexture2D: public Texture2D
	{
	public:
		DepthTexture2D(uint32_t width, uint32_t height, DepthFormat df);
		DepthFormat getDepthFormat() const { return m_df; }
	protected:
		virtual void allocator(uint32_t width, uint32_t height);
	private:
		DepthFormat m_df;
	};
}

#endif