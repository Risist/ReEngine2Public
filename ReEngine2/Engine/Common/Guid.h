#pragma once
#include "Settings.h"
#include <filesystem>
#include <xhash>
#include "../Reflection/Reflection.h"

class Guid
{
public:
	Guid();
	Guid(uint64 guid);
	Guid(const Guid&) = default;
	 
	operator uint64() const { return _id; }

private:
	uint64 _id;
};

namespace rtti
{
	template<>
	class TTypeSerializerCast<Guid> : public TypeSerializerBase
	{
		virtual void Serialize(YAML::Emitter& emitter, SerializationContext context) const override
		{
			emitter << YAML::Value << (uint64)*(Guid*)context.objectStart;
		}
		virtual void Deserialize(const YAML::Node& node, SerializationContext context) const override
		{
			Guid* v = (Guid*)context.objectStart;
			*v = Guid(node.as<uint64>());
		}
	};
}

namespace std
{
	template<>
	struct hash<Guid>
	{
		std::size_t operator() (const Guid& guid) const
		{
			return hash<uint64>()((uint64)guid);
		}
	};
}

RTTI_DEFINE(Guid, {
	RTTI_DISPLAY_NAME("Guid");
	RTTI_DRAWER(editor::IntDrawer<Guid>);
	RTTI_SERIALIZER(rtti::TTypeSerializerCast<Guid>);
})