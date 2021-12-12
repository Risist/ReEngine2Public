#include "../../Common/Settings.h"
#include "HierarchyInspector.h"

#include "../../../Imgui/imgui.h"
#include "../../../Imgui/imgui-SFML.h"

#include <iostream>
#include "../ImGuiManager.h"

#include "../Editor.h"
#include "../../Engine.h"

#include "../../Entity/EntityCore.h"

RTTI_DEFINE_CLASS(editor::HierarchyInspector,
	{
		RTTI_BASE(editor::EditorWindow);
	});

namespace editor
{

	void HierarchyInspector::Init()
	{
	}



	void HierarchyInspector::Draw()
	{
		bool open = ImGui::Begin(IMGUI_ID("Hierarchy"), &_isOpen,
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_AlwaysVerticalScrollbar
		);

		if (!open)
		{
			ImGui::End();
			return;
		}

		auto& world = entity::world;

		static char buffer[128];
		memset(buffer, 0, sizeof(buffer));
		ImGui::Text("Search:");
		ImGui::SameLine();
		ImGui::InputText(IMGUI_ID(""), buffer, sizeof(buffer));

		std::string s = buffer;


		if (s.empty())
		{
			int i = 0;
			world->Iterate([&i, this](std::shared_ptr<entity::Entity> it)
			{
				ImGuiPushID pushId(i++);
				DrawEntity(it, true);
				return false;
			});
		}
		else
		{
			int i = 0;
			world->IterateInDepth([&i, &s, this](std::shared_ptr<entity::Entity> it)
			{
				if (it->GetName().find(s) != std::string::npos)
				{
					ImGuiPushID pushId(i);
					DrawEntity(it, false);
				}
				++i;
				return false;
			});
		}



		if (ImGui::BeginPopupContextWindow())
		{
			entity::CompoundEntity* compound = dynamic_cast<entity::CompoundEntity*>(hoveredEntity.get());

			if (!hoveredEntity)
			{
				if (ImGui::BeginMenu("Create Entity"))
				{
					auto actorType = rtti::GetClassDescriptor<entity::Entity>();

					for (int i = 0; i < actorType->GetDerivedAllCount(); ++i)
					{
						ImGuiPushID id(i);

						auto itType = actorType->GetDerivedAll(i);

						if (ImGui::MenuItem(IMGUI_ID(itType->GetDisplayName())))
						{
							std::string name("New ");
							name += itType->GetDisplayName();

							auto _new = std::move(itType->CreateNewShared<entity::Entity>());
							_new->SetName(name);
							entity::world->GetCurrentLevel()->InsertEntity<entity::Entity>(std::move(_new));
						}
						if (itType->GetTooltip() != "" && ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text(itType->GetTooltip());
							ImGui::EndTooltip();
						}

					}
					ImGui::EndMenu();
				}
			}
			else if (compound)
			{
				if (ImGui::BeginMenu("Add Child Entity"))
				{
					auto actorType = rtti::GetClassDescriptor<entity::Entity>();

					for (int i = 0; i < actorType->GetDerivedAllCount(); ++i)
					{
						ImGuiPushID id(i);

						auto itType = actorType->GetDerivedAll(i);
						auto itClassType = dynamic_cast<const rtti::ClassType*>(itType);
						RE_ASSERT(itClassType);

						if (compound->CanBeAChild(*itClassType) == false)
						{
							continue;
						}

						if (ImGui::MenuItem(IMGUI_ID(itType->GetDisplayName())))
						{
							std::string name("New ");
							name += itType->GetDisplayName();

							auto _new = std::move(itType->CreateNewShared<entity::Entity>());
							_new->SetName(name);
							compound->InsertComponent<entity::Entity>(std::move(_new));
						}
					}
					ImGui::EndMenu();
				}
			}

			if (hoveredEntity != nullptr)
			{
				if (ImGui::MenuItem(IMGUI_ID("Delete")))
				{
					hoveredEntity->Destroy();
				}

				if (ImGui::MenuItem(IMGUI_ID("Rename")))
				{
					renamedEntity = hoveredEntity;
				}

				if (ImGui::MenuItem(IMGUI_ID("Duplicate")))
				{
					auto entity = hoveredEntity->Duplicate();
					if( hoveredEntity->GetParent().expired() )
					{
						entity::world->GetCurrentLevel()->InsertEntity(entity);
					}
					else
					{
						auto parent = dynamic_cast<entity::CompoundEntity*>(hoveredEntity->GetParent().lock().get());
						parent->InsertComponent(entity);
					}

					//renamedEntity = entity;
				}
			}

			ImGui::EndPopup();
		}
		else
		{
			hoveredEntity = nullptr;
		}

		ImGui::End();
	}

	void HierarchyInspector::DrawEntity(std::weak_ptr<entity::Entity> entity, bool recursive)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_DefaultOpen
			;

		if (entity.expired())
			return;

		auto& world = entity::world;

		auto locked = entity.lock();
		auto compound = dynamic_cast<entity::CompoundEntity*>(locked.get());
		ImGuiPushID id(locked->GetGuid());

		if (!compound || !compound->HasAnyChildComponent() || !recursive)
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool opened = true;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
		//ImGui::InvisibleButton(IMGUI_ID(""), { ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 20, 2.0f });
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Scene_Component"))
			{
				Guid guid1 = *((Guid*)payload->Data);
				//int id1 = world.IndexOf(engine::engine.GetWorld().GetComponent(guid1));
				//int id2 = world.IndexOf(locked);

				//if (id1 != -1 && id2 != -1)
				//	world.SwapActors(id1, id2);
			}
			ImGui::EndDragDropTarget();
		}

		static char renameBuffor[100];
		if (renamedEntity.get() == locked.get())
		{
			ImGui::BeginGroup();
			opened = ImGui::TreeNodeEx(IMGUI_ID(""), flags);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(175);
			if (ImGui::InputText(IMGUI_ID(""), renameBuffor, sizeof(renameBuffor), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				renamedEntity->SetName(renameBuffor);
				renamedEntity = nullptr;
			}
			ImGui::EndGroup();
		}
		else
		{
			opened = ImGui::TreeNodeEx(IMGUI_ID(locked->GetName().c_str()), flags);

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				Guid id = locked->GetGuid();

				ImGui::SetDragDropPayload(entity::Entity::SCENE_PAYLOAD_CODE, &id, sizeof(id));
				ImGui::EndDragDropSource();
			}
		}

		

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(entity::Entity::SCENE_PAYLOAD_CODE))
			{
				Guid guid1 = *((Guid*)payload->Data);
				//int id1 = world.IndexOf(engine::engine->GetWorld().GetComponent(guid1));
				//int id2 = world.IndexOf(locked);

				//if (id1 != -1 && id2 != -1)
				//	world.SwapActors(id1, id2);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemHovered())
		{
			hoveredEntity = locked;

			if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				editor::editor->editorContext.Select(entity);
			}
		}

		// rename upon double click
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			renamedEntity = locked;
			memcpy(renameBuffor, locked->GetName().c_str(), locked->GetName().size());
		}

		if (opened)
		{
			if (compound && recursive)
			{
				for (auto& it : *compound)
				{
					DrawEntity(it);
				}
			}

			ImGui::TreePop();
		}
	}
}