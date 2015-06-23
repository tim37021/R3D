#include <r3d/Pipeline/SSAO.hpp>
#include <r3d/Core/Core.hpp>
#include <r3d/Window/ContextWindow.hpp>
#include <r3d/Core/Texture.hpp>
#include <r3d/Core/RenderTarget.hpp>
#include <r3d/Renderer/Renderer.hpp>
#include <r3d/Camera/Camera.hpp>

#include <cstdint>
#include <cmath>

static const char *vertex_shader=
	"#version 330\n"
	"void main(){}";

static const char *geometry_shader=
	"#version 330\n"
	"layout(points) in;\n"
	"layout(triangle_strip, max_vertices=6) out;\n"
	"void main(){"
	"gl_Position=vec4(-1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n"
	"gl_Position=vec4(-1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(-1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n}"
	"\n";

static const char *fragment_shader=
	"#version 330\n"
	"uniform sampler2D posMap;"
	"uniform sampler2D depthMap;"
	"uniform sampler2D normMap;"
	"uniform sampler2D noiseMap;"
	"uniform vec3 kernel[64];"
	"uniform ivec3 noiseScale_kernelSize=ivec3(4, 4, 24);"
	"uniform float radius=2.0f;"
	"uniform vec3 converter;"
	"uniform vec2 viewport;"
	"uniform mat4 view, pureView, proj;"
	"layout(location=0) out float color;"

	"mat3 calcTBN(vec3 normal, vec2 texcoord){"
	 	//"ivec2 noiseScale = noiseScale_kernelSize.xy;"
	    "vec3 rvec = normalize(texture(noiseMap, texcoord).xyz);"
	    "vec3 tangent = normalize(rvec - normal * dot(rvec, normal));"
	    "vec3 bitangent = cross(normal, tangent);"
	    "return mat3(tangent, bitangent, normal);"
	"}"

	"void main(){"
		"vec2 vTexCoord=gl_FragCoord.xy/viewport;"
		"vec3 normView=(pureView*texture(normMap, vTexCoord)).xyz;"
		"if(length(normView)<0.5) discard;"
		"vec4 worldPos=texture(posMap, vTexCoord);"
		"vec3 posView=(view*worldPos).xyz;"
		"mat3 tbn=calcTBN(normView, normalize(worldPos.xy+worldPos.zz));"

		"float ao=0.0f;"
		"int kernel_size=noiseScale_kernelSize.z;"
		"for(int i=0; i<kernel_size; i++){"
		"vec3 sample=tbn*kernel[i];"
		"sample=sample*radius+posView;"
		"vec4 offset = proj*vec4(sample, 1.0);"
		"offset.xy/=offset.w;"
		// from [-1, 1] -> [0, 1] to lookUp in depthMap
		"offset.xy=offset.xy*0.5+0.5;"
		"float sample_depth = texture(depthMap, offset.xy).x*2.0-1.0;"
		//"if(sample_depth>0) {color=vec3(1.0, 0.0, 0.0); return;}"
		"float linear_depth = -converter.x / (converter.y-sample_depth*converter.z);"
		"if(abs(posView.z - linear_depth) < radius && linear_depth<-0.1f)"
			"ao += (sample.z <= linear_depth ? 1.0: 0.0);"
		"}"
		"color=1.0-ao/kernel_size;"
	"}\n";

static const char *fragment_shader_blur=
	"#version 330\n"
	"uniform sampler2D text;"
	"uniform vec2 textResol;"
	"uniform vec2 viewport;"
	"layout(location=0) out vec4 color;"
	"void main(){"
	"vec2 vTexCoord=gl_FragCoord.xy/viewport;"
	"const int radius=5;"
    "const float kernel_size=float((radius*2+1)*(radius*2+1));"
    "vec3 c=vec3(0.0);"
	"for(int i=-radius; i<=radius; i++)"
		"for(int j=-radius; j<=radius; j++)"
			"c+=texture2D(text, vTexCoord+vec2(i, j)/textResol).rgb;"
	"c=c/kernel_size;"
	"color=vec4(c, 1.0);"
	"}";
  
namespace r3d
{
	SSAO::SSAO(Engine *engine, ContextWindow *cw, Texture2D *posMap, Texture2D *depthMap, 
		Texture2D *normMap):
		m_engine(engine), m_cw(cw), m_posMap(posMap), m_depthMap(depthMap), m_normMap(normMap)
	{
		// Prepare SSAO shader
		m_programSSAO=MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader);
		m_programSSAO->setUniform("posMap", 0);
		m_programSSAO->setUniform("depthMap", 1);
		m_programSSAO->setUniform("normMap", 2);
		m_programSSAO->setUniform("noiseMap", 3);
		//m_programSSAO->setUniform("noiseScale_kernelSize", {4.0f, 4.0f, 16.0f});
		generateKernel(24);
		uploadKernel();

