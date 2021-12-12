#pragma once
#include "TextureAsset.h"

namespace rendering
{
	class SpriteAsset : public Asset
	{
		RTTI_DECLARE_ASSET(SpriteAsset);
	public:
		static constexpr const char* ASSET_PAYLOAD_CODE = "Asset_Sprite";

		virtual void CreateDragPayload() const override;

		virtual bool DrawContentBrowserElement(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath, const editor::ContentBrowserAssetDrawingSettings& assetDrawingContext) override;

		std::weak_ptr<TextureAsset> textureAssetSource;
		sf::Vector2i position;
		sf::Vector2i size;
		bool perserveAspectRatio = true;
	};


	extern engine::TAssetManager<rendering::SpriteAsset>& spriteManager;
}

namespace editor
{
	// drawer for std::weak_ptr<SpriteAsset>
	class SpriteAssetDrawer : public rtti::TypeDrawerBase
	{
	public:
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override;
	};
}

RTTI_DEFINE_EXTERNAL_CLASS(std::weak_ptr<rendering::SpriteAsset>,
	{
		RTTI_DRAWER(editor::SpriteAssetDrawer);
		RTTI_SERIALIZER(rtti::TTypeSerializerAsset<rendering::SpriteAsset>);

		RTTI_DISPLAY_NAME("Sprite");
	});