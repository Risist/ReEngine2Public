#include "../../Common/Settings.h"

#include "SkeletalMesheditorWindow.h"
#include "../ImGuiManager.h"


RTTI_DEFINE_CLASS(editor::SkeletalMesheditorWindow,
	{
		RTTI_BASE(editor::EditorWindow);
		RTTI_DISPLAY_NAME("Skeletal Mesh Editor");
	});

namespace editor
{
	SkeletalMesheditorWindow::SkeletalMesheditorWindow()
	{

	}

	void SkeletalMesheditorWindow::Draw()
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