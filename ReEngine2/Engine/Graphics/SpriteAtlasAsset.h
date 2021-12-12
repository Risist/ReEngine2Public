#pragma once
#include "SpriteAsset.h"

namespace rendering
{
	class SpriteAtlasAsset : public Asset
	{
		RTTI_DECLARE_ASSET(SpriteAtlasAsset);
	public:
		static constexpr const char* ASSET_PAYLOAD_CODE = "Asset_Sprite_Atlas";

		virtual bool DrawContentBrowserElement(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath, const editor::ContentBrowserAssetDrawingSettings& assetDrawingContext) override;
		virtual void CreateDragPayload() const override;

		void PrepareAtlassSprites();

		void SetPartition(sf::Vector2u partition)
		{
			RE_ASSERT(textureAssetSource.expired() == false);

			auto& texture = textureAssetSource.lock()->texture;
			auto size = texture.getSize();


			bool changed = this->partition != partition;
			this->partition = partition;

			this->partition.x = this->partition.x < 1 ? 1 : this->partition.x;
			this->partition.x = this->partition.x > size.x ? size.x : this->partition.x;

			this->partition.y = this->partition.y < 1 ? 1 : this->partition.y;
			this->partition.y = this->partition.y > size.y ? size.y : this->partition.y;

			if (changed)
				PrepareAtlassSprites();

		}

		void SetPositionOffset(sf::Vector2u positionOffset)
		{
			RE_ASSERT(textureAssetSource.expired() == false);

			bool changed = this->positionOffset != positionOffset;
			this->positionOffset = positionOffset;

			if (changed)
				PrepareAtlassSprites();
		}

		std::weak_ptr<TextureAsset> textureAssetSource;
		std::vector<std::weak_ptr<SpriteAsset>> sprites;
		sf::Vector2u partition;
		sf::Vector2u positionOffset;
	};

	extern engine::TAssetManager<rendering::SpriteAtlasAsset>& spriteAtlassManager;
}