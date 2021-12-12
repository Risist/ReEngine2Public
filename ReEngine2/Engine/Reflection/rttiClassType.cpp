#include "rttiClassType.h"
#include "BasicDrawers.h"

#include <iostream>

namespace rtti
{



	ClassType::ClassType(const char* name, uint16 size)
		: TypeBase(name, size)
		, _isAbstract(false)
		, _baseType(nullptr)
	{
		SetTypeDrawer(this);
		SetTypeSerializer(this);
	}

	bool ClassType::DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		/**for (int i = 0; i < context.offset; ++i)
			std::cout << "\t";

		std::cout << "Type: " << GetDisplayName();

		if (GetBaseType())
		{
			std::cout << " | base: " << GetBaseType()->GetDisplayName();
		}
		std::cout << "\n";*/
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + context.offset * editor::OFFSET_VALUE);

		bool open = ImGui::CollapsingHeader(context.GetDisplayName(), flags);
		afterLabelCall();
		
		if (open)
		{
			++context.offset;
			return CompoundTypeDrawer::DrawType(context);
		}
		return false;
	}

	void ClassType::Serialize(YAML::Emitter& emitter, SerializationContext serializationContext) const
	{
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "classType" << YAML::Value << _nativeName;
		
		for (auto it : _properties)
		{
			SerializationContext propertyContext{serializationContext};
			propertyContext.offset += 1;
			propertyContext.property = it.get();
			it->Serialize(emitter, propertyContext);
		}

		emitter << YAML::EndMap;
	}

	void ClassType::Deserialize(const YAML::Node& node, SerializationContext serializationContext) const
	{
		RE_ASSERT(node);

		for (auto it : _properties)
		{
			it->Deserialize(node, serializationContext);
		}
	}

}