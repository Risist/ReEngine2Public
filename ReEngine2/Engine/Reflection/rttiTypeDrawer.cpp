#include "rttiTypeDrawer.h"

#include "rttiProperty.h"
#include <iostream>
#include "../Editor/ImGuiManager.h"
#include "BasicDrawers.h"

namespace rtti
{
	bool CompoundTypeDrawer::DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow;

		bool anyChanged = false;
		bool first = true;;
		for (auto category : _categories)
		{
			if (!first)
			{
				ImGui::NewLine();
				first = false;
			}

			bool open = true;
			bool hasCategory = strcmp("", category) != 0;
			if (hasCategory)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
				open = ImGui::CollapsingHeader(category, flags);
			}

			int i = 0;
			if (open)
			{
				for (auto& entry : _typeDrawingEntries)
				{
					if (strcmp(entry->GetCategory(), category) == 0)
					{
						TypeDrawingContext entryContext(context);
						if (hasCategory)
						{
							++entryContext.offset;
						}

						ImGuiPushID id(i++);

						anyChanged |= entry->DrawType(entryContext);
					}
				}
			}

		}
		return anyChanged;
	}
	void CompoundTypeDrawer::AddDrawer(TypeDrawerBase* drawer)
	{
		_typeDrawingEntries.push_back(drawer);

		// set default category
		drawer->SetCategory(GetCategory());
	}
	void CompoundTypeDrawer::PrepareCategories()
	{
		_categories.clear();
		for (auto& entry : _typeDrawingEntries)
		{
			if (!entry->IsEditorVissible())
				continue;

			bool categoryAlreadyDefined = false;
			for (auto category : _categories)
			{
				if (strcmp(category, entry->GetCategory()) == 0)
				{
					categoryAlreadyDefined = true;
					break;
				}
			}
			if (categoryAlreadyDefined)
				continue;

			_categories.push_back(entry->GetCategory());
		}
	}
	const char* TypeDrawingContext::GetDisplayName() const
	{
		return property ? property->GetDisplayName() : fieldName;
	}
}