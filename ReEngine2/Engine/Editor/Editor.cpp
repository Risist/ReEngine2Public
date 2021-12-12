#include "../Common/Settings.h"
#include "Editor.h"

#include "ImGuiManager.h"

#include "Windows/HierarchyInspector.h"
#include "EditorWindow.h"
#include "Windows/ContentBrowserWindow.h"
#include "Windows/AssetInspectorWindow.h"
#include "Windows/SkeletalMeshEditorWindow.h"

#include "../Entity/EntityCore.h"


#include "../Engine.h"

namespace editor
{

	std::unique_ptr<Editor> editor;

	Editor::Editor()
	{
		AddNewWindow(std::move(rtti::GetTypeDescriptor<HierarchyInspector>()->CreateNewUnique<EditorWindow>()));
		AddNewWindow(std::move(rtti::GetTypeDescriptor<AssetInspectorWindow>()->CreateNewUnique<EditorWindow>()));
		AddNewWindow(std::move(rtti::GetTypeDescriptor<ContentBrowserWindow>()->CreateNewUnique<EditorWindow>()));
		//AddNewWindow(std::move(rtti::GetTypeDescriptor<SkeletalMesheditorWindow>().CreateNewUnique<EditorWindow>()));
	}

	void Editor::Init()
	{
		auto& window = engine::engine->GetMainViewport()->_window;
		ImGui::SFML::Init(window);
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();
	}

	void Editor::Draw()
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
		DrawMenuBar();
		DrawWindows();
	}

	void Editor::AddNewWindow(std::unique_ptr<class EditorWindow> newWindow)
	{
		newWindow->Init();
		_editorWindows.push_back(std::move(newWindow));
	}

	void Editor::DrawMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo"))
				{

				}
				
				if (ImGui::MenuItem("Redo"))
				{

				}
				
				if (ImGui::MenuItem("Save Scene As"))
				{

				}

				if (ImGui::MenuItem("Save Scene"))
				{

				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::BeginMenu("Entity"))
				{
					auto actorType = rtti::GetClassDescriptor<entity::Entity>();

					for (int i = 0; i < actorType->GetDerivedAllCount(); ++i)
					{
						// TODO check if an entity can have no parent
						ImGuiPushID id(i);
						auto itType = actorType->GetDerivedAll(i);
						if (ImGui::MenuItem(IMGUI_ID(itType->GetDisplayName())))
						{
							auto _new = std::move(itType->CreateNewShared<entity::Entity>());

							entity::world->GetCurrentLevel()->InsertEntity<entity::Entity>(std::move(_new));
						}
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				auto windowType = rtti::GetClassDescriptor<EditorWindow>();

				for (int i = 0; i < windowType->GetDerivedAllCount(); ++i)
				{
					ImGuiPushID id(i);
					auto itType = windowType->GetDerivedAll(i);
					if (ImGui::MenuItem(IMGUI_ID(itType->GetDisplayName())))
					{
						AddNewWindow(std::move(itType->CreateNewUnique<EditorWindow>()));
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}


	}

	void Editor::DrawWindows()
	{
		for (int i = (int)_editorWindows.size() - 1; i >= 0; --i)
		{
			auto& it = _editorWindows[i];
			if (it->IsOpen() == false)
			{
				_editorWindows.erase(_editorWindows.begin() + i);
				continue;
			}
			ImGuiPushID id(it->GetGuid());
			it->Draw();
		}
	}

}