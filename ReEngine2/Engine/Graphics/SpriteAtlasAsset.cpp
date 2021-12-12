#include "SpriteAtlasAsset.h"
#include "../Editor/Windows/ContentBrowserWindow.h"
#include "../Editor/Editor.h"
#include "Imgui/imgui_internal.h"

RTTI_DEFINE_ASSET(rendering::SpriteAtlasAsset,
	{
		RTTI_BASE(Asset);

		RTTI_FILE_EXTENSION(L".re_sprite_atlas");
		RTTI_DISPLAY_NAME("Sprite Atlass");


		RTTI_PROPERTY(textureAssetSource)
			.SetOverrideTypeDrawer<editor::VoidDrawer>(); // TODO readonly, or custom display ?

		RTTI_PROPERTY(positionOffset);
		RTTI_PROPERTY_SETTER(positionOffset, SetPositionOffset);

		RTTI_PROPERTY(partition);
		RTTI_PROPERTY_SETTER(partition, SetPartition);

		RTTI_PROPERTY(sprites);
	});

namespace rendering
{
	engine::TAssetManager<SpriteAtlasAsset>& spriteAtlassManager = engine::GetAssetManager<SpriteAtlasAsset>();

	bool SpriteAtlasAsset::DrawContentBrowserElement(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath, const editor::ContentBrowserAssetDrawingSettings& assetDrawingContext)
	{
		bool contextMenuOpened = false;
		static bool bSpritesExtended = false;

		if (ImGui::BeginTable(IMGUI_ID("dsada"), 2))//, ImGuiTableFlags_::))
		{
			ImGui::TableSetupColumn("ad", ImGuiTableColumnFlags_WidthFixed, assetDrawingContext.thumbnailSize - 20);
			ImGui::TableSetupColumn("ads", ImGuiTableColumnFlags_WidthFixed, 20);

			ImGui::TableNextColumn();

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

			if (ImGui::BeginPopupContextItem())
			{
				ContextMenuContentBrowser(_this, currentPath);
				ImGui::EndPopup();
				contextMenuOpened = true;
			}

			ImGui::TableNextColumn();

			if (ImGui::ImageButton(editor::ContentBrowserWindow::assetDrawingSettings.arrowTexture, { 17.5f, 17.5f }))
			{
				bSpritesExtended = !bSpritesExtended;
			}

			if (ImGui::ImageButton(editor::ContentBrowserWindow::assetDrawingSettings.arrowTexture, { 17.5f, 17.5f }))
			{

			}

			ImGui::EndTable();
		}

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		ImGui::Button(IMGUI_ID(GetType()->GetDisplayName()), { assetDrawingContext.thumbnailSize + 15 , 20 });
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();


		ImGui::Text(GetName().c_str());

		if (bSpritesExtended)
		{

			for (auto it : sprites)
			{
				auto locked = it.lock();
				auto tsLocked = locked->textureAssetSource.lock();
				ImGui::TableNextColumn();
				sf::Sprite sp;
				sp.setTexture(tsLocked->texture);
				sp.setTextureRect(sf::IntRect(locked->position, locked->size));
				float aspect = (float)locked->size.x / locked->size.y;
				ImGui::ImageButton(sp, { (assetDrawingContext.thumbnailSize - 20) * aspect, (assetDrawingContext.thumbnailSize - 20) }, 0);
				//ImGui::InvisibleButton(IMGUI_ID(""), { 20,20 });

				if (ImGui::BeginDragDropSource())
				{
					locked->CreateDragPayload();
					ImGui::EndDragDropSource();
				}
			}

		}

		return contextMenuOpened;
	}

	void SpriteAtlasAsset::CreateDragPayload() const
	{
		if (textureAssetSource.expired())
		{
			ImGui::Image(textureAssetSource.lock()->texture, { 20,20 });
			ImGui::SameLine();
		}
		CreateDefaultDragPayload(ASSET_PAYLOAD_CODE);
	}
	void SpriteAtlasAsset::PrepareAtlassSprites()
	{
		RE_ASSERT(textureAssetSource.expired() == false);


		auto& texture = textureAssetSource.lock()->texture;
		auto size = texture.getSize();

		// TODO remove all previous sprites from asset manager
		// TODO prepare assets on load
		// TODO some function runing on load? 
		//		or setter function should be called 
		// TODO Draw sprite images
		for (auto& it : sprites)
		{
			spriteManager.RemoveAsset(it.lock()->GetGuid());
		}
		sprites.clear();
		sprites.shrink_to_fit();
		sprites.resize(partition.x * partition.y);

		int i = 0;
		for (int y = 0; y < partition.y; ++y)
			for (int x = 0; x < partition.x; ++x)
			{
				auto sprite = spriteManager.CreateAsset();
				sprite->textureAssetSource = textureAssetSource;
				sprite->SetName(GetName()); // TODO add x and y 

				sprite->size = sf::Vector2i(
					(size.x - positionOffset.x) / partition.x,
					(size.y - positionOffset.y) / partition.y
				);
				sprite->position.x = sprite->size.x * x + positionOffset.x;
				sprite->position.y = sprite->size.y * y + positionOffset.y;

				sprites[i++] = sprite;
			}


	}
}