#pragma once
#include "../../Imgui/imgui.h"
#include "../../Imgui/imgui-SFML.h"

struct ImGuiPushID
{
	ImGuiPushID(int id)
	{
		ImGui::PushID(id);
	}
	~ImGuiPushID()
	{
		ImGui::PopID();
	}
};

/*#define ImCheckbox(varriable)									 \
	{															 \
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);  \
		ImGui::Text(#varriable);								 \
		ImGui::SameLine();										 \
		ImGui::Checkbox("###Updateaed" #varriable, &(varriable));\
	}
	*/
#define ResStringify(x) #x

#define ResCheckbox(varriable)									 \
{																 \
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);		 \
    ImGui::Text(ResStringify(varriable));						 \
    ImGui::SameLine();											 \
	ImGui::Checkbox("###" ResStringify(varriable), &(varriable));\
}

#define ResFloat(varriable)											\
{																	\
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);			\
    ImGui::Text(ResStringify(varriable));							\
	ImGui::SameLine();												\
	ImGui::InputFloat("###" ResStringify(varriable), &(varriable));	\
} 