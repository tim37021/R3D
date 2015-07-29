#include <r3d/r3d.hpp>
#include <string>
#include <fstream>
#include <memory>

#define toRadian(d) (d*3.1415f/180)

using namespace r3d;

static const char *vertex_shader=
	"#version 330\n"
	"void main(){}";

static const char *geometry_shader=
	"#version 330\n"
	"layout(points) in;\n"
	"layout(triangle_strip, max_vertices=6) out;\n"
	"void main(){"
	//First triangle
	"gl_Position=vec4(-1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n"
	//Second triangle
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
	"layout(location=0) out vec4 color;"
	"uniform sampler2D text;"
	"uniform vec2 viewport;"
	"void main(){"
	"vec2 vTexCoord=gl_FragCoord.xy/viewport;"
	"color=texture(text, vTexCoord);"
	"}";

static std::string readfile(const char *filename)
{
	std::ifstream ifs(filename);
	if(!ifs)
		exit(EXIT_FAILURE);
	return std::string( (std::istreambuf_iterator<char>(ifs)),
                       (std::istreambuf_iterator<char>()));
}

int main(int argc, char *argv[])
{
	Engine *engine = new r3d::Engine(RA_OPENGL_4_3);
	ContextWindow *cw=engine->newContextWindow(800, 800, "R3D Example");
	Renderer *renderer=engine->getRenderer();

	// In order to load/create texture, we need texture manager
	TextureManager *tMgr=cw->getTextureManager();
	// Or you can use Texture2D *
	ColorTexture2D *text=tMgr->registerColorTexture2D("white", 800, 800, PF_BGRA);
	ColorTexture2D *rndText=tMgr->registerColorTexture2D("noise.png");

	// We need a dummy vertex array to do attributeless rendering
	VertexArray *vao=cw->getVertexArrayManager()->registerVertexArray("DUMMY");

	ProgramPtr program=MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader);
	ProgramPtr program_cs=MakeShaderProgram(engine, readfile("./shaders/pathtracer.cs").c_str());

	std::shared_ptr<Camera> fps(new FPSCamera(cw, 45.0f, glm::vec3(5.0f, 0.0f, 0.0f)));

	renderer->enableDepthTest(false);
	renderer->enableBlending(true, BP_ONE, BP_ONE, BF_ADD);

	cw->getMouse()->setPos(cw->getWidth()/2, cw->getHeight()/2);
	fps->update(engine->getTime());

	while(!cw->isCloseButtonClicked())
	{
		renderer->clear();
		fps->update(engine->getTime());

		rndText->bind(1);
		program_cs->setUniform("uTexRandom", 1);
		program_cs->setUniform("randFactor", {(float)rand()/RAND_MAX, (float)rand()/RAND_MAX});
		program_cs->setUniform("eyePos", fps->getPos());
		program_cs->setUniform("eyeDir", fps->getDir());
		program_cs->setUniform("eyeUp", fps->getUp());
		program_cs->setUniform("near", fps->getNear());
		program_cs->setUniform("fov", toRadian(fps->getFov()));
		program_cs->setUniform("destTex", 0);
		text->bindImage(0, 0, AL_READ_WRITE);
		program_cs->dispatchCompute(800, 800, 1);


		// Because we render without attribute, and generate quad with geometry shader
		// take care of parameter here, make sure geometry shader runs only one time
		program->use();
		text->bind(0);
		program->setUniform("text", 0);
		program->setUniform("viewport", {cw->getWidth(), cw->getHeight()});
		renderer->drawArrays(program.get(), vao, PT_POINTS, 1);

		cw->pollInput();
		cw->swapBuffers();
	}

	delete engine;
	return 0;
}