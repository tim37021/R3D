#include <r3d/Window/ContextWindow.hpp>

namespace r3d
{
	std::map<ContextOptions, std::string> ContextWindow::WindowCreationHint=
	{
		{CO_RESIZABLE, "TRUE"},
		{CO_VISIBLE, "TRUE"},

		{CO_RED_BITS, "8"},
		{CO_GREEN_BITS, "8"},
		{CO_BLUE_BITS, "8"},
		{CO_DEPTH_BITS, "24"},
		{CO_STENCIL_BITS, "8"},

		{CO_GL_VERSION_MAJOR, "3"},
		{CO_GL_VERSION_MINOR, "3"}
	};

	ContextWindow::ContextWindow(uint32_t width, uint32_t height, const std::string &title):
		m_mouse(nullptr), m_keybd(nullptr), m_width(width), m_height(height), 
		m_title(title), m_el(nullptr)
	{

	}

	ContextWindow::~ContextWindow()
	{
		delete m_mouse;
		delete m_keybd;
	}

	void ContextWindow::SetHint(ContextOptions op, bool value)
	{
		auto it=WindowCreationHint.begin();
		if((it=WindowCreationHint.find(op))!=WindowCreationHint.end())
			it->second=value?"TRUE":"FALSE";
	}
	void ContextWindow::SetHint(ContextOptions op, int32_t value)
	{
		auto it=WindowCreationHint.begin();
		if((it=WindowCreationHint.find(op))!=WindowCreationHint.end())
			it->second=std::to_string(value);
	}
}