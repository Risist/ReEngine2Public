#pragma once
#include "../Common/Settings.h"

#include <Engine/AssetManagament/Asset.h>
#include <Engine/AssetManagament/AssetManager.h>

#include <fstream>
#include <iostream>
#include "yaml-cpp/yaml.h"
#include <filesystem>
#include <unordered_map>



namespace rendering
{
	// There are two ways to handle native Assets
	// first - to create two types - assetSource - the native part and assetInstance - engine part with settings
	
	// the second one is to have only one type - the engine one. Native version will be handled by the asset
	// asset manager upon seeing siutable extension will generate right asset description file with all the settings of the asset

	class TextureAsset : public NativeAsset
	{
		RTTI_DECLARE_ASSET(TextureAsset);
	public:
		static constexpr const char* ASSET_PAYLOAD_CODE = "Asset_Texture_Source";

		virtual void CreateDragPayload() const override;

		virtual bool DrawContentBrowserElement(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath, const editor::ContentBrowserAssetDrawingSettings& assetDrawingContext) override;
		virtual void ContextMenuContentBrowser(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath);

		virtual void DeserializeNative(const std::filesystem::path& path) override;

		float GetAspectRatio()
		{
			return (float)texture.getSize().y / texture.getSize().x;
		}

		void SetSmooth(bool s)
		{
			smooth = s;
			texture.setSmooth(s);
			// TODO add to unsaved assets
		}

		void SetRepeated(bool s)
		{
			repeated = s;
			texture.setRepeated(s);
			// TODO add to unsaved assets
		}

		sf::Texture texture; 
		bool smooth;
		bool repeated;
	};

	extern engine::TAssetManager<rendering::TextureAsset>& textureManager;
}

namespace editor
{
	// drawer for std::weak_ptr<TextureAsset>
	class TextureAssetDrawer : public rtti::TypeDrawerBase
	{
	public:
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override;
	};
}


namespace rtti
{
	class TypeDrawerInfoImage : public TypeDrawerBase
	{
	public:
		TypeDrawerInfoImage(
			rtti::ClassType* type = nullptr,
			const char* texturePropertyName = "",
			float imageSize = -1,
			const char* positionPropertyName = "",
			const char* sizePropertyName = ""
		)
			: textureProperty(type->FindPropertyByNativeName(texturePropertyName))
			, positionProperty(type->FindPropertyByNativeName(positionPropertyName))
			, sizeProperty(type->FindPropertyByNativeName(sizePropertyName))
			, imageSize(imageSize)
		{
		}

		virtual bool DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const override;

		rtti::Property* textureProperty;
		rtti::Property* positionProperty;
		rtti::Property* sizeProperty;
		float imageSize;
	};


#define RTTI_IMAGE(...)																\
	{																				\
		static rtti::TypeDrawerInfoImage  drawer{RTTI_DESCRIPTOR_NAME,__VA_ARGS__};	\
		RTTI_DESCRIPTOR_NAME->AddDrawer(&drawer);									\
	}

}

RTTI_DEFINE_EXTERNAL_CLASS(std::weak_ptr<rendering::TextureAsset>,
	{
		RTTI_DRAWER(editor::TextureAssetDrawer);
		RTTI_SERIALIZER(rtti::TTypeSerializerAsset<rendering::TextureAsset>);

		RTTI_DISPLAY_NAME("Texture");
	});

