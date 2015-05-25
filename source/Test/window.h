#ifndef __R3D_TEST_WINDOW_H_
#define __R3D_TEST_WINDOW_H_

namespace Rocket
{
	namespace Core
	{
		class Context;
		class ElementDocument;
	}
}

namespace R3DRocket
{
	class Window
	{
	public:
		Window(Rocket::Core::Context *context, const char *filename);
		virtual ~Window();

		void show();
		void hide();
	protected:
		Rocket::Core::ElementDocument *m_document;
	};

	class DebugWindow: public Window
	{
	public:
		DebugWindow(Rocket::Core::Context *context);
	};
};

#endif