		// Prepare blur shader
		m_programBlur=MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader_blur);
		m_programBlur->setUniform("text", 0);

		auto tMgr = cw->getTextureManager();
		// Load needed noise texture
		m_noiseMap = tMgr->registerColorTexture2D("noise.png");

		// Generate needed texture storage, in half resolution
		m_ambientMap = tMgr->registerColorTexture2D("SSAOMap", cw->getWidth()/2, cw->getHeight()/2, PF_BGR);
		m_ambientMap->setFilter(F_LINEAR, F_LINEAR);
		// Clamp to edge !!!!!!!!!
		m_ambientMap->setWrapping(W_CLAMP_TO_EDGE, W_CLAMP_TO_EDGE);

		m_ambientMapBlurred = tMgr->registerColorTexture2D("BlurredSSAOMap", cw->getWidth(), cw->getHeight(), PF_BGR);
		m_ambientMapBlurred->setFilter(F_LINEAR, F_LINEAR);
		m_ambientMapBlurred->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);
		//////////////////////////////////////////////

		// Generate fbo for SSAO
		m_fbo = engine->newRenderTarget2D();
		ColorTexture2D *texts[]={m_ambientMap};
		m_fbo->attachColorTextures(1, texts);

		// Generate fbo for blurring
		m_fboBlur = engine->newRenderTarget2D();
		texts[0]=m_ambientMapBlurred;
		m_fboBlur->attachColorTextures(1, texts);

		m_vao = cw->getVertexArrayManager()->registerVertexArray("ATTRIBUTELESS");
	}

	void SSAO::update(Camera *cam)
	{
		glm::mat4 pureView=cam->getVMatrix();
		// clear position
		pureView[3][0]=0; pureView[3][1]=0; pureView[3][2]=0;

		glViewport(0, 0, m_cw->getWidth()/2, m_cw->getHeight()/2);
		m_fbo->bind();
		m_programSSAO->use();
		m_programSSAO->setUniform("viewport", {m_cw->getWidth()/2, m_cw->getHeight()/2});
		m_programSSAO->setUniform("view", cam->getVMatrix());
		m_programSSAO->setUniform("pureView", pureView);
		m_programSSAO->setUniform("proj", cam->getPMatrix());
		m_programSSAO->setUniform("converter", {2.0f*0.1f*1000, 0.1f+1000, 1000-0.1f});
		m_posMap->bind(0);
		m_depthMap->bind(1);
		m_normMap->bind(2);
		m_noiseMap->bind(3);
		m_engine->getRenderer()->clear();
		m_engine->getRenderer()->drawArrays(m_programSSAO.get(), m_vao, PT_POINTS, 1);
		m_fbo->unbind();
		glViewport(0, 0, m_cw->getWidth(), m_cw->getHeight());

		m_fboBlur->bind();
		// prepare shader input...
		m_ambientMap->bind(0);
		m_programBlur->setUniform("viewport", {m_cw->getWidth(), m_cw->getHeight()});
		m_programBlur->setUniform("textResol", {m_cw->getWidth()/2.0f, m_cw->getHeight()/2.0f});
		m_engine->getRenderer()->clear();
		m_engine->getRenderer()->drawArrays(m_programBlur.get(), m_vao, PT_POINTS, 1);
		m_fboBlur->unbind();
		
	}

	void SSAO::generateKernel(int num_kernels)
	{
		m_kernel.resize(num_kernels);
		
		for(int i=0; i<num_kernels; ++i)
		{
			m_kernel[i]=glm::vec3(1, 0, 0);
			while(glm::dot(m_kernel[i], glm::vec3(0, 0, 1))<cos(89.0*3.1415/180.0))
			{
				m_kernel[i].x=((float)rand()/RAND_MAX-0.5f)*2.0f;
				m_kernel[i].y=((float)rand()/RAND_MAX-0.5f)*2.0f;
				m_kernel[i].z=(float)rand()/RAND_MAX;

				m_kernel[i]=glm::normalize(m_kernel[i]);
			}

			// make it within the hemisphere by scaling
			float scale=(float)(i+1)/num_kernels;
			scale=glm::mix(0.1f, 1.0f, scale*scale);
			m_kernel[i]*=scale;
		}
	}

	void SSAO::uploadKernel()
	{
		for(uint32_t i=0; i<m_kernel.size(); i++)
		{
			std::string uniform="kernel["+std::to_string(i)+"]";
			m_programSSAO->setUniform(uniform, m_kernel[i]);
		}
	}

	void SSAO::setSampleRadius(int32_t r)
	{
		m_programSSAO->setUniform("radius", r);
	}
}