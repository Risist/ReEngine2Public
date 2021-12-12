#include "../../Common/Settings.h"
#include "LevelEditorWindow.h"
#include "../ImGuiManager.h"

RTTI_DEFINE_CLASS(editor::LevelEditorWindow,
	{
		RTTI_BASE(editor::EditorWindow);
		RTTI_DISPLAY_NAME("LevelEditor");
	});
namespace editor
{
	LevelEditorWindow::LevelEditorWindow()
	{

	}

	void LevelEditorWindow::Draw()
	{
		bool open = ImGui::Begin(IMGUI_ID("Skeletal Mesh Editor"), &_isOpen,
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_AlwaysVerticalScrollbar
		);

		if (!open)
		{
			ImGui::End();
			return;
		}


		ImGui::End();
	}
}