#include "rttiProperty.h"
#include "rttiTypeBase.h"
#include "rttiClassType.h"

#include "BasicDrawers.h"

#include <iostream>


namespace rtti
{
	Property::Property(TypeBase* type, const char* nativeName, const ClassType* owningType, uint32 offset)
		: _propertyType(type)
		, _owningClass(owningType)
		, _nativeName(nativeName)
		, _displayName(nativeName)
		, _offset(offset)
		, _readOnly(false)
		, _serialized(true)
		, _overrideTypeDrawer(nullptr)
		, _overrideTypeSerializer(nullptr)
		, _tooltip("")
		, _overrideSetter(nullptr)
		, _overrideGetter(nullptr)
		, _isTypeDrawerAReference(false)
		, _isTypeSerializerAReference(false)
	{
	}
	Property::~Property()
	{
		if (_overrideTypeDrawer && !_isTypeDrawerAReference)
			delete _overrideTypeDrawer;

		if (_overrideTypeSerializer && !_isTypeSerializerAReference)
			delete _overrideTypeSerializer;
	}
	bool Property::operator==(const Property& other) const
	{
		return strcmp(other.GetNativeName(), GetNativeName())
			&& _owningClass->IsA(other._owningClass);
	}
	bool Property::DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const
	{
		if (!IsEditorVissible())
			return false;

		context.property = this;
		context.fieldName = GetDisplayName();
		context.objectStart = GetPropertyAddres(context.objectStart);

		auto drawer = _overrideTypeDrawer ? _overrideTypeDrawer : GetPropertyType()->GetTypeDrawer();
		auto call = [&]()
		{
			if (GetTooltip() != "" && ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text(GetTooltip());
				ImGui::EndTooltip();
			}
			afterLabelCall();
		};

		return drawer->DrawType(context, call);
	}
	void Property::Serialize(YAML::Emitter& emitter, SerializationContext context) const
	{
		if (!IsSerialized())
			return;
		emitter << YAML::Key << _nativeName;
		
		context.objectStart = GetPropertyAddres(context.objectStart);
		context.property = this;

		auto serializer = _overrideTypeSerializer ? _overrideTypeSerializer : GetPropertyType()->GetTypeSerializer();

		RE_CHECK(serializer)->Serialize(emitter, context);
	}
	void Property::Deserialize(const YAML::Node& node, SerializationContext context) const
	{
		if (!IsSerialized())
			return;

		auto n = node[_nativeName];
		if (n)
		{
			context.objectStart = GetPropertyAddres(context.objectStart);
			context.property = this;

			auto serializer = _overrideTypeSerializer ? _overrideTypeSerializer : GetPropertyType()->GetTypeSerializer();
			RE_CHECK(serializer)->Deserialize(n, context);
		}
		else
		{
			std::cerr << "File is ill formed\n";
		}
	}
}