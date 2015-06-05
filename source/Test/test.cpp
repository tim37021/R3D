#include <r3d/r3d.hpp>

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

static const char *compute_shader=
	"#version 430\n"
	"uniform float roll;"
	"writeonly uniform image2D destTex;"
	"layout (local_size_x = 16, local_size_y = 16) in;"
	"void main() {"
	"ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);"
	"float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);"
	"float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 + roll)*0.5;"
	"imageStore(destTex, storePos, vec4(1.0-globalCoef*localCoef));"
	"}";

static ProgramPtr MakeShaderProgram(const Engine *engine, const char *vsource,
	const char *gsource, const char *fsource)
{
	auto program=engine->newProgram();
	auto vs=engine->newShader(ST_VERTEX_SHADER);
	auto gs=engine->newShader(ST_GEOMETRY_SHADER);
	auto fs=engine->newShader(ST_FRAGMENT_SHADER);
	vs->source(vsource);
	gs->source(gsource);
	fs->source(fsource);
	vs->compile();
	gs->compile();
	fs->compile();
	program->attachShader(vs);
	program->attachShader(fs);
	program->attachShader(gs);
	program->link();

	return program;
}

static ProgramPtr MakeShaderProgram(const Engine *engine, const char *csource)
{
	auto program=engine->newProgram();
	auto cs=engine->newShader(ST_COMPUTE_SHADER);
	cs->source(csource);
	cs->compile();
	program->attachShader(cs);
	program->link();

	return program;
}

int main(int argc, char *argv[])
{
	if(argc!=2) return 1;
	Engine *engine = new r3d::Engine(r3d::RA_OPENGL_4_3);
	ContextWindow *cw=engine->newContextWindow(800, 600, "R3D Example");
	Renderer *renderer=engine->getRenderer();

	// In order to load/create texture, we need texture manager
	TextureManager *tMgr=cw->getTextureManager();
	// Or you can use Texture2D *
	ColorTexture2D *text=tMgr->registerColorTexture2D(argv[1]);

	// We need a dummy vertex array to do attributeless rendering
	VertexArray *vao=cw->getVertexArrayManager()->registerVertexArray("DUMMY");

	ProgramPtr program=MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader);
	ProgramPtr program_cs=MakeShaderProgram(engine, compute_shader);

	while(!cw->isCloseButtonClicked())
	{
		renderer->clear();

		program_cs->use();
		program_cs->setUniform("roll", (float)engine->getTime() * 0.01f);
		program_cs->setUniform("destTex", 0);
		text->bindImage(0, 0, AL_WRITE_ONLY);
		glDispatchCompute(512/16, 512/16, 1);


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