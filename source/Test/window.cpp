#include "window.h"
#include <Rocket/Core.h>

namespace R3DRocket
{
	Window::Window(Rocket::Core::Context *context, const char *filename)
	: m_document(context->LoadDocument(filename))
	{

	}

	Window::~Window()
	{
		m_document->Close();
	}

	void Window::show()
	{
		m_document->Show();
	}
	void Window::hide()
	{
		m_document->Hide();
	}

	/////////////DEBUG WINDOW

	DebugWindow::DebugWindow(Rocket::Core::Context *context)
	: Window(context, "./assets/debug.rml")
	{

	}
}