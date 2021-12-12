#pragma once
#include "rttiTypeBuilder.h"

/// /brief declares base class of a given class
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// Use only in class like types (what means also Assets )
/// will add all drawers of the base class, so place it in the order you want to display varriables
#define RTTI_BASE(BaseClass)								\
	static_assert(std::is_class<BaseClass>::value);			\
	static_assert(std::is_base_of<BaseClass,Type>::value);	\
	RTTI_DESCRIPTOR_NAME->SetBaseType((rtti::ClassType*)rtti::GetTypeDescriptor<BaseClass>()())
	
/// /brief declares a property of a class
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// Use only in class like types (what means also Assets )
/// will place inspector drawer in the order you declare propertities
/// @returns created property, allows chain setup of property parameters if needed
/// for more information about property @see rtti::Property
#define RTTI_PROPERTY(propertyName)									\
	RTTI_DESCRIPTOR_NAME->AddProperty(								\
		(rtti::TypeBase*)rtti::GetTypeDescriptor<decltype(Type::propertyName)>(),	\
		RE_STRINGIFY(propertyName),									\
		offsetof(Type, propertyName)								\
	)

/// /brief declares a member function of a class
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// Use only in class like types (what means also Assets )
/// @returns created function, allows chain setup of property parameters if needed
/// for more information about property @see rtti::FunctionBase
#define RTTI_MEMBER_FUNCTION(function)																		\
	static rtti::TMemberFunctionType functionType_ ## function (RE_STRINGIFY(function), &Type::function);	\
	RTTI_DESCRIPTOR_NAME->AddFunction(&functionType_ ## function)											\

/// /brief adds a getter to a property @propertyName
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// Use only in class like types (what means also Assets )
/// Creates property if needed 
/// @returns added function
#define RTTI_PROPERTY_GETTER(propertyName, functionName)															\
	auto propertyName ## _Getter_ ## functionName = RTTI_DESCRIPTOR_NAME->FindFunctionByNativeName(#functionName);	\
	if(! propertyName ## _Getter_ ## functionName)																	\
	{																												\
		RTTI_MEMBER_FUNCTION(functionName);																			\
		propertyName ## _Getter_ ## functionName = RTTI_DESCRIPTOR_NAME->FindFunctionByNativeName(#functionName);	\
	}																												\
	RTTI_DESCRIPTOR_NAME->FindPropertyByNativeName(#propertyName)->SetOverridePropertyGetter(propertyName ## _Getter_ ## functionName);

/// /brief adds a setter to a property @propertyName
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// Use only in class like types (what means also Assets )
/// Creates property if needed 
/// @returns added function
#define RTTI_PROPERTY_SETTER(propertyName, functionName)															\
	auto propertyName ## _Setter_ ## functionName = RTTI_DESCRIPTOR_NAME->FindFunctionByNativeName(#functionName);	\
	if(! propertyName ## _Setter_ ## functionName)																	\
	{																												\
		RTTI_MEMBER_FUNCTION(functionName);																			\
		propertyName ## _Setter_ ## functionName = RTTI_DESCRIPTOR_NAME->FindFunctionByNativeName(#functionName);	\
	}																												\
	RTTI_DESCRIPTOR_NAME->FindPropertyByNativeName(#propertyName)->SetOverridePropertySetter(propertyName ## _Setter_ ## functionName);

/// /brief sets a default setter for the type
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// Creates property if needed 
/// @returns added function
/// TODO move to a file rttiTypeTraitsMacros
#define RTTI_DEFAULT_SETTER(functionName)																			\
	auto _DefaultSetter_ ## functionName = RTTI_DESCRIPTOR_NAME->FindFunctionByNativeName(#functionName);			\
	if(! _DefaultSetter_ ## functionName)																			\
	{																												\
		RTTI_MEMBER_FUNCTION(functionName);																			\
		_DefaultSetter_ ## functionName = RTTI_DESCRIPTOR_NAME->FindFunctionByNativeName(#functionName);			\
	}																												\
	RTTI_DESCRIPTOR_NAME->SetDefaultSetter(_DefaultSetter_ ## functionName);

/// /brief sets a default getter for the type
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// Creates property if needed 
/// @returns added function
/// TODO move to a file rttiTypeTraitsMacros
#define RTTI_DEFAULT_GETTER(functionName)																			\
	auto _DefaultGetter_ ## functionName = RTTI_DESCRIPTOR_NAME->FindFunctionByNativeName(#functionName);			\
	if(! _DefaultGetter_ ## functionName)																			\
	{																												\
		RTTI_MEMBER_FUNCTION(functionName);																			\
		_DefaultGetter_ ## functionName = RTTI_DESCRIPTOR_NAME->FindFunctionByNativeName(#functionName);			\
	}																												\
	RTTI_DESCRIPTOR_NAME->SetDefaultGetter(_DefaultGetter_ ## functionName);


/////////////// Drawing

/// /brief sets the name type will be displayed in editor with
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// TODO move to a file rttiTypeTraitsMacros
#define RTTI_DISPLAY_NAME(_Name)				\
	RTTI_DESCRIPTOR_NAME->SetDisplayName(_Name)

/// /brief sets default drawer type will use to draw it's values
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// TODO move to a file rttiTypeTraitsMacros
#define RTTI_DRAWER(TypeDrawer, ...)			\
	RTTI_DESCRIPTOR_NAME->SetTypeDrawer<TypeDrawer>(__VA_ARGS__)

/// /brief sets default serializer type will use to serialize it's values
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// TODO move to a file rttiTypeTraitsMacros
#define RTTI_SERIALIZER(TypeSerializer, ...) \
	RTTI_DESCRIPTOR_NAME->SetTypeSerializer<TypeSerializer>(__VA_ARGS__ )

/// /brief sets default category properties will be added to
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// Use only in class like types (what means also Assets )
#define RTTI_CATEGORY(CategoryName) RTTI_DESCRIPTOR_NAME->SetCategory(CategoryName)

/// /brief sets tooltip to the class type
///
/// To be used inside RTTI_DEFINE_XXXX bodies
/// TODO move to a file rttiTypeTraitsMacros
#define RTTI_TOOLTIP(tooltip) RTTI_DESCRIPTOR_NAME->SetTooltip(tooltip)