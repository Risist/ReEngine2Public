#pragma once
#include "../Common/Settings.h"
#include "rttiTypeDrawer.h"
#include "rttiFunction.h"
#include "rttiSerialization.h"

#include <functional>

namespace rtti
{
	class TypeBase;
	class ClassType;

	// TODO
	// Clean up - put longer functions into cpp file
	// Property made of 2 functions
	// override set || get function

	class Property : public TypeDrawerBase, public TypeSerializerBase
	{
	public:
		using DrawingConditionType = std::function<bool()>;
	public:
		Property(TypeBase* type, const char* nativeName, const ClassType* owningType, uint32 offset);
		~Property();

		/// /brief returns native name of the varriable
		const char* GetNativeName() const { return _nativeName; }
		
		/// /brief sets display name of the varriable in editor
		Property& SetDisplayName(const char* name) { _displayName = name; return *this; }

		/// /brief returns display name of the varriable in editor
		const char* GetDisplayName() const { return _displayName; }

		/// /brief sets category the property lies in in the editor
		Property& SetCategory(const char* category) { TypeDrawerBase::SetCategory(category); return *this; }
		
		/// GetCategory inherited from TypeDrawerBase

		/// /brief sets property visibility in the editor
		///
		/// by default varriables are vissible
		/// useful when you want to turn visibility and only serialize the property
		Property& SetEditorVissible(bool isEditorVisible) { _editorVissible = isEditorVisible; return *this; }
		
		/// /brief returns if property should be displayed in the editor
		///
		/// by default varriables are vissible
		virtual bool IsEditorVissible() const override 
		{ 
			return _editorVissible 
				&& (!_drawingCondition || _drawingCondition()); 
		}

		/// TODO
		Property& SetDrawingCondition(DrawingConditionType& drawingCondition) { _drawingCondition = drawingCondition; return *this; }

		/// /brief returns tooltip for the property
		const char* GetTooltip() const { return _tooltip; }

		/// /brief sets up tooltip for the property
		Property& SetTooltip(const char* tooltip) { _tooltip = tooltip; return *this; }

		/// TODO
		//Property& SetReadOnly(bool isReadOnly) { _readOnly = isReadOnly; return *this; }
		//bool IsReadOnly() const { return _readOnly; }

		/// /brief turns on/off varriable serialization
		///
		/// by default varriables are serialized
		/// useful when you want to only display the varriable or when property can be recreated when deserializing other varriable
		Property& SetSerialized(bool isSerialized) { _serialized = isSerialized; return *this; }
		
		/// /brief returns if varriable is serialized
		///
		/// by default varriables are serialized
		/// useful when you want to only display the varriable or when property can be recreated when deserializing other varriable
		bool IsSerialized() const { return _serialized; }

		/// /brief returns type of property
		TypeBase* GetPropertyType() const { return _propertyType; }
		
		/// /brief returns type of a class that uses the property
		///
		/// should be always non null, otherwise will assert
		const ClassType* GetOwningClass() const { RE_ASSERT(_owningClass); return _owningClass; }

		/// /brief given adress of the owning class returns address of the property 
		void* GetPropertyAddres(void* classStartAdress) const
		{
			return ((char*)classStartAdress) + _offset;
		}

		/// /brief given property address returns address of an owner
		void* GetOwnerAddres(void* propertyAdress) const
		{
			return ((char*)propertyAdress) - _offset;
		}

		bool operator == (const Property& other) const;


		virtual bool DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const override;

		virtual void Serialize(YAML::Emitter& emitter, SerializationContext serializationContext) const override;
		virtual void Deserialize(const YAML::Node& node, SerializationContext serializationContext) const override;

		
		/// /brief sets override type drawer for the property
		Property& SetOverrideTypeDrawer(TypeDrawerBase* typeDrawer) { _isTypeDrawerAReference = true; _overrideTypeDrawer = typeDrawer; return *this; }
		
