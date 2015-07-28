#ifndef __R3D_RENDERER_RENDERER_HPP_
#define __R3D_RENDERER_RENDERER_HPP_

#include <r3d/Shader/Program.hpp>

namespace r3d
{
	enum PrimitiveType
	{
		PT_POINTS,
		PT_LINE_LOOP,
		PT_LINES,
		PT_TRIANGLES
	};

	enum BlendFunction
	{
		BF_ADD,
		BF_SUBTRACT,
		BF_REVERSE_SUBTRACT
	};

	enum BlendParameter
	{
		BP_ZERO,
		BP_ONE,
		BP_SRC_COLOR,
		BP_ONE_MINUS_SRC_COLOR,
		BP_DST_COLOR,
		BP_ONE_MINUS_DST_COLOR,
		BP_SRC_ALPHA,
		BP_ONE_MINUS_SRC_ALPHA,
		BP_DST_ALPHA,
		BP_ONE_MINUS_DST_ALPHA
	};

	enum FillMode
	{
		FM_POINT,
		FM_LINE,
		FM_FILL
	};

	enum Face
	{
		F_FRONT,
		F_BACK,
		F_FRONT_AND_BACK
	};

	class VertexArray;

	class Renderer
	{
	public:
		virtual ~Renderer(){}
		virtual void clear()=0;
		virtual void setViewport(int32_t, int32_t, uint32_t, uint32_t)=0;
		virtual void setFillMode(FillMode)=0;
		virtual void enableDepthTest(bool value)=0;
		virtual void enableFaceCulling(Face, bool value)=0;
		virtual void enableScissorTest(bool enable)=0;
		virtual void setScissorRegion(int32_t, int32_t, uint32_t, uint32_t)=0;
		virtual void enableBlending(bool value, BlendParameter sf=BP_ONE, BlendParameter df=BP_ZERO, BlendFunction bf=BF_ADD)=0;
		virtual void drawElements(Program *program, VertexArray *vao, PrimitiveType, uint32_t count)=0;
		virtual void drawArrays(Program *program, VertexArray *vao, PrimitiveType, uint32_t count)=0;
	};
}

#endif