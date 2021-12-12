#pragma once

#include "rttiTypeBuilder.h"
#include "rttiClassType.h"
#include "rttiTypeDrawer.h"
#include <functional>

namespace rtti
{
	class ClassBuilder
	{
	public:
		ClassBuilder(ClassType* typeDescriptor, std::function<void(ClassType*)> initializer)
		{
			initializer(typeDescriptor);
		}
	};
}

/// /brief Declares reflected class body propertities with support for dynamic type detection
/// 
/// Put it inside class declaration
/// @Class is a class name (without namespaces)
#define RTTI_POLYMORPHIC_CLASS_BODY(Class)\
	private:\
		static rtti::ClassBuilder RTTI_BUILDER_NAME;\
		friend class rtti::ClassBuilder;\
		friend struct rtti::GetTypeDescriptor<Class>;\
	public:\
		virtual rtti::ClassType* GetType() const { return GetStaticType(); }\
		static rtti::ClassType* GetStaticType();

/// /brief Declares reflected class body propertities for non polimorphic types
/// 
/// Put it inside class declaration
/// @Class is a class name (without namespaces)
#define RTTI_DECLARE_CLASS(Class)\
	private:\
		static rtti::ClassBuilder RTTI_BUILDER_NAME;\
		friend class rtti::ClassBuilder;\
		friend struct rtti::GetTypeDescriptor<Class>;\
	public:\
		rtti::ClassType* GetType() const { return GetStaticType(); }\
		static rtti::ClassType* GetStaticType();


/// /brief Define a class type propertities
/// 
/// inside _Body expression define propertities
/// @warrning do not put this macro inside any namespace, place it in cpp file
#define RTTI_DEFINE_CLASS(Class, _BODY)																			\
	rtti::ClassBuilder Class::RTTI_BUILDER_NAME(Class::GetStaticType(), [](rtti::ClassType*) {});				\
	rtti::ClassType* Class::GetStaticType()																		\
	{																											\
		static rtti::TClassType<Class> RTTI_DESCRIPTOR_NAME(RE_STRINGIFY(Class));								\
		static rtti::ClassBuilder RTTI_BUILDER_NAME(&RTTI_DESCRIPTOR_NAME, [](rtti::ClassType* RTTI_DESCRIPTOR_NAME)	\
		{																										\
			using Type = Class;																					\
			_BODY																								\
			RTTI_DESCRIPTOR_NAME->PrepareCategories();															\
			RTTI_DESCRIPTOR_NAME->FinalizeInitialization();														\
		});																										\
		return &RTTI_DESCRIPTOR_NAME;																			\
	}

/// /brief
///
/// EXPERIMENTAL
#define RTTI_DUPLICATE(virt, BaseClass, additionalInitialization)\
	public:\
		virt std::shared_ptr<BaseClass> Duplicate() const\
		{\
			std::shared_ptr<Entity> duplicated(GetType()->DuplicateNewShared<Entity>(this));\
			additionalInitialization\
			return duplicated;\
		}

/// /brief
///
/// EXPERIMENTAL
#define RTTI_DUPLICATE_THIS(virt, BaseClass, additionalInitialization)\
	RTTI_DUPLICATE(virt, BaseClass, \
		{\
			duplicated->_this = duplicated; \
			additionalInitialization\
		})

/// /brief Externaly registers a class type into the reflection system
/// 
/// in case you can't (or for some reason don't want to) modify class declaration you can register the function externaly
///		downside to that approach is lack of usefull functions and a fact that you need to do type declaration in header file -> longer compilation
/// 
/// Type is not instantiated until first call appears
/// 
/// inside _Body expression define propertities
/// @warrning do not put this macro inside any namespace
#define RTTI_DEFINE_EXTERNAL_CLASS(_Type, _Body)\
namespace rtti\
{\
	template<>\
	struct GetTypeDescriptor<_Type>\
	{\
		TypeBase* operator()()\
		{\
			using Type = _Type;\
			static TClassType<Type>	RTTI_DESCRIPTOR_NAME(RE_STRINGIFY(_Type));\
			static ClassBuilder	RTTI_BUILDER_NAME(&RTTI_DESCRIPTOR_NAME, [](ClassType* RTTI_DESCRIPTOR_NAME)\
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
		operator TypeBase*()\
		{\
			return operator()();\
		}\
	};\
}