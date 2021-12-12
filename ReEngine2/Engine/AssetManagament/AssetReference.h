#pragma once
#include <Engine/Editor/ImGuiManager.h>

#include "AssetManager.h"


namespace rtti
{
	template<typename InnerType>
	class TTypeSerializerAsset : public TypeSerializerBase
	{
		virtual void Serialize(YAML::Emitter& emitter, SerializationContext context) const override
		{
			auto weak = (std::weak_ptr<InnerType>*)context.objectStart;

			if (weak->expired())
			{
				emitter << YAML::Value << 0;
			}
			else
			{
				auto locked = weak->lock();
				emitter << YAML::Value << locked->GetGuid();
			}


		}
		virtual void Deserialize(const YAML::Node& node, SerializationContext context) const override
		{
			std::weak_ptr<InnerType>* v = (std::weak_ptr<InnerType>*)context.objectStart;

			Guid guid(node.as<uint64>());
			auto assetManager = (engine::TAssetManager<InnerType>)engine::GetAssetManager<InnerType>();
			*v = assetManager.FindAssetByGuid(guid);
		}
	};


}


namespace editor
{
	// drawer for assets
	template <typename AssetT>
	class TAssetDrawer : public rtti::TypeDrawerBase
	{
	public:
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			const char* currentItemName = "None";
			const char* popupId = "ComponentPopup";

			std::weak_ptr<AssetT>* varriable = (std::weak_ptr<AssetT>*)context.objectStart;

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
			{
				ImGui::OpenPopup(popupId);
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetT::ASSET_PAYLOAD_CODE))
				{
					Guid componentGuid = *((Guid*)payload->Data);

					*varriable = engine::GetAssetManager<AssetT>().FindAssetByGuid(componentGuid);
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

				if (ImGui::Button(IMGUI_ID((std::string("Current: ") + currentItemName).c_str())))
				{
					ImGui::CloseCurrentPopup();
				}

				if (!varriable->expired())
				{
					if (ImGui::Button(IMGUI_ID("None")))
					{
						*varriable = std::weak_ptr<AssetT>();
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::NewLine();

				int i = 0;
				for (auto& it : engine::GetAssetManager<AssetT>())
				{
					ImGuiPushID id{ ++i };

					auto& name = it.second->GetName();
					if (!searchString.empty() && name.find(searchString) == std::string::npos)
						continue;

					if (ImGui::Button(IMGUI_ID(name.c_str())))
					{
						*varriable = it.second;
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::EndPopup();
			}

			return false;
		};
	};
}


RTTI_DEFINE_NESTED(std::weak_ptr, AssetDerivedType,
	{
		RTTI_DRAWER(editor::TAssetDrawer<AssetDerivedType>);
		RTTI_SERIALIZER(rtti::TTypeSerializerAsset<AssetDerivedType>);
	});