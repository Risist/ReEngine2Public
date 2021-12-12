#pragma once
#include "rttiEnum.h"
#include "rttiTypeBuilder.h"
#include <functional>


namespace rtti
{
    class EnumBuilder
    {
    public:
        EnumBuilder(EnumType* typeDescriptor, std::function<void(EnumType* typeDescriptor)> initializationFunction)
        {
            initializationFunction(typeDescriptor);
        }
    };
}


/// /brief Registers an enum into the reflection system
/// 
/// inside _Body expression define enum propertities - mostly declare enum options
/// @warrning do not put this macro inside any namespace
#define RTTI_DEFINE_ENUM(_Type, _Body)                                                                          \
namespace rtti                                                                                                  \
{                                                                                                               \
    template<>                                                                                                  \
    struct GetTypeDescriptor<_Type>                                                                             \
    {                                                                                                           \
        TypeBase* operator()()                                                                                      \
        {                                                                                                           \
            static_assert(std::is_enum<_Type>());                                                                   \
            using Type = _Type;                                                                                     \
            static TEnumType<_Type>	    RTTI_DESCRIPTOR_NAME(RE_STRINGIFY(_Type));                                  \
            static EnumBuilder	        RTTI_BUILDER_NAME(&RTTI_DESCRIPTOR_NAME, [](EnumType* RTTI_DESCRIPTOR_NAME)  \
            {                                                                                                       \
                RTTI_DESCRIPTOR_NAME->SetTypeDrawer(RTTI_DESCRIPTOR_NAME);									        \
                _Body                                                                                               \
                RTTI_DESCRIPTOR_NAME->FinalizeInitialization();                                                     \
            });                                                                                                     \
            return &RTTI_DESCRIPTOR_NAME;                                                                           \
        }                                                                                                           \
        TypeBase* operator->()                                                                                      \
		{                                                                                                           \
			return operator()();                                                                                    \
		}                                                                                                           \
        operator TypeBase* ()                                                                                       \
		{                                                                                                           \
			return operator()();                                                                                    \
		}                                                                                                           \
    };                                                                                                              \
}                                                                                                                   \

/// /brief declares a new option into enum
///
/// use it only inside RTTI_DEFINE_ENUM body declaration
/// @optionName is a name of possible enum value
#define RTTI_ENUM_OPTION(optionName) \
    RTTI_DESCRIPTOR_NAME->AddOption(#optionName, Type:: optionName)