//#include "TextureAsset.h"
#include "SpriteAtlasAsset.h"
#include "../Editor/ImGuiManager.h"
#include <fstream>
#include <iostream>
#include "../Editor/Windows/ContentBrowserWindow.h"
#include "../Editor/Editor.h"
#include "Imgui/imgui_internal.h"


RTTI_DEFINE_ASSET(rendering::TextureAsset,
	{
		RTTI_BASE(NativeAsset);
		RTTI_DISPLAY_NAME("Texture");

		RTTI_FILE_EXTENSION(L".re_texture");
		RTTI_NATIVE_FILE_EXTENSION(L".bmp");
		RTTI_NATIVE_FILE_EXTENSION(L".dds");
		RTTI_NATIVE_FILE_EXTENSION(L".jpg");
		RTTI_NATIVE_FILE_EXTENSION(L".png");
		RTTI_NATIVE_FILE_EXTENSION(L".tga");
		RTTI_NATIVE_FILE_EXTENSION(L".psd");

		RTTI_PROPERTY(smooth);
		RTTI_PROPERTY_SETTER(smooth, SetSmooth);
		RTTI_PROPERTY(repeated);
		RTTI_PROPERTY_SETTER(repeated, SetRepeated);
	});

namespace rendering
{
	engine::TAssetManager<TextureAsset>& textureManager = engine::GetAssetManager<TextureAsset>();

	void TextureAsset::CreateDragPayload() const
	{
		ImGui::Image(texture, { 20,20 });
		ImGui::SameLine();
		CreateDefaultDragPayload(ASSET_PAYLOAD_CODE);
	}
	bool TextureAsset::DrawContentBrowserElement(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath, const editor::ContentBrowserAssetDrawingSettings& assetDrawingContext)
	{
		ImGui::ImageButton(texture, { assetDrawingContext.thumbnailSize - 20, assetDrawingContext.thumbnailSize -20 });
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

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		ImGui::Button(IMGUI_ID(GetType()->GetDisplayName()), { assetDrawingContext.thumbnailSize + 15 , 20 });
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
		
		ImGui::Text(GetName().c_str());
		return contextMenuOpened;
	}
	
	void TextureAsset::ContextMenuContentBrowser(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath)
	{
		if (ImGui::BeginMenu("Create"))
		{
			auto spriteType = rtti::GetAssetDescriptor<SpriteAsset>();
			auto spritePath = std::filesystem::path((currentPath.parent_path() / currentPath.filename()).string() + spriteType->GetFileExtension());
			bool isSpriteAlreadyCreated = std::filesystem::exists(spritePath);
			if (ImGui::MenuItem(IMGUI_ID("Sprite Asset"), nullptr, false, !isSpriteAlreadyCreated))
			{
				auto sprite = engine::GetAssetManager<SpriteAsset>().CreateAsset();
				sprite->textureAssetSource = textureManager.FindAssetByGuid(GetGuid());
				sprite->SetName(GetName());
				sprite->size = sf::Vector2i(texture.getSize().x, texture.getSize().y);

				rtti::SerializeToFile(spritePath, *sprite.get());
			}
			
			
			auto atlasType = rtti::GetAssetDescriptor<SpriteAtlasAsset>();
			auto atlasPath = std::filesystem::path((currentPath.parent_path() / currentPath.filename()).string() + atlasType->GetFileExtension());
			bool isAtlasAlreadyCreated = std::filesystem::exists(atlasPath);
			if (ImGui::MenuItem(IMGUI_ID("Sprite Atlass"), nullptr, false, !isAtlasAlreadyCreated))
			{
				auto atlas = engine::GetAssetManager<SpriteAtlasAsset>().CreateAsset();
				auto texture = textureManager.FindAssetByGuid(GetGuid());
				atlas->textureAssetSource = texture;
				atlas->SetName(GetName());
				atlas->partition = { 1,1 };
				atlas->PrepareAtlassSprites();

				
				rtti::SerializeToFile(atlasPath, *atlas.get());
			}

			ImGui::EndMenu();
		}
	}
	
