#pragma once
#include "rttiTypeBase.h"
#include "rttiProperty.h"
#include "rttiFunction.h"


namespace rtti
{
	class ClassType : public TypeBase, public CompoundTypeDrawer, public TypeSerializerBase
	{
	public:
		ClassType(const char* name, uint16 size);

	public: // Inheritance

		/// /brief sets base type of the class
		void SetBaseType(ClassType* baseType)
		{
			_baseType = baseType;
			if (baseType)
			{
				for (auto& baseProperty : baseType->_properties)
				{
					_properties.push_back(baseProperty);
				}
				for (auto& baseDrawer : baseType->_typeDrawingEntries)
				{
					_typeDrawingEntries.push_back(baseDrawer);
				}

				baseType->AddDerived(this);
			}
		}
		
		/// /brief returns base type of the class
		ClassType* GetBaseType() const { return _baseType; }

		/// /brief sets if the class is or isn't abstract
		///
		/// abstract classes wont apear in editor
		/// TODO think about make the function protected
		void SetAsAbstract(bool isAbstract)
		{
			_isAbstract = isAbstract;
		}

		/// /brief returns if the class is declared as and abstract one
		bool IsAbstract() const { return _isAbstract; }

		/// /brief returns ith class directly deriving from this one
		///
		/// usefull for iterating over class types
		const ClassType* GetDirectlyDerived(size_t i) const
		{
			return _derived[i];
		}
		
		/// /brief returns how many classes are directly derived from this one 
		///
		/// usefull for iterating over class types
		size_t GetDirectlyDerivedCount() const
		{
			return _derived.size();
		}

		/// /brief returns ith class (directly or not) deriving from this one
		///
		/// usefull for iterating over class types
		const ClassType* GetDerivedAll(int i) const
		{
			assert(i < _derivedAll.size());
			return _derivedAll[i];
		}

		/// /brief returns ith class (directly or not) deriving from this one
		///
		/// usefull for iterating over class types
		size_t GetDerivedAllCount() const
		{
			return _derivedAll.size();
		}

		/// /brief checks if class is or is deriving from type @Ty
		template<typename Ty>
		bool IsA() const;

		/// /brief checks if class is or is deriving from type @otherType
		bool IsA(const ClassType* otherType) const
		{
			if (otherType == this)
				return true;
			if (_baseType == nullptr)
				return false;
			if (_baseType == otherType)
				return true;

			return _baseType->IsA(otherType);
		}

	public: ////// Properties

		/// /brief returns ith property of the class
		Property* GetProperty(size_t i)
		{
			if (i >= _properties.size())
			{
				return nullptr;
			}

			return _properties[i].get();
		}

		/// /brief returns ith property of the class
		const Property* GetProperty(size_t i) const
		{
			if (i >= _properties.size())
			{
				return nullptr;
			}

			return _properties[i].get();
		}

		/// /brief returns property having native name @nativeName
		Property* FindPropertyByNativeName(const char* nativeName)
		{
			for (int i = 0; i < _properties.size(); ++i)
			{
				if (strcmp(nativeName, _properties[i]->GetNativeName()) == 0)
				{
					return _properties[i].get();
				}
			}
			return nullptr;
		}

		/// /brief returns property having native name @nativeName
		const Property* FindPropertyByNativeName(const char* nativeName) const
		{
			for (int i = 0; i < _properties.size(); ++i)
			{
				if (strcmp(nativeName, _properties[i]->GetNativeName()) == 0)
				{
					return _properties[i].get();
				}
			}
			return nullptr;
		}

		/// /brief returns property having display name @displayName
		Property* FindPropertyByDisplayName(const char* displayName)
		{
			for (int i = 0; i < _properties.size(); ++i)
			{
				if (strcmp(displayName, _properties[i]->GetDisplayName()) == 0)
				{
					return _properties[i].get();
				}
			}
			return nullptr;
		}

		/// /brief returns property having display name @displayName
		const Property* FindPropertyByDisplayName(const char* displayName) const
		{
			for (int i = 0; i < _properties.size(); ++i)
			{
				if (strcmp(displayName, _properties[i]->GetDisplayName()) == 0)
				{
					return _properties[i].get();
				}
			}
			return nullptr;
		}

		/// /brief returns how many properties have this class declared
		size_t GetPropertyCount() const { return _properties.size(); }

