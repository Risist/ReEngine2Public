#include "../../Common/Settings.h"

#include "ContentBrowserWindow.h"

#include "../../../Imgui/imgui.h"
#include "../../../Imgui/imgui-SFML.h"

#include "../ImGuiManager.h"

#include <iostream>

#include <Engine/AssetManagament/Asset.h>
#include <Engine/AssetManagament/AssetManager.h>

RTTI_DEFINE_CLASS(editor::ContentBrowserWindow,
	{
		RTTI_BASE(editor::EditorWindow);
		RTTI_DISPLAY_NAME("ContentBrowser");
	});
namespace editor
{
	ContentBrowserAssetDrawingSettings ContentBrowserWindow::assetDrawingSettings;
	ContentBrowserWindow::ContentBrowserWindow()
	{
		assetDirectory = "Assets";
		currentDirectory = assetDirectory;
	}

	void ContentBrowserWindow::Draw()
	{
		bool open = ImGui::Begin(IMGUI_ID("Content Browser"), &_isOpen,
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_AlwaysVerticalScrollbar
		);

		if (!open)
		{
			ImGui::End();
			return;
		}

		float cellSize = assetDrawingSettings.thumbnailSize + assetDrawingSettings.padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int(panelWidth / cellSize));
		if (columnCount < 1)
			columnCount = 1;


		char buffer[128];
		memset(buffer, 0, sizeof(buffer));
		ImGui::Text("Search: ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);
		ImGui::InputText(IMGUI_ID(""), buffer, sizeof(buffer));

		std::string searchedExpression = buffer;

		if (currentDirectory != assetDirectory)
		{
			ImGui::SameLine();
			if (ImGui::Button("<--"))
			{
				currentDirectory = currentDirectory.parent_path();
			}
		}

		bool contextMenuOpened = false;
		if (ImGui::BeginTable(IMGUI_ID(""), columnCount))
		{
			if (searchedExpression.empty())
			{
				// iterate over folders to display them first
				for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory))
				{
					if (!directoryEntry.is_directory())
						continue;

					contextMenuOpened |= ProcessItem(directoryEntry);
				}

				for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory))
				{
					if (directoryEntry.is_directory())
						continue;

					contextMenuOpened |= ProcessItem(directoryEntry);
				}

			}
			else
			{
				// iterate over folders to display them first
				for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(assetDirectory))
				{
					if (!directoryEntry.is_directory())
						continue;

					const auto& path = directoryEntry.path();
					auto relativePath = std::filesystem::relative(path, assetDirectory);
					std::string filenameString = relativePath.filename().string();

					if (filenameString.find(searchedExpression) == std::string::npos)
						continue;

					contextMenuOpened |= ProcessItem(directoryEntry);
				}

				for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(assetDirectory))
				{
					if (directoryEntry.is_directory())
						continue;

					const auto& path = directoryEntry.path();
					auto relativePath = std::filesystem::relative(path, assetDirectory);
					std::string filenameString = relativePath.filename().string();

					if (filenameString.find(searchedExpression) == std::string::npos)
						continue;

					contextMenuOpened |= ProcessItem(directoryEntry);
				}
			}

			ImGui::EndTable();
		}

		if ( !contextMenuOpened && ImGui::BeginPopupContextWindow(IMGUI_ID("")))
		{
			if (ImGui::BeginMenu("New"))
			{


				ImGui::EndMenu();
			}

			if (ImGui::MenuItem(IMGUI_ID("?")))
			{

			}

			ImGui::EndPopup();
		}

		


		ImGui::End();
	}

	bool ContentBrowserWindow::ProcessItem(const std::filesystem::directory_entry& currentEntry)
	{
		bool contextMenuOpened = false;
		const auto& path = currentEntry.path();
		auto relativePath = std::filesystem::relative(path, assetDirectory);
		std::string filenameString = relativePath.filename().string();

		//if (filenameString.find(searchedExpression) == std::string::npos)
		//	continue;

		if (currentEntry.is_directory())
		{
			ImGui::TableNextColumn();
			ImGui::ImageButton(assetDrawingSettings.directoryTexture, { assetDrawingSettings.thumbnailSize, assetDrawingSettings.thumbnailSize });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				currentDirectory /= path.filename();
			}

			ImGui::Text(filenameString.c_str());

			return contextMenuOpened;
		}

		// find out what kind of asset is the file
		{
			//////////////// Step 1

			///// Option 1
			// meta would have some kind of string code describing current 
			// then we would retreive the asset 

			///// Option 2
			// we will need to register somehow the asset into an array of supported assets (?)

			///// Option 3
			// we will iterate over All types deriving from Asset
			// then AssetType needs to have function returning manager for the given type
			// and also managers will need to have their base class with the function implemented by template overload
			// the function will return an asset given guid
			// overloads also need to have function that will return exact value stored

			///////////////// Step 2

			// now for each asset type we need to check if the extension is of correct type
			// there could be many extensions of the same type so we will need to store asset type name in the meta file
			// soo asset ownership is identified by extensions and type name
			// 
			// assets like textures will be identified just by an extension
			// engine generated assets (those where we can serialize anything we'd like) will have type name inside
			// meta file will contain guid and type name
			// in case of textures the meta file will contain also all the settings since we can not modify texture to store the information
			// audio files and all the other ones will be treated in the same way as textures

			for (auto it = engine::AssetManagerBase::GetAllAssetManagersBegin(); it != engine::AssetManagerBase::GetAllAssetManagersEnd(); ++it)
			{
				auto ptr = (*it);
				
				if (ptr->GetAssetType()->IsSupportedNativeFileExtension(path.extension().c_str()))
				{
					bool isAssetAlreadyCreated = std::filesystem::exists(std::filesystem::path(path.string() + ptr->GetAssetType()->GetFileExtension()));;
					if (!isAssetAlreadyCreated)
					{
						// create asset an file bassed on the extension
						ptr->CreateAssetFileFromNative(path);
					}

					break;
				}
				else if (ptr->GetAssetType()->IsSupportedFileExtension(path.extension().c_str()))
				{
					auto guid = ptr->ReadGuidFromMetaFile(path);

					auto asset = ptr->FindAssetBaseByGuid(guid);

					if (asset.expired() == false)
					{
						ImGui::TableNextColumn();
						auto lockedAsset = asset.lock();
						contextMenuOpened = lockedAsset->DrawContentBrowserElement(asset, path, assetDrawingSettings);

					}
					else
					{
						// load the file into asset manager
						ptr->AddAssetFromFile(path);
					}

					break;
				}
			}

			
		}
		

		/**auto& ts = currentEntry.is_directory() ? assetDrawingSettings.directoryTexture : assetDrawingSettings.fileTexture;
		ImGui::ImageButton(ts, { assetDrawingSettings.thumbnailSize, assetDrawingSettings.thumbnailSize });


		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (currentEntry.is_directory())
			{
				currentDirectory /= path.filename();
			}
		}

		ImGui::Text(filenameString.c_str());*/

		return contextMenuOpened;
	}
	ContentBrowserAssetDrawingSettings::ContentBrowserAssetDrawingSettings()
	{
		directoryTexture.loadFromFile("Assets/EngineAssets/folderIcon.png");
		fileTexture.loadFromFile("Assets/EngineAssets/fileIcon.png");
		arrowTexture.loadFromFile("Assets/EngineAssets/arrow.png");
	}
}