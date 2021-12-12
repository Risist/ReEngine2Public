#include "../../Common/Settings.h"

#include "ImGuiDemoWindow.h"

RTTI_DEFINE_CLASS(editor::ImGuiDemoWindow,
	{
		RTTI_BASE(editor::EditorWindow);
	});

namespace editor
{
	void ImGuiDemoWindow::Draw()
	{
		ImGui::ShowDemoWindow(&_isOpen);
	}
}