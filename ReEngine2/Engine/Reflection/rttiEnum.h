#pragma once
#include "rttiTypeBase.h"
#include "rttiTypeDrawer.h"
#include "rttiSerialization.h"

namespace rtti
{
	// TODO enum flags display

	/// /brief possible value of an enum 
	/// 
	/// as a value is stored int64 but it can represent any of enum sizes
	class EnumOption
	{
	public:
		EnumOption(const char* nativeName, int64 value)
			: _nativeName(nativeName)
			, _displayName(nativeName)
			, _value(value)
		{
		}

		const char* GetNativeName() const { return _nativeName; }

		const char* GetDisplayName() const { return _displayName; }
		EnumOption& SetDisplayName(const char* name) { _displayName = name; return *this; }

		uint64 GetValue() const { return _value; }
		EnumOption& SetValue(uint64 value) { _value = value; return *this; }

	private:
		const char* _nativeName;
		const char* _displayName;
		int64		_value;
	};

	class EnumType : public TypeBase, public TypeDrawerBase, public TypeSerializerBase
	{
	public:
		EnumType(const char* name, uint16 size)
			: TypeBase( name, size)
		{
			SetTypeSerializer(this);
		}

		EnumOption& AddOption(const char* optionName, int64 value)
		{
			_options.push_back(EnumOption(optionName, value));
			return _options.back();
		}

		const EnumOption* GetOption(size_t i) const
		{
			if (i >= _options.size())
			{
				return nullptr;
			}
			return &_options[i];
		}
		const EnumOption* FindOptionByNativeName(const char* nativeName) const
		{
			for (int i = 0; i < _options.size(); ++i)
			{
				if (strcmp(nativeName, _options[i].GetNativeName()) == 0)
				{
					return &_options[i];
				}
			}
			return nullptr;
		}
		const EnumOption* FindOptionByDisplayName(const char* displayName) const
		{
			for (int i = 0; i < _options.size(); ++i)
			{
				if (strcmp(displayName, _options[i].GetDisplayName()) == 0)
				{
					return &_options[i];
				}
			}
			return nullptr;
		}

		size_t GetOptionsCount() const { return _options.size(); }

	public: // Drawing

		virtual bool DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const override;
	
	public: // Serializing

		virtual void Serialize(YAML::Emitter& emitter, SerializationContext serializationContext) const override;
		virtual void Deserialize(const YAML::Node& node, SerializationContext serializationContext) const override;



	private:
		std::vector<EnumOption>	_options;

	};

	/// /brief type template for ease of creation of nonabstract type objects
	template<typename Type>
	class TEnumType : public EnumType
	{
	public:
		TEnumType(const char* name )
			: EnumType(name, sizeof(Type))
		{
			static_assert(std::is_enum<Type>::value);
		}

	protected:
		virtual void* CreateNew() const override
		{
			return new Type();
		}
		virtual void* DuplicateNew(const void* original) const override
		{
			return new Type(*((const Type*)original));// Call copy constructor
		}
	};

}