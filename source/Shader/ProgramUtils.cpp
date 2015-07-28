#include <r3d/Shader/Program.hpp>
#include <r3d/Shader/Shader.hpp>
#include <r3d/Core/Core.hpp>


namespace r3d
{
	ProgramPtr MakeShaderProgram(Engine *engine, const char *vsource, const char *fsource)
	{
		auto program=engine->newProgram();
		auto vs=engine->newShader(ST_VERTEX_SHADER);
		auto fs=engine->newShader(ST_FRAGMENT_SHADER);
		vs->source(vsource);
		fs->source(fsource);
		vs->compile();
		fs->compile();
		program->attachShader(vs);
		program->attachShader(fs);
		program->link();

		return program;
	}

	ProgramPtr MakeShaderProgram(Engine *engine, const char *vsource, const char *gsource, const char *fsource)
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
		program->attachShader(gs);
		program->attachShader(fs);
		program->link();

		return program;
	}

	ProgramPtr MakeShaderProgram(Engine *engine, const char *csource)
	{
		auto program=engine->newProgram();
		auto cs=engine->newShader(ST_COMPUTE_SHADER);
		cs->source(csource);
		cs->compile();
		program->attachShader(cs);
		program->link();

		return program;
	}
}