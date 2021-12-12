#include "Entity.h"
#include "CompoundEntity.h"
#include "EntityWorld.h"

#include <Engine/AssetManagament/AssetManager.h>
#include <Engine/Engine.h>


RTTI_DEFINE_ASSET(entity::Entity,
	{
		RTTI_BASE(Asset);
	});


namespace entity
{
	/////////////////////////////////////////
	//////////////// Entity

	Entity::Entity()
		: _entityState(EEntityState::ECreated)
		, _bVisible(true)
	{
		SetName(std::string("New ") + GetType()->GetDisplayName());
	}

	void Entity::Init()
	{
		_entityState = EEntityState::EInitializing;
		OnInit();
		_entityState = EEntityState::EInitialized;
	}

	void Entity::Spawn()
	{
		_entityState = EEntityState::ESpawning;
		OnSpawn();
		Enable();
	}

	void Entity::Enable()
	{
		RE_ASSERT(_entityState != EEntityState::EEnabling);
		//RE_ASSERT(_entityState != EEntityState::EEnabled);
		if (_entityState == EEntityState::EEnabled)
			return;

		_entityState = EEntityState::EEnabling;
		OnEnable();
		_entityState = EEntityState::EEnabled;
	}

	void Entity::Disable()
	{
		RE_ASSERT(_entityState != EEntityState::EDisabling);
		if (_entityState == EEntityState::EDisabled)
			return;

		_entityState = EEntityState::EDisabling;
		OnDisable();
		_entityState = EEntityState::EDisabled;
	}

	void Entity::SetEnabled(bool enabled)
	{
		if (enabled)
			Enable();
		else
			Disable();
	}

	void Entity::SetVisibility(bool visible)
	{
		bool visibilityIsChanged = visible != _bVisible;
		_bVisible = visible;

		if (visibilityIsChanged)
			OnVisibilityChanged();
	}

	void Entity::Destroy()
	{
		if (IsEnabled())
		{
			Disable();
		}
		_entityState = EEntityState::EPeendingDestroy;
	}

	void Entity::PerformDestroy()
	{
		_entityState = EEntityState::EDestroying;
		OnDestroy();

		_entityState = EEntityState::EDestroyed;
	}


	/////////////////// Editor

	void Entity::ShowInInspector(editor::EditorContext& editorContext)
	{
		rtti::Draw(*this, GetName().c_str());
	}

	void Entity::CreateDragPayload() const
	{
		CreateDefaultDragPayload(ASSET_PAYLOAD_CODE);
	}

	bool Entity::CanBeChildOf(CompoundEntity* potentialParent)
	{
		if (potentialParent == nullptr)
		{
			return false;
		}
		return true;
	}

	std::shared_ptr<Entity> Entity::Duplicate() const
	{
		std::shared_ptr<Entity> duplicated(GetType()->DuplicateNewShared<Entity>(this));
		duplicated->_this = duplicated;
		return duplicated;
	}

	/////////////////////////////////////////
	//////////////// EntityContainer
	
}

namespace editor
{
	bool EntityAssetDrawer::DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		const char* currentItemName = "None";
		const char* popupId = "ComponentPopup";

		std::weak_ptr<entity::Entity>* varriable = (std::weak_ptr<entity::Entity>*)context.objectStart;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
		ImGui::Text(context.GetDisplayName());
		afterLabelCall();

		if (!varriable->expired())
		{
			auto lockedVarriable = varriable->lock();
			currentItemName = lockedVarriable->GetName().c_str();
		}

		ImGui::SameLine();
		if (ImGui::Button(IMGUI_ID(currentItemName)))
			//if (ImGui::ImageButton(sf::Texture(), {30,30}))
		{
			ImGui::OpenPopup(popupId);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(entity::Entity::SCENE_PAYLOAD_CODE))
			{
				Guid entityGuid = *((Guid*)payload->Data);

				*varriable = entity::world->FindEntityInDepth(entityGuid);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopup(popupId))
		{
			static char searchBuffer[64];
			memset(searchBuffer, 0, sizeof(searchBuffer));
			ImGui::Text("Search: ");
			ImGui::SameLine();
			ImGui::InputText(IMGUI_ID(""), searchBuffer, sizeof(searchBuffer));

			std::string searchString = searchBuffer;

			static bool sceneAsset = true;
			if (ImGui::Button(IMGUI_ID("Scene")))
			{
				sceneAsset = true;
			}
			ImGui::SameLine();
			if (ImGui::Button(IMGUI_ID("Assets")))
			{
				sceneAsset = false;
			}

			if (ImGui::Button(IMGUI_ID((std::string("Current: ") + currentItemName).c_str())))
			{
				ImGui::CloseCurrentPopup();
			}

			if (!varriable->expired())
			{
				if (ImGui::Button(IMGUI_ID("None")))
				{
					*varriable = std::weak_ptr<entity::Entity>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::NewLine();

			if (sceneAsset)
			{
				int i = 0;
				for (auto& it : *entity::world)
				{
					ImGuiPushID id{ ++i };
					auto& name = it->GetName();
					if (!searchString.empty() && name.find(searchString) == std::string::npos)
						continue;

					if (ImGui::Button(IMGUI_ID(name.c_str())))
					{
						*varriable = it;
						ImGui::CloseCurrentPopup();
					}
				}
			}
			else
			{

			}

			ImGui::EndPopup();
		}

		return false;
	}
}