		/// /brief declares a new property to the class
		///
		/// @returns added property value to allow chained setup of it's parameters
		/// @see RTTI_PROPERTY macro, it is adviced to use it instead of directly calling the function
		Property& AddProperty(TypeBase* type, const char* nativeName, uint32 offset)
		{
			for (int i = 0; i < _properties.size(); ++i)
			{
				if (strcmp(nativeName, _properties[i]->GetNativeName()) == 0)
				{
					// remove the old property drawer
					for (int j = 0; j < _typeDrawingEntries.size(); ++j)
					{
						if (_typeDrawingEntries[j] == _properties[i].get())
						{
							_typeDrawingEntries.erase(_typeDrawingEntries.begin() + j);
							break;
						}
					}

					// remove old property
					_properties.erase(_properties.begin() + i);
					break;
				}
			}

			_properties.push_back(std::make_shared<Property>(type, nativeName, this, offset));
			AddDrawer(_properties.back().get());
			return *_properties.back();
		}


	public: // function

		/// /brief declares a new function to the class
		/// 
		/// @returns added function value to allow chained setup it's parameters
		/// @see RTTI_MEMBER_FUNCTION, it is adviced to use the function instead of directly calling
		FunctionTypeBase& AddFunction(FunctionTypeBase* function)
		{
			_functions.push_back(function);
			return *function;
		}
		
		/// /brief returns id'th function of the class
		/// 
		/// @returns nullptr if id isn't in function number range
		const FunctionTypeBase* GetFunction(size_t id) const
		{
			if (id >= _functions.size())
			{
				return nullptr;
			}
			return _functions[id];
		}

		/// /brief returns function with @nativeName
		/// 
		/// @returns nullptr if there is no such a function
		const FunctionTypeBase* FindFunctionByNativeName(const char* nativeName)
		{
			for (int i = 0; i < _functions.size(); ++i)
			{
				if (strcmp(nativeName, _functions[i]->GetNativeName()) == 0)
				{
					return _functions[i];
				}
			}
			return nullptr;
		}

		/// /brief returns function with @displayName
		/// 
		/// @returns nullptr if there is no such a function
		const FunctionTypeBase* FindFunctionByDisplayName(const char* displayName)
		{
			for (int i = 0; i < _functions.size(); ++i)
			{
				if (strcmp(displayName, _functions[i]->GetDisplayName()) == 0)
				{
					return _functions[i];
				}
			}
			return nullptr;
		}

		/// /brief returns how many functions are declared to the class
		size_t GetFunctionCount() const { return _functions.size(); }

	public: // Drawing

		virtual bool DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const override;

	public: // Serializing

		virtual void Serialize(YAML::Emitter& emitter, SerializationContext serializationContext) const override;
		virtual void Deserialize(const YAML::Node& node, SerializationContext serializationContext) const override;

	protected:

		void AddDerived(ClassType* derived)
		{
			if (std::find(_derived.begin(), _derived.end(), derived) != _derived.end())
				return; // already added;

			derived->_baseType = this;
			_derived.push_back(derived);
			AddToAllDerrived(derived);
		}
		void AddToAllDerrived(ClassType* derived)
		{
			if (std::find(_derivedAll.begin(), _derivedAll.end(), derived) != _derivedAll.end())
				return; // already added;
			_derivedAll.push_back(derived);

			if (_baseType)
			{
				_baseType->AddToAllDerrived(derived);
			}
		}
	
	private:
		ClassType* _baseType;

		bool _isAbstract = false;

		std::vector<std::shared_ptr<Property>> _properties;
		std::vector<ClassType*> _derived;
		std::vector<ClassType*> _derivedAll;
		std::vector<FunctionTypeBase*> _functions;
	};

	/// /brief type template for ease of creation of nonabstract type objects
	template<typename Type>
	class TClassType : public ClassType
	{
	public:
		TClassType(const char* name)
			: ClassType(name, sizeof(Type))
		{
			static_assert(std::is_class<Type>::value);
			SetAsAbstract(std::is_abstract<Type>::value);
		}

	protected:
		virtual void* CreateNew() const override
		{
			if constexpr (std::is_abstract<Type>::value == false)
				return new Type;
			else
				return nullptr;
		}
		virtual void* DuplicateNew(const void* original) const override
		{
			if constexpr (std::is_abstract<Type>::value == false)
				return new Type(*((const Type*)original));// Call copy constructor
			else
				return nullptr;
		}
	};

	/// /brief accessor of class types
	template<typename Type>
	inline ClassType* GetClassDescriptor()
	{
		RE_ASSERT((dynamic_cast<ClassType*>((TypeBase*)GetTypeDescriptor<Type>()) != nullptr));
		return (ClassType*)(TypeBase*)GetTypeDescriptor<Type>();
	}
}