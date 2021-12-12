#include "SpriteAsset.h"
#include "Imgui/imgui_internal.h"
#include "../Editor/Windows/ContentBrowserWindow.h"
#include "../Editor/Editor.h"

RTTI_DEFINE_ASSET(rendering::SpriteAsset,
	{
		RTTI_BASE(Asset);

		RTTI_FILE_EXTENSION(L".re_sprite");
		RTTI_DISPLAY_NAME("Sprite");


		RTTI_PROPERTY(textureAssetSource)
			.SetOverrideTypeDrawer<editor::VoidDrawer>(); // TODO readonly, or custom display ?
		RTTI_PROPERTY(position);
		RTTI_PROPERTY(size);
		RTTI_PROPERTY(perserveAspectRatio);


		RTTI_IMAGE("textureAssetSource", 0.8f, "position", "size");
	});

namespace rendering
{
	engine::TAssetManager<SpriteAsset>& spriteManager = engine::GetAssetManager<SpriteAsset>();

	void SpriteAsset::CreateDragPayload() const
	{
		if (!textureAssetSource.expired())
		{
			sf::Sprite sp;
			sp.setTexture(textureAssetSource.lock()->texture);
			sp.setTextureRect(sf::IntRect(position, size));

			ImGui::Image(sp, { 20,20 });
			ImGui::SameLine();
		}
		CreateDefaultDragPayload(ASSET_PAYLOAD_CODE);
	}
	bool SpriteAsset::DrawContentBrowserElement(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath, const editor::ContentBrowserAssetDrawingSettings& assetDrawingContext)
	{
		auto textureAssetLocked = textureAssetSource.lock();
		ImGui::ImageButton(textureAssetLocked->texture, { assetDrawingContext.thumbnailSize - 20, assetDrawingContext.thumbnailSize - 20 });
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

		//ImGui::SameLine();
		//ImGui::ImageButton(editor::ContentBrowserWindow::assetDrawingSettings.arrowTexture, { 15, 15 });

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		ImGui::Button(IMGUI_ID(GetType()->GetDisplayName()), { assetDrawingContext.thumbnailSize + 15 , 20 });
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();

		ImGui::Text(GetName().c_str());
		return contextMenuOpened;
	}
}

namespace editor
{
	// TODO when draging texture in place of sprite try to find related sprite
	// if this operation fails create sprite based on the texture and then assign it
	bool SpriteAssetDrawer::DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		static const sf::Texture emptyTexture;
		sf::Sprite currentItemSprite;
		currentItemSprite.setTexture(emptyTexture);
		const char* currentItemName = "None";
		const char* popupId = IMGUI_ID(currentItemName);

		std::weak_ptr<rendering::SpriteAsset>* varriable = (std::weak_ptr<rendering::SpriteAsset>*)context.objectStart;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
		ImGui::Text(context.GetDisplayName());
		afterLabelCall();

		//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
		if (!varriable->expired())
		{
			auto lockedVarriable = varriable->lock();
			if (lockedVarriable->textureAssetSource.expired() == false)
			{
				auto lockedTexture = lockedVarriable->textureAssetSource.lock();

				currentItemName = lockedVarriable->GetName().c_str();
				popupId = IMGUI_ID(currentItemName);
				currentItemSprite.setTexture(lockedTexture->texture);
				currentItemSprite.setTextureRect(sf::IntRect(lockedVarriable->position, lockedVarriable->size));
			}
		}

		ImGui::SameLine();
		if (ImGui::ImageButton(currentItemSprite, { 30,30 }))
		{
			ImGui::OpenPopup(popupId);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(rendering::SpriteAsset::ASSET_PAYLOAD_CODE))
			{
				Guid textureGuid = *((Guid*)payload->Data);

				*varriable = rendering::spriteManager.FindAssetByGuid(textureGuid);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		ImGui::Text(currentItemName);

		if (ImGui::BeginPopup(popupId))
		{
			const float iconSize = 60.0f;

			static char searchBuffer[64];
			memset(searchBuffer, 0, sizeof(searchBuffer));
			ImGui::Text("Search: ");
			ImGui::SameLine();
			ImGui::InputText(IMGUI_ID(""), searchBuffer, sizeof(searchBuffer));

			std::string searchString = searchBuffer;

			if (ImGui::ImageButton(currentItemSprite, { iconSize, iconSize }))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			ImGui::Text((std::string("Current: ") + currentItemName).c_str());

			if (!varriable->expired())
			{
				if (ImGui::ImageButton(sf::Texture(), { iconSize, iconSize }))
				{
					*varriable = std::weak_ptr<rendering::SpriteAsset>();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				ImGui::Text("None");
			}

			ImGui::NewLine();

			for (auto& it : engine::GetAssetManager<rendering::SpriteAsset>())
			{
				if (!searchString.empty() && it.second->GetName().find(searchString) == std::string::npos)
					continue;

				if (it.second->textureAssetSource.expired() == false)
				{
					auto lockedTexture = it.second->textureAssetSource.lock();

					sf::Sprite itemSprite;
					itemSprite.setTexture(lockedTexture->texture);
					itemSprite.setTextureRect(sf::IntRect(it.second->position, it.second->size));

					if (ImGui::ImageButton(itemSprite, { iconSize, iconSize }))
					{
						*varriable = it.second;
						ImGui::CloseCurrentPopup();
					}

					ImGui::SameLine();
					ImGui::Text(it.second->GetName().c_str());
				}
			}

			ImGui::EndPopup();
		}

		return false;
	}
}