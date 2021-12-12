#include "../../Common/Settings.h"
#include "AssetInspectorWindow.h"

#include "../../../Imgui/imgui.h"
#include "../../../Imgui/imgui-SFML.h"

#include "../ImGuiManager.h"
#include "../Editor.h"

#include "../ImGuiManager.h"
#include "../Editor.h"

#include <Engine/AssetManagament/Asset.h>


RTTI_DEFINE_CLASS(editor::AssetInspectorWindow,
	{
		RTTI_BASE(editor::EditorWindow);
		RTTI_DISPLAY_NAME("AssetInspector");
	});

namespace editor
{
	void AssetInspectorWindow::Draw()
	{
		bool open = ImGui::Begin(IMGUI_ID("Asset Inspector"), &_isOpen,
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_AlwaysVerticalScrollbar
		);

		if (!open)
		{
			ImGui::End();
			return;
		}

		if (editor::editor->editorContext.HasAnyAssetChoosen())
		{
			auto asset = editor::editor->editorContext.GetAsset().lock();

			ImGuiPushID id(GetGuid());
			asset->ShowInInspector(editor::editor->editorContext);
		}

		ImGui::End();
	}
}