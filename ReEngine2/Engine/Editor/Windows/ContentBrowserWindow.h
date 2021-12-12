#pragma once
#include <memory>
#include <functional>
#include <filesystem>
#include "../EditorWindow.h"
#include "../../Common/Settings.h"

namespace editor
{
	class ContentBrowserAssetDrawingSettings
	{
	public:
		ContentBrowserAssetDrawingSettings();
		float padding = 16.0f;
		float thumbnailSize = 128.0f;
		sf::Texture directoryTexture;
		sf::Texture fileTexture;

		sf::Texture arrowTexture;
	};

	class ContentBrowserWindow : public EditorWindow
	{
		RTTI_POLYMORPHIC_CLASS_BODY(ContentBrowserWindow)
	public:
		ContentBrowserWindow();

		virtual void Draw() override;


		static ContentBrowserAssetDrawingSettings assetDrawingSettings;
	private:
		std::filesystem::path assetDirectory;
		std::filesystem::path currentDirectory;
		

		bool ProcessItem(const std::filesystem::directory_entry& currentEntry);
	};
}