	void TextureAsset::DeserializeNative(const std::filesystem::path& path)
	{
		const std::filesystem::path& npath = path.parent_path() / GetNativeRelativePath();
		texture.loadFromFile(npath.string());
		texture.setSmooth(smooth);
		texture.setRepeated(repeated);
	}
}

namespace editor
{
	bool TextureAssetDrawer::DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		static const sf::Texture emptyTexture;
		const sf::Texture* currentItemTexture = &emptyTexture;
		const char* currentItemName = "None";
		const char* popupId = IMGUI_ID(currentItemName);

		std::weak_ptr<rendering::TextureAsset>* varriable = (std::weak_ptr<rendering::TextureAsset>*)context.objectStart;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
		ImGui::Text(context.GetDisplayName());
		afterLabelCall();

		//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
		if (!varriable->expired())
		{
			auto lockedVarriable = varriable->lock();
			currentItemName = lockedVarriable->GetName().c_str();
			popupId = IMGUI_ID(currentItemName);
			currentItemTexture = &lockedVarriable->texture;
		}

		ImGui::SameLine();
		if (ImGui::ImageButton(*currentItemTexture, { 30,30 }))
		{
			ImGui::OpenPopup(popupId);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(rendering::TextureAsset::ASSET_PAYLOAD_CODE))
			{
				Guid textureGuid = *((Guid*)payload->Data);

				*varriable = rendering::textureManager.FindAssetByGuid(textureGuid);
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

			if (ImGui::ImageButton(*currentItemTexture, { iconSize, iconSize }))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			ImGui::Text((std::string("Current: ") + currentItemName).c_str());

			if (!varriable->expired())
			{
				if (ImGui::ImageButton(sf::Texture(), { iconSize, iconSize }))
				{
					*varriable = std::weak_ptr<rendering::TextureAsset>();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				ImGui::Text("None");
			}

			ImGui::NewLine();

			for (auto& it : engine::GetAssetManager<rendering::TextureAsset>())
			{
				if (!searchString.empty() && it.second->GetName().find(searchString) == std::string::npos)
					continue;

				if (ImGui::ImageButton(it.second->texture, { iconSize, iconSize }))
				{
					*varriable = it.second;
					ImGui::CloseCurrentPopup();
				}


				ImGui::SameLine();
				ImGui::Text(it.second->GetName().c_str());
			}

			ImGui::EndPopup();
		}

		return false;
	}
}


namespace rtti
{
	bool TypeDrawerInfoImage::DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		auto texture = (std::weak_ptr<rendering::TextureAsset>*)textureProperty->GetPropertyAddres(context.objectStart);
		float aspectRatio = 1;

		sf::Sprite sprite;
		if (texture->expired() == false)
		{
			auto ts = texture->lock();;
			sprite.setTexture(ts->texture);

			aspectRatio = ts->GetAspectRatio();
		}

		if (positionProperty != nullptr && sizeProperty != nullptr)
		{
			auto position = (sf::Vector2i*)positionProperty->GetPropertyAddres(context.objectStart);
			auto size = (sf::Vector2i*)sizeProperty->GetPropertyAddres(context.objectStart);

			sprite.setTextureRect(sf::IntRect(*position, *size));
		}

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
		
		// TODO change to clamp function
		float tmpImageSize = ImGui::GetWindowContentRegionWidth() * imageSize;
		tmpImageSize = tmpImageSize < 200 ? 200 : tmpImageSize;
		tmpImageSize = tmpImageSize > 500 ? 500 : tmpImageSize;
		
		ImGui::Image(sprite, sf::Vector2f(tmpImageSize, tmpImageSize* aspectRatio), sf::Color::White, sf::Color::Blue);
		
		return false;
	}

}
