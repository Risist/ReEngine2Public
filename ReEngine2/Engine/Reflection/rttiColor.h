#pragma once
#include "rttiTypeDrawer.h"
#include "rttiSerialization.h"

/////////////// Color

/*RTTI_DEFINE(sf::Color, {
	RTTI_DISPLAY_NAME("Color");
	RTTI_DRAWER(editor::ColorDrawer);
	RTTI_SERIALIZER(rtti::TTypeSerializerCast<sf::Color>);
})*/

RTTI_DEFINE_EXTERNAL_CLASS(sf::Color,
    {
        RTTI_DISPLAY_NAME("Color");
        RTTI_DRAWER(editor::ColorDrawer);

        // for serialization
        RTTI_PROPERTY(r);
        RTTI_PROPERTY(g);
        RTTI_PROPERTY(b);
        RTTI_PROPERTY(r);
    });