#include "rttiEnum.h"
#include "rttiProperty.h"
#include "../Editor/ImGuiManager.h"
#include "BasicDrawers.h"

#include <iostream>

namespace rtti
{
    template<typename Ty>
    static inline bool DrawEnum(const EnumType& enumV, TypeDrawingContext& context, std::function<void()> afterLabelCall)
    {
        int64 enumId = *((Ty*)context.objectStart);
        if (context.property != nullptr && context.property->GetOverridePropertyGetter())
        {
            rtti::FunctionCallingContext functionContext;
            functionContext.object = context.property->GetOwnerAddres(context.objectStart);
            functionContext.result = &enumId;
            context.property->GetOverridePropertyGetter()->Invoke(functionContext);
        }

        bool valueChanged = false;

        if (enumId >= 0 && enumId < (int64)enumV.GetOptionsCount())
        {
            //std::cout << GetOption(enumId)->GetDisplayName();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);
            ImGui::Text(context.GetDisplayName());
            ImGui::SameLine();

            if (ImGui::BeginCombo(IMGUI_ID("", context.GetDisplayName()), enumV.GetOption(enumId)->GetDisplayName()))
            {
                for (size_t i = 0; i < enumV.GetOptionsCount(); i++)
                {
                    const bool is_selected = (enumId == i);
                    if (ImGui::Selectable(enumV.GetOption(i)->GetDisplayName(), is_selected))
                    {
                        if (context.property != nullptr && context.property->GetOverridePropertySetter())
                        {
                            rtti::FunctionCallingContext functionContext;
                            functionContext.object = context.property->GetOwnerAddres(context.objectStart);
                            functionContext.argumentPointers.push_back(&enumId);
                            context.property->GetOverridePropertySetter()->Invoke(functionContext);
                        }
                        else
                        {
                            *((Ty*)context.objectStart) = i;
                        }

                        valueChanged = true;
                    }
                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        return valueChanged;
    }

	bool EnumType::DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
        if(GetTypeSize() == sizeof(uint8))
            return DrawEnum<uint8>(*this, context, afterLabelCall);

        if (GetTypeSize() == sizeof(uint16))
            return DrawEnum<uint16>(*this, context, afterLabelCall);

        if (GetTypeSize() == sizeof(uint32))
            return DrawEnum<uint32>(*this, context, afterLabelCall);

        if (GetTypeSize() == sizeof(uint64))
            return DrawEnum<uint64>(*this, context, afterLabelCall);

        return false;
	}
    void EnumType::Serialize(YAML::Emitter& emitter, SerializationContext context) const
    {
#define TYPE_DEPENDENT(Type)                                   \
        if(GetTypeSize() == sizeof(Type))                      \
        {                                                      \
            int64 enumId = *((Type*)context.objectStart);      \
                                                               \
            if (enumId >= 0 && enumId < (int64)GetOptionsCount())     \
                emitter << GetOption(enumId)->GetNativeName(); \
        }

        TYPE_DEPENDENT(uint8) 
        else TYPE_DEPENDENT(uint16)
        else TYPE_DEPENDENT(uint32)
        else TYPE_DEPENDENT(uint64)
        else std::cerr << "Invalid enum type size";

#undef TYPE_DEPENDENT
    }
    void EnumType::Deserialize(const YAML::Node& node, SerializationContext context) const
    {
#define TYPE_DEPENDENT(Type)                                                        \
        if(GetTypeSize() == sizeof(Type))                                           \
        {                                                                           \
            std::string parsedEnum = node.as<std::string>();                        \
            auto option = FindOptionByNativeName(parsedEnum.c_str());               \
            if(option == nullptr)                                                   \
            {                                                                       \
                std::cerr << "Invalid enum name";                                   \
            }                                                                       \
            int64 parsedEnumId = option->GetValue();                                \
                                                                                    \
            if (parsedEnumId >= 0 && parsedEnumId < (int64)GetOptionsCount())       \
            {                                                                       \
                Type* enumId = (Type*)context.objectStart;                          \
                *enumId = (Type)parsedEnumId;                                             \
            }                                                                       \
            else std::cerr << "loaded invalid enum value";                          \
        }

        TYPE_DEPENDENT(uint8)
        else TYPE_DEPENDENT(uint16)
        else TYPE_DEPENDENT(uint32)
        else TYPE_DEPENDENT(uint64)
        else std::cerr << "Invalid enum type";

#undef TYPE_DEPENDENT
    }
}