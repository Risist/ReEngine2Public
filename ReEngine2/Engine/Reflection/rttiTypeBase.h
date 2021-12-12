#pragma once
#include <memory>
#include <vector>
#include <assert.h>
#include <iostream>
#include <filesystem>
#include "../Common/Settings.h"

namespace rtti
{
	class TypeDrawerBase;
	class TypeSerializerBase;
	class FunctionTypeBase;

	class TypeBase
	{
	public:
		TypeBase(const char* name, uint16 size);
		~TypeBase();

		/// /brief	any final operations that have to be applied after type is configured
		virtual void FinalizeInitialization();

		/// /brief returns native name of the type
		///
		/// Native name is the name of type with inclusion of any namespaces
		const char* GetNativeName() const { return _nativeName; }

		/// /brief returns display name of the type
		///
		/// Display name is a name that will be displayed in editor
		const char* GetDisplayName() const { return _displayName; }
		
		/// /brief changes display name of the type
		///
		/// Display name is a name that will be displayed in editor
		void SetDisplayName(const char* displayName) { _displayName = displayName; }


		/// /brief returns tooltip for the type
		const char* GetTooltip() const { return _tooltip; }
		
		/// /brief sets up tooltip for the type
		void SetTooltip(const char* tooltip) { _tooltip = tooltip; }

		/// /brief returns size of the type (aka result of sizeof operator)
		size_t GetTypeSize() const { return _size; }

		/// /brief creates a shared_ptr<BaseType> with a new object of the type
		///
		/// TODO think about common memory pool, or way to set it up
		template<typename BaseType>
		std::shared_ptr<BaseType> CreateNewShared() const
		{
			return std::move(std::shared_ptr<BaseType>((BaseType*)CreateNew()));
		}

		/// /brief creates an unique_ptr<BaseType> with a new object of the type
		///
		/// TODO think about common memory pool, or way to set it up
		template<typename BaseType>
		std::unique_ptr<BaseType> CreateNewUnique() const
		{
			return std::move(std::unique_ptr<BaseType>((BaseType*)CreateNew()));
		}

		/// /brief	creates a shared_ptr<BaseType> with a new object of the type, 
		///			being a copy of provided varriable
		///
		/// TODO think about common memory pool, or way to set it up
		template<typename BaseType>
		std::shared_ptr<BaseType> DuplicateNewShared(const BaseType* original) const
		{
			return std::move(std::shared_ptr<BaseType>((BaseType*)DuplicateNew((void*)original)));
		}

		/// /brief	creates an unique_ptr<BaseType> with a new object of the type, 
		///			being a copy of provided varriable
		///
		/// TODO think about common memory pool, or way to set it up
		template<typename BaseType>
		std::unique_ptr<BaseType> DuplicateNewUnique(const BaseType* original) const
		{
			return std::move(std::unique_ptr<BaseType>((BaseType*)DuplicateNew(original)));
		}

		/// /brief	returns inspector type drawer 
		/// 
		/// TODO what about constify the result?
		TypeDrawerBase* GetTypeDrawer() const { return _typeDrawer; }
		
		/// /brief	sets up inspector type drawer to a provided reference
		/// 
		/// TODO what about constify @typeDrawer ?
		void SetTypeDrawer(TypeDrawerBase* typeDrawer) { _typeDrawer = typeDrawer; _isTypeDrawerAReference = true; }

		/// /brief	sets up inspector type drawer to a newly created type drawer
		/// 
		/// @DrawerType - type of a drawer to be created
		template<typename DrawerType, typename ... Args>
		void SetTypeDrawer(Args... args) 
		{ 
			if (_typeDrawer && !_isTypeDrawerAReference)
				delete _typeDrawer;

			_typeDrawer = new DrawerType(args...); 
			_isTypeDrawerAReference = false; 
		}

		/// /brief	returns a setter for the type varriables
		/// 
		/// Setter function is a function that is called when varriable changes inside editor
		/// If there is no setter varriable is simply set
		/// TODO make possible doing "fake" varriable, created only from setter and getter but not from real varriable
		FunctionTypeBase* GetDefaultSetter() const { return _defaultSetter; }

		/// /brief	sets up a setter for the type varriables
		/// 
		/// Setter function is a function that is called when varriable changes inside editor
		/// If there is no setter varriable is simply set
		/// TODO make possible doing "fake" varriable, created only from setter and getter but not from real varriable
		void SetDefaultSetter(FunctionTypeBase* setter) { _defaultSetter = setter; }

