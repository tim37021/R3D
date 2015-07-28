#include <r3d/Core/Texture.hpp>

template <class T>
struct ArrayDeleter
{
	void operator()(const T *p)
	{
		delete [] p;
	}
};

namespace r3d
{
	ColorTexture2D::ColorTexture2D(uint32_t width, uint32_t height, PixelFormat pf)
		:Texture2D(width, height), m_pf(pf)
	{
		allocator(width, height);
	}

	void ColorTexture2D::allocator(uint32_t width, uint32_t height)
	{
		int element_size;
		switch(m_pf)
		{
			case PF_R:
				element_size=1; break;
			case PF_RF:
				element_size=4; break;
			case PF_OBJECT_R:
				element_size=4; break;
			case PF_RGB:
			case PF_BGR:
				element_size=3; break;
			case PF_RGBA:
			case PF_BGRA:
				element_size=4; break;
			case PF_RGBF:
			case PF_BGRF:
				element_size=12; break;
			case PF_RGBAF:
			case PF_BGRAF:
				element_size=16; break;
		}
		m_data=std::shared_ptr<uint8_t>(new uint8_t[height*width*element_size], 
			ArrayDeleter<uint8_t>());
	}

	DepthTexture2D::DepthTexture2D(uint32_t width, uint32_t height, DepthFormat df)
		:Texture2D(width, height), m_df(df)
	{
		allocator(width, height);
	}

	void DepthTexture2D::allocator(uint32_t width, uint32_t height)
	{
		m_data=std::shared_ptr<void>(new float[width*height],
			ArrayDeleter<float>());
	}
}