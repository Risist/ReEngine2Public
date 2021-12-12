#pragma once
#include "rttiTypeBuilder.h"
#include "rttiClassTraitsMacros.h"
#include "BasicDrawers.h"

//////////////// Float

RTTI_DEFINE(void, {
    RTTI_DISPLAY_NAME("Void");
    RTTI_DRAWER(editor::VoidDrawer);
    RTTI_SERIALIZER(rtti::TypeSerializerNoSerialize);
})

RTTI_DEFINE(float, {
    RTTI_DISPLAY_NAME("Float32");
    RTTI_DRAWER(editor::FloatDrawer<float>);
    RTTI_SERIALIZER(TTypeSerializerCast<float>);
})

RTTI_DEFINE(double, {
    RTTI_DISPLAY_NAME("Float64");
    RTTI_DRAWER(editor::FloatDrawer<double>);
    RTTI_SERIALIZER(TTypeSerializerCast<double>);
})

//////////////// Int

RTTI_DEFINE(int8, {
    RTTI_DISPLAY_NAME("Int8");
    RTTI_DRAWER(editor::IntDrawer<int8>);
    RTTI_SERIALIZER(TTypeSerializerCast<int8>);
})
    
RTTI_DEFINE(int16, {
    RTTI_DISPLAY_NAME("Int16");
    RTTI_DRAWER(editor::IntDrawer<int16>);
    RTTI_SERIALIZER(TTypeSerializerCast<int16>);
})

RTTI_DEFINE(int32, {
    RTTI_DISPLAY_NAME("Int32");
    RTTI_DRAWER(editor::IntDrawer<int32>);
    RTTI_SERIALIZER(TTypeSerializerCast<int32>);
})

RTTI_DEFINE(int64, {
    RTTI_DISPLAY_NAME("Int64");
    RTTI_DRAWER(editor::IntDrawer<int64>);
    RTTI_SERIALIZER(TTypeSerializerCast<int64>);
})

RTTI_DEFINE(uint8, {
    RTTI_DISPLAY_NAME("Unsigned Int8");
    RTTI_DRAWER(editor::IntDrawer<uint8>);
    RTTI_SERIALIZER(TTypeSerializerCast<uint8>);
})
    
RTTI_DEFINE(uint16, {
    RTTI_DISPLAY_NAME("Unsigned Int16");
    RTTI_DRAWER(editor::IntDrawer<uint16>);
    RTTI_SERIALIZER(TTypeSerializerCast<uint16>);
})

RTTI_DEFINE(uint32, {
    RTTI_DISPLAY_NAME("Unsigned Int32");
    RTTI_DRAWER(editor::IntDrawer<uint32>);
    RTTI_SERIALIZER(TTypeSerializerCast<uint32>);
})

RTTI_DEFINE(uint64, {
    RTTI_DISPLAY_NAME("Unsigned Int64");
    RTTI_DRAWER(editor::IntDrawer<uint64>);
    RTTI_SERIALIZER(TTypeSerializerCast<uint64>);
})

//////////////// Bool

RTTI_DEFINE(bool, {
    RTTI_DISPLAY_NAME("Bool");
    RTTI_DRAWER(editor::BoolDrawer);
    RTTI_SERIALIZER(TTypeSerializerCast<bool>);
})

//////////////// String

RTTI_DEFINE(std::string, {
    RTTI_DISPLAY_NAME("String");
    RTTI_DRAWER(editor::StringDrawer<std::string>);
    RTTI_SERIALIZER(TTypeSerializerCast<std::string>);
})

// TODO
//RTTI_DEFINE(std::filesystem::path, {
//    RTTI_DISPLAY_NAME("Path");
//    RTTI_DRAWER(editor::StringDrawer<std::filesystem::path>);
//    RTTI_SERIALIZER(TTypeSerializerCast<std::string>);
//})


//////////////// External



