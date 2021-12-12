#pragma once
#include "rttiAssetType.h"
#include "rttiTypeBuilder.h"
#include "rttiTypeDrawer.h"
#include <functional>

namespace rtti
{
	class AssetBuilder
	{
	public:
		AssetBuilder(AssetType* typeDescriptor, std::function<void(AssetType*)> initializer)
		{
			initializer(typeDescriptor);
		}
	};
}


#define RTTI_FILE_EXTENSION(extension) \
	RTTI_DESCRIPTOR_NAME->SetFileExtension(extension)

// if an asset has native file extension added corresponding asset file will be created 
#define RTTI_NATIVE_FILE_EXTENSION(extension) \
	RTTI_DESCRIPTOR_NAME->AddNativeFileExtension(extension)

/// /brief Declares reflected asset body propertities
/// 
/// Put it inside class declaration
/// @Class is a class name (without namespaces)
#define RTTI_DECLARE_ASSET(Class)\
	private:\
		static rtti::AssetBuilder RTTI_BUILDER_NAME;\
		friend class rtti::AssetBuilder;\
		friend struct rtti::GetTypeDescriptor<Class>;\
	public:\
		virtual rtti::AssetType* GetType() const { return GetStaticType(); }\
		static rtti::AssetType* GetStaticType();

/// /brief Define an asset type propertities
/// 
/// inside _Body expression define propertities
/// @warrning do not put this macro inside any namespace, place it in cpp file
#define RTTI_DEFINE_ASSET(Class, _BODY)																			\
	rtti::AssetBuilder Class::RTTI_BUILDER_NAME(Class::GetStaticType(), [](rtti::AssetType*) {});				\
	rtti::AssetType* Class::GetStaticType()																		\
	{																											\
		static rtti::TAssetType<Class> RTTI_DESCRIPTOR_NAME(RE_STRINGIFY(Class));	\
		static rtti::AssetBuilder RTTI_BUILDER_NAME(&RTTI_DESCRIPTOR_NAME, [](rtti::AssetType* RTTI_DESCRIPTOR_NAME)	\
		{																										\
			using Type = Class;																					\
			_BODY																								\
			RTTI_DESCRIPTOR_NAME->PrepareCategories();															\
			RTTI_DESCRIPTOR_NAME->FinalizeInitialization();														\
		});																										\
		return &RTTI_DESCRIPTOR_NAME;																			\
	}

/// /brief Externaly registers an asset type into the reflection system
/// 
/// in case you can't (or for some reason don't want to) modify class declaration you can register the function externaly
///		downside to that approach is lack of usefull functions and a fact that you need to do type declaration in header file -> longer compilation
/// 
/// Type is not instantiated until first call appears
/// 
/// inside _Body expression define propertities
/// @warrning do not put this macro inside any namespace
#define RTTI_DEFINE_EXTERNAL_ASSET(_Type, _Body)\
namespace rtti\
{\
	template<>\
	struct GetTypeDescriptor<_Type> \
	{\
		TypeBase* operator()()\
		{\
			using Type = _Type;\
			static TAssetType<Type>	RTTI_DESCRIPTOR_NAME(RE_STRINGIFY(_Type));\
			static AssetBuilder	RTTI_BUILDER_NAME(&RTTI_DESCRIPTOR_NAME, [](AssetType* RTTI_DESCRIPTOR_NAME)\
			{\
				_Body \
				RTTI_DESCRIPTOR_NAME->PrepareCategories();															\
				RTTI_DESCRIPTOR_NAME->FinalizeInitialization();														\
			});\
			return &RTTI_DESCRIPTOR_NAME;\
		}\
		TypeBase* operator->()\
		{\
			return operator()();\
		}\
		operator TypeBase* ()\
		{\
			return operator()();\
		}\
	};\
}