		/// /brief	sets up inspector type drawer override to a newly created type drawer
		/// 
		/// @DrawerType - type of a drawer to be created
		template<typename DrawerType, typename ... Args>
		Property& SetOverrideTypeDrawer(Args... args)
		{
			if (_overrideTypeDrawer && !_isTypeDrawerAReference)
				delete _overrideTypeDrawer;

			_overrideTypeDrawer = new DrawerType(args...);
			_isTypeDrawerAReference = false;

			return *this;
		}

		/// /brief returns override type drawer for the property or null if type defined one should be used
		TypeDrawerBase* GetOverrideTypeDrawer() const { return _overrideTypeDrawer; }


		/// /brief sets override type serializer for the property
		Property& SetOverrideTypeSerializer(TypeSerializerBase* typeSerializer) { _isTypeSerializerAReference = true; _overrideTypeSerializer = typeSerializer; return *this; }
		
		/// /brief sets up type serializer
		///
		/// Type serializer is used to define how to serialize the type 
		/// @SerializerType - type of a serializer to be created
		template<typename SerializerType, typename ... Args>
		Property& SetOverrideTypeSerializer(Args... args)
		{
			if (_overrideTypeSerializer && !_isTypeSerializerAReference)
				delete _overrideTypeSerializer;

			_overrideTypeSerializer = new SerializerType(args...);
			_isTypeSerializerAReference = false;

			return *this;
		}

		/// /brief returns override type serializer for the property or null if type defined one should be used
		TypeSerializerBase* GetOverrideTypeSerializer() const { return _overrideTypeSerializer; }

		/// /brief	sets up an override setter for the type varriables
		/// 
		/// Setter function is a function that is called when varriable changes inside editor
		/// If there is no setter varriable is simply set
		/// TODO make possible doing "fake" varriable, created only from setter and getter but not from real varriable
		Property& SetOverridePropertySetter(const FunctionTypeBase* s) { _overrideSetter = s; return *this; }
		
		/// /brief	returns an override setter for the type varriables
		/// 
		/// Setter function is a function that is called when varriable changes inside editor
		/// If there is no setter varriable is simply set
		/// property setter overrides default type behaviour if present
		/// TODO make possible doing "fake" varriable, created only from setter and getter but not from real varriable
		const FunctionTypeBase* GetOverridePropertySetter() const { return _overrideSetter;  }

		/// /brief	sets up an override getter for the type varriables
		/// 
		/// Setter function is a function that is called when varriable changes inside editor
		/// If there is no setter varriable is simply set
		/// property getter overrides default type behaviour if present
		/// TODO make possible doing "fake" varriable, created only from setter and getter but not from real varriable
		Property& SetOverridePropertyGetter(const FunctionTypeBase* s) { _overrideGetter = s; return *this; }

		/// /brief	returns an override getter for the type varriables
		/// 
		/// Setter function is a function that is called when varriable changes inside editor
		/// If there is no setter varriable is simply set
		/// property getter overrides default type behaviour if present
		/// TODO make possible doing "fake" varriable, created only from setter and getter but not from real varriable
		const FunctionTypeBase* GetOverridePropertyGetter() const { return _overrideGetter; }

	private:
		// TODO we need a mechanism of passing right object to the condition
		// we could do so by void* , but then the user will have to cast it manually, what is not very handy
		DrawingConditionType	_drawingCondition;

		TypeBase*				_propertyType;				
		const ClassType*		_owningClass;				

		TypeDrawerBase*			_overrideTypeDrawer;
		TypeSerializerBase*		_overrideTypeSerializer;

		const FunctionTypeBase*		_overrideGetter;
		const FunctionTypeBase*		_overrideSetter;


		const char*				_nativeName;			
		const char*				_displayName;			
		const char*				_tooltip;

		uint32					_offset;				

		uint8					_readOnly : 1;			 
		uint8					_serialized : 1;

		bool _isTypeSerializerAReference : 1;
		bool _isTypeDrawerAReference : 1;
	};
}