#include "TextureManagerWindow.h"
#include "../../Graphics/TextureAsset.h"
#include "../Editor.h"

RTTI_DEFINE_CLASS(editor::TextureManagerWindow,
	{
		RTTI_DISPLAY_NAME("Texture Manager");
		RTTI_BASE(EditorWindow);
	});

namespace editor
{
	TextureManagerWindow::TextureManagerWindow()
	{

	}
	void TextureManagerWindow::Draw()
	{
		bool open = ImGui::Begin(IMGUI_ID("Texture Manager"), &_isOpen,
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_AlwaysVerticalScrollbar
		);

		if (!open)
		{
			ImGui::End();
			return;
		}


		for (auto& it : rendering::textureManager)
		{
			const float iconSize = 80.0f;
			ImGui::ImageButton(it.second->texture, { iconSize, iconSize });

			if (ImGui::BeginDragDropSource())
			{
				it.second->CreateDragPayload();
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
				{
					editor::editor->editorContext.Select(it.second);
				}
			}

			ImGui::SameLine();
			ImGui::Text(it.second->GetName().c_str());




		}

		ImGui::End();
	}
}