#include "Asset.h"
#include "AssetManager.h"

#include <Engine/Editor/ImGuiManager.h>
#include <Engine/Editor/Editor.h>
#include <Engine/Editor/Windows/ContentBrowserWindow.h>

RTTI_DEFINE_ASSET(Asset,
	{
		RTTI_PROPERTY(_guid)
			.SetOverrideTypeDrawer<editor::VoidDrawer>()
			.SetDisplayName("guid");
		RTTI_PROPERTY(_name)
			.SetOverrideTypeDrawer<editor::VoidDrawer>()
			.SetDisplayName("name");
	});

RTTI_DEFINE_ASSET(NativeAsset,
	{
		RTTI_BASE(Asset);  

		RTTI_PROPERTY(_nativeRelativePath)
			.SetOverrideTypeDrawer<editor::VoidDrawer>()
			.SetDisplayName("nativeRelativePath");
	});

bool Asset::DrawContentBrowserElement(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath, const editor::ContentBrowserAssetDrawingSettings& assetDrawingContext)
{
	ImGui::ImageButton(assetDrawingContext.fileTexture, { assetDrawingContext.thumbnailSize, assetDrawingContext.thumbnailSize });
	if (ImGui::BeginDragDropSource())
	{
		CreateDragPayload();
		ImGui::EndDragDropSource();
	} 


	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			editor::editor->editorContext.Select(_this);
		}
	}

	bool contextMenuOpened = false;
	if (ImGui::BeginPopupContextItem())
	{
		ContextMenuContentBrowser(_this, currentPath);
		ImGui::EndPopup();
		contextMenuOpened = true;
	}
	
	ImGui::Text(GetName().c_str());
	return contextMenuOpened;
}

void Asset::ContextMenuContentBrowser(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath)
{
	if (ImGui::BeginMenu(""))
	{

		ImGui::EndMenu();
	}
}

void Asset::CreateDefaultDragPayload(const char* assetPayloadCode) const
{
	ImGui::Text(GetName().c_str());
	Guid guid = GetGuid();
	ImGui::SetDragDropPayload(assetPayloadCode, &guid, sizeof(guid));
}