		/// /brief	returns a getter for the type varriables
		/// 
		/// Getter function is a function that is called to obtain varriable value inside editor
		/// If there is no getter varriable is simply returned
		/// TODO make possible doing "fake" varriable, created only from setter and getter but not from real varriable
		FunctionTypeBase* GetDefaultGetter() const { return _defaultGetter; }

		/// /brief	returns a getter for the type varriables
		/// 
		/// Getter function is a function that is called to obtain varriable value inside editor
		/// If there is no getter varriable is simply returned
		/// TODO make possible doing "fake" varriable, created only from setter and getter but not from real varriable
		void SetDefaultGetter(FunctionTypeBase* getter) { _defaultGetter = getter; }

	public: // Serialization

		/// /brief sets up type serializer
		///
		/// Type serializer is used to define how to serialize the type 
		void SetTypeSerializer(TypeSerializerBase* typeSerializer) { _typeSerializer = typeSerializer; _isTypeSerializerAReference = true; }
		
		/// /brief sets up type serializer
		///
		/// Type serializer is used to define how to serialize the type 
		/// @SerializerType - type of a serializer to be created
		template<typename SerializerType, typename ... Args>
		void SetTypeSerializer(Args... args)
		{
			if (_typeSerializer && !_isTypeSerializerAReference)
				delete _typeSerializer;

			_typeSerializer = new SerializerType(args...);
			_isTypeSerializerAReference = false;
		}

		/// /brief returns type serializer
		///
		/// Type serializer is used to define how to serialize the type 
		TypeSerializerBase* GetTypeSerializer() const { return _typeSerializer; }

		/// /brief serializes varriable from context to the given file
		///
		/// End user should not need to use this function, use rtti::Draw function instead
		bool SerializeToFile(const std::filesystem::path& filePath, const class SerializationContext& context);
		
		/// /brief deserializes varriable from context to the given file
		///
		/// End user should not need to use this function, use rtti::Draw function instead
		bool DeserializeFromFile(const std::filesystem::path& filePath, const class SerializationContext& context) const;

		/// /brief serializes varriable to context to the given string
		///
		/// End user should not need to use this function, use rtti::Draw function instead
		bool SerializeToString(std::string& str, const class SerializationContext& context);
		
		/// /brief deserializes varriable from context to the given file
		///
		/// End user should not need to use this function, use rtti::Draw function instead
		bool DeserializeFromString(const std::string_view& str, const class SerializationContext& context) const;

	protected:

		virtual void* CreateNew() const = 0;
		virtual void* DuplicateNew(const void* original) const = 0;

		const char* _displayName;
		const char* _nativeName;
		const char* _tooltip;

		uint16		_size;

		TypeDrawerBase* _typeDrawer;
		TypeSerializerBase* _typeSerializer;
		FunctionTypeBase* _defaultGetter;
		FunctionTypeBase* _defaultSetter;

		bool _isTypeDrawerAReference : 1;
		bool _isTypeSerializerAReference : 1;
	};

	/// /brief type template for ease of creation of nonabstract type objects
	template<typename Type>
	class TType : public TypeBase
	{
	public:
		TType(const char* name)
			: TypeBase(name, sizeof(Type))
		{
		}

	protected:
		virtual void* CreateNew() const override
		{
			return new Type();
		}
		virtual void* DuplicateNew(const void* original) const override
		{
			return new Type(*((const Type*)original));
		}
	};

	/// \brief specialization for void, cause we cannot instantiate it
	template<>
	class TType<void> : public TypeBase
	{
	public:
		TType(const char* name)
			: TypeBase(name, sizeof(0))
		{
		}

	protected:
		virtual void* CreateNew() const override
		{
			return nullptr;
		}
		virtual void* DuplicateNew(const void* original) const override
		{
			return nullptr;
		}
	};

	/// /brief main accessor of type descriptor data
	///
	/// you can treat this struct mostly as a function
	/// GetTypeDescriptor is implemented as a struct to support default nested types (as for std::vector)
	template<typename Type>
	struct GetTypeDescriptor
	{
		TypeBase* operator()()
		{
			return Type::GetStaticType();
		}

		TypeBase* operator->()
		{
			return operator()();
		}
		operator TypeBase* ()
		{
			return operator()();
		}
	};

	
}