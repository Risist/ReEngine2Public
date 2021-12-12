#pragma once
#include "rttiTypeBase.h"
#include "rttiArray.h"
#include <functional>

#define RTTI_DESCRIPTOR_NAME _typeDescriptor
#define RTTI_BUILDER_NAME _typeBuilder


namespace rtti
{
	class TypeBuilder
	{
	public:
		TypeBuilder(TypeBase* typeDescriptor, std::function<void(TypeBase* typeDescriptor)> initializationFunction)
		{
			initializationFunction(typeDescriptor);
		}
	};

	class ArrayBuilder
	{
	public:
		ArrayBuilder(ArrayTypeBase* typeDescriptor, std::function<void(ArrayTypeBase* typeDescriptor)> initializationFunction)
		{
			initializationFunction(typeDescriptor);
		}
	};

	/// \brief Serializes type value to a given file path
	/// 
	/// Deserialized type has to be registered in the reflection system 
	/// @see RTTI_DEFINE macro family)
	/// To specify serialization behaviour set TypeSerializer for a type / property
	template<typename Type>
	void SerializeToFile(const std::filesystem::path& filePath, const Type& objToSerialize)
	{
		rtti::SerializationContext serializationContext((Type*)&objToSerialize);
		RE_CHECK(rtti::GetTypeDescriptor<Type>())->SerializeToFile(filePath, serializationContext);
	}

	/// \brief Deserializes type value from a given file path
	/// 
	/// Deserialized type has to be registered in the reflection system 
	/// @see RTTI_DEFINE macro family)
	/// To specify serialization behaviour set TypeSerializer for a type / property
	template<typename Type>
	void DeserializeFromFile(const std::filesystem::path& filePath, Type& objToDeserialize)
	{
		rtti::SerializationContext serializationContext(&objToDeserialize);
		RE_CHECK(rtti::GetTypeDescriptor<Type>())->DeserializeFromFile(filePath, serializationContext);
	}

	/// \brief Serializes type value from a given string
	/// 
	/// Serialized type has to be registered in the reflection system 
	/// @see RTTI_DEFINE macro family)
	/// To specify serialization behaviour set TypeSerializer for a type / property
	/// 
	/// @param s place to put serialized object in yaml format
	template<typename Type>
	void SerializeToString(std::string& s, const Type& objToSerialize)
	{
		rtti::SerializationContext serializationContext((Type*)&objToSerialize);
		RE_CHECK(rtti::GetTypeDescriptor<Type>())->SerializeToString(s, serializationContext);
	}

	/// \brief Deserializes type value from a given string
	/// 
	/// Deserialized type has to be registered in the reflection system 
	/// @see RTTI_DEFINE macro family)
	/// To specify serialization behaviour set TypeSerializer for a type / property
	template<typename Type>
	void DeserializeFromString(const std::string_view& yamlSource, Type& objToDeserialize)
	{
		rtti::SerializationContext serializationContext(&objToDeserialize);
		RE_CHECK(rtti::GetTypeDescriptor<Type>())->DeserializeFromString(yamlSource, serializationContext);
	}

	inline void DrawByType(const rtti::TypeBase* type, void* objToDraw, const char* fieldName = "", const char* imGuiId = "", int initialOffset = 0)
	{
		RE_ASSERT(type);
		RE_ASSERT(objToDraw);

		TypeDrawingContext context(objToDraw, imGuiId);
		context.offset = initialOffset;
		context.fieldName = fieldName;

		type->GetTypeDrawer()->DrawType(context);
	}

	/// \brief Deserializes type value from a given string
	/// 
	/// Deserialized type has to be registered in the reflection system 
	/// @see RTTI_DEFINE macro family)
	template<typename Type>
	void Draw(Type& objToDraw, const char* fieldName = "", const char* imGuiId = "", int initialOffset = 0)
	{
		rtti::TypeBase* type;
		if constexpr (std::is_member_function_pointer<decltype(&Type::GetType)>::value)
		{
			type = objToDraw.GetType();
		}
		else
		{
			type = RE_CHECK(GetTypeDescriptor<Type>());
		}

		DrawByType(type, &objToDraw, fieldName, imGuiId, initialOffset);
	}

	
}


/// /brief Registers a type into the reflection system
/// 
/// inside _Body expression define propertities
/// @warrning do not put this macro inside any namespace
#define RTTI_DEFINE(_Type, _Body)\
namespace rtti\
{\
	template<>\
	struct GetTypeDescriptor<_Type>\
	{\
		TypeBase* operator()()\
		{\
			using Type = _Type;\
			static TType<Type>	RTTI_DESCRIPTOR_NAME(RE_STRINGIFY(_Type));\
			static TypeBuilder	RTTI_BUILDER_NAME(&RTTI_DESCRIPTOR_NAME, [](TypeBase* RTTI_DESCRIPTOR_NAME)\
			{\
				RTTI_SERIALIZER(rtti::TypeSerializerNoSerialize);  \
				_Body \
				RTTI_DESCRIPTOR_NAME->FinalizeInitialization();\
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

/// /brief Registers a type into the reflection system
/// 
/// inside _Body expression define propertities
/// @warrning do not put this macro inside any namespace
#define RTTI_DEFINE_NESTED(OuterType, InnerType, _Body)\
namespace rtti\
{\
	template<typename InnerType>\
	struct GetTypeDescriptor<OuterType<InnerType>>\
	{\
		TypeBase* operator()()\
		{\
			using Type = OuterType<InnerType>;\
			static TType<Type>	RTTI_DESCRIPTOR_NAME(RE_STRINGIFY(Type));\
			static TypeBuilder	RTTI_BUILDER_NAME(&RTTI_DESCRIPTOR_NAME, [](TypeBase* RTTI_DESCRIPTOR_NAME)\
			{\
				RTTI_SERIALIZER(rtti::TypeSerializerNoSerialize);  \
				_Body \
				RTTI_DESCRIPTOR_NAME->FinalizeInitialization();\
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