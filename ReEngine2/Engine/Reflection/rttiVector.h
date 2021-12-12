#pragma once
#include "rttiTypeDrawer.h"
#include "rttiSerialization.h"
#include "glm/glm.hpp"

RTTI_DEFINE_EXTERNAL_CLASS(sf::Vector2f,
    {
        RTTI_DRAWER(editor::Vector2FDrawer<sf::Vector2f>);

        RTTI_DISPLAY_NAME("Vector2f");
        RTTI_PROPERTY(x);
        RTTI_PROPERTY(y);
    });

RTTI_DEFINE_EXTERNAL_CLASS(sf::Vector2i,
	{
		RTTI_DRAWER(editor::Vector2IDrawer<sf::Vector2i>);

		RTTI_DISPLAY_NAME("Vector2i");
		RTTI_PROPERTY(x);
		RTTI_PROPERTY(y);
	});

RTTI_DEFINE_EXTERNAL_CLASS(sf::Vector2u,
	{
		RTTI_DRAWER(editor::Vector2IDrawer<sf::Vector2u>);

		RTTI_DISPLAY_NAME("Vector2u");
		RTTI_PROPERTY(x);
		RTTI_PROPERTY(y);
	});

namespace rtti
{
	class TypeSerializerCast_glmVec2 : public TypeSerializerBase
	{
		virtual void Serialize(YAML::Emitter& emitter, SerializationContext context) const override
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "classType" << YAML::Value << "glm::vec2";

			auto ptr = (glm::vec2*)context.objectStart;
			emitter << YAML::Key << "x" << YAML::Value << (float)ptr->x;
			emitter << YAML::Key << "y" << YAML::Value << (float)ptr->y;

			emitter << YAML::EndMap;
		}
		virtual void Deserialize(const YAML::Node& node, SerializationContext context) const override
		{
			// TODO
			//Type* v = (Type*)context.objectStart;
			//*v = node.as<Type>();
		}
	};

}

RTTI_DEFINE_EXTERNAL_CLASS(glm::vec2,
    {
        RTTI_DRAWER(editor::Vector2FDrawer<glm::vec2>);

        RTTI_DISPLAY_NAME("Vector2");
		RTTI_SERIALIZER(rtti::TypeSerializerCast_glmVec2);
    });