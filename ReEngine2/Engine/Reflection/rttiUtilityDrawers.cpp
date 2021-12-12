#include "rttiUtilityDrawers.h"
#include "BasicDrawers.h"
#include "../Editor/ImGuiManager.h"

namespace rtti
{
	bool TypeDrawerSpace::DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		ImGui::NewLine();
		return false;
	}

	bool TypeDrawerInfoBox::DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
		ImGui::BeginChild(IMGUI_ID(_message), sf::Vector2f(0, 50), true);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
		ImGui::Text(_message);
		ImGui::EndChild();

		return false;
	}

}