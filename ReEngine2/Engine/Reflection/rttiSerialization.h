#pragma once
#include <vector>
#include "../Common/Settings.h"
#include "yaml-cpp/yaml.h"

namespace rtti
{
	class SerializationContext
	{
	public:
		SerializationContext(void* objectToDeserializePtr)
			: objectStart(objectToDeserializePtr)
			, property(nullptr)
		{
			RE_ASSERT((objectStart, "SerializationContext.objectToDeserializePtr can\'t be null, please assign a valid object address"));
		}
		void* objectStart;
		const class Property* property;
		int offset = 0;
	};

	class TypeSerializerBase
	{
	public:
		virtual void Serialize(YAML::Emitter& emitter, SerializationContext serializationContext) const = 0;
		virtual void Deserialize(const YAML::Node& node, SerializationContext serializationContext) const = 0;
	};

	class TypeSerializerNoSerialize : public TypeSerializerBase
	{

		virtual void Serialize(YAML::Emitter& emitter, SerializationContext serializationContext) const override
		{

		}
		virtual void Deserialize(const YAML::Node& node, SerializationContext serializationContext) const override
		{
		}
	};

	template<typename Type>
	class TTypeSerializerCast : public TypeSerializerBase
	{
		virtual void Serialize(YAML::Emitter& emitter, SerializationContext context) const override
		{
			emitter << YAML::Value << *(Type*)context.objectStart;
		}
		virtual void Deserialize(const YAML::Node& node, SerializationContext context) const override
		{
			Type* v = (Type*)context.objectStart;
			*v = node.as<Type>();
		}
	};
	
}