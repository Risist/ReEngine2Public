#include "rttiArray.h"

#include "../Editor/ImGuiManager.h"
#include "../Editor/ImGuiHelpers.h"
#include "BasicDrawers.h"
#include "rttiProperty.h"
#include <iostream>

namespace rtti
{
	static enum class EDelayedCommand
	{
		EDuplicate,
		ERemove,
		ESwap,
	};
	static struct DelayedCommandData
	{
		EDelayedCommand commantType;
		size_t id1;
		size_t id2;
	};

	bool ArrayTypeBase::DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
			//| ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_DefaultOpen
			;

		float offset = ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
		ImGui::SetNextItemWidth(10);
		bool opened = ImGui::TreeNodeEx(IMGUI_ID(context.property->GetDisplayName()), flags);

		afterLabelCall();

		ImGui::SameLine(offset + 50, 50); 
		ImGui::SetNextItemWidth(200);
		int size = GetArraySize(context.objectStart);
		bool resized = ImGui::InputInt(IMGUI_ID("size"), &size);

		static std::vector<DelayedCommandData> delayedCommands;
		delayedCommands.clear();

		bool anyChanged = false;
		if (opened)
		{
			size_t arraySize = GetArraySize(context.objectStart);

			for (size_t i = 0; i < arraySize; ++i)
			{
				TypeDrawingContext elementContext{ context };
				elementContext.offset = context.offset + 1;
				elementContext.objectStart = GetArrayElementAdress(i, context.objectStart);
				elementContext.property = nullptr;
				elementContext.imGuiId = IMGUI_ID("", i);

				std::stringstream stream;
				stream << "[" << i << "] ";
				std::string s = stream.str();
				elementContext.fieldName = s.c_str();

				ImGuiPushID id(i);

				auto afterLabelCall = [&context, i, &s]() {

					const char* dragReorderCode = "ArrayElement";
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						ImGui::LabelText(s.c_str(), "");

						ImGui::SetDragDropPayload(dragReorderCode, &i, sizeof(i));
						ImGui::EndDragDropSource();
					}

					// swap elements // TODO insert in place instead?
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dragReorderCode))
						{
							size_t id1 = *((size_t*)payload->Data);
							delayedCommands.push_back({ EDelayedCommand::ESwap, i, id1 });
						}
						ImGui::EndDragDropTarget();
					}

					if (ImGui::BeginPopupContextItem(IMGUI_ID("",i)))
					{
						if (ImGui::MenuItem(IMGUI_ID("Delete")))
						{
							delayedCommands.push_back({EDelayedCommand::ERemove, i });
						}

						if (ImGui::MenuItem(IMGUI_ID("Duplicate")))
						{
							delayedCommands.push_back({ EDelayedCommand::EDuplicate, i });
						}

						ImGui::EndPopup();
					}
				};

				anyChanged != GetArrayElementType()->GetTypeDrawer()->DrawType(elementContext, afterLabelCall);
			}

			ImGui::TreePop();
		}

		for (auto& it : delayedCommands)
		{
			switch (it.commantType)
			{
			case EDelayedCommand::EDuplicate:
				DuplicateElement(it.id1, context.objectStart);
				break;

			case EDelayedCommand::ERemove:
				RemoveElement(it.id1, context.objectStart);
				break;

			case EDelayedCommand::ESwap:
				Swap(it.id1, it.id2, context.objectStart);
				break;
			}
		}

		if (resized && size >= 0)
		{
			Resize(size, context.objectStart);
		}

		return anyChanged;
	}
	void ArrayTypeBase::Serialize(YAML::Emitter& emitter, SerializationContext context) const
	{
		emitter << YAML::Value << YAML::BeginSeq;

		size_t arraySize = GetArraySize(context.objectStart);
		
		for (size_t i = 0; i < arraySize; ++i)
		{
			SerializationContext elementContext{ context };
			elementContext.objectStart = GetArrayElementAdress(i, context.objectStart);

			GetArrayElementType()->GetTypeSerializer()->Serialize(emitter, elementContext);
		}

		emitter << YAML::EndSeq;
	}
	void ArrayTypeBase::Deserialize(const YAML::Node& node, SerializationContext serializationContext) const
	{
		RE_ASSERT(node);
		RE_ASSERT(node.IsSequence());

		ClearArray(serializationContext.objectStart);

		for (int i = 0; i < node.size(); ++i)
		{
			SerializationContext elementContext{ serializationContext };
			//elementContext.objectStart = AddArrayElement(elementContext.objectStart);

			GetArrayElementType()->GetTypeSerializer()->Deserialize(node[i], elementContext);
		}


	}